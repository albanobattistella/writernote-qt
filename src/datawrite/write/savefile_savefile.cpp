#include "../savefile.h"
#include "../source_read_ext.h"
#include "../../utils/common_error_definition.h"
#include "../../images/fromimage.h"
#include "../../frompdf/frompdf.h"
#include "../../currenttitle/document.h"
#include "../../indice_class.h"

#define SAVE_STRINGA(x, y) if(save_string(x, y) != OK) goto delete_;

static int save_string(zip_source_t *, const char *);
static void setCurrentVersion(Document *data);

#include <QFile>

/*
 * the function save the copybook and all it's data
 * if save_audio == true -> save also the audio
*/

int savefile::savefile_check_file(){
    setCurrentVersion(currenttitle);

    int error, temp, len, i, check;
    zip_error_t errore;

    fromimage::load_res res_img;
    frompdf::load_res res_pdf;

    zip_t *filezip;
    zip_source_t *file;

    filezip = zip_open(path->toUtf8().constData(), ZIP_CREATE, &error);

    if(!filezip)
        return ERROR;

    file = zip_source_buffer_create(0, 0, 0, &errore);
    if(!file){
        zip_close(filezip);
        return ERROR;
    }

    zip_source_begin_write(file);

    SOURCE_WRITE(file, &currenttitle->versione, sizeof(int))

    SAVE_STRINGA(file, currenttitle->nome_copybook.toUtf8().constData());

    {
        int temp = static_cast<int>(currenttitle->se_registato);
        SOURCE_WRITE(file, &temp, sizeof(int))
    }

    SOURCE_WRITE(file, &currenttitle->se_tradotto, sizeof(currenttitle->se_tradotto));

    SAVE_STRINGA(file, currenttitle->testi.toUtf8().constData());

    SAVE_STRINGA(file, currenttitle->audio_position_path.toUtf8().constData())

    SOURCE_WRITE(file, &currenttitle->m_touch, sizeof(currenttitle->m_touch))
    SOURCE_WRITE(file, &currenttitle->count_pdf, sizeof(currenttitle->count_pdf));
    SOURCE_WRITE(file, &currenttitle->count_img, sizeof(currenttitle->count_img));

    if(currenttitle->m_touch)
        SAVE_BINARY(filezip);

    /* testinohtml */
    len = currenttitle->testinohtml.length();
    SOURCE_WRITE(file, &len, sizeof(int));

    for(i=0; i<len; i++)
        SAVE_STRINGA(file, currenttitle->testinohtml.at(i).toUtf8().constData())


    /* posizione_iniz */
    for(i=0; i<len; i++){
        temp = currenttitle->posizione_iniz.at(i);
        SOURCE_WRITE(file, &temp, sizeof(int))
    }

    res_img = currenttitle->m_img->save_metadata(file);
    if(res_img != fromimage::load_res::ok)
        goto delete_;

    res_pdf = currenttitle->m_pdf->save_metadata(file);
    if(res_pdf != frompdf::load_res::ok)
        goto delete_;

    check = 0;

    /*
     * Upon successful completion 0 is returned. Otherwise, -1 is returned
     * and the error information in source is set to indicate the error.
    */
    check += zip_source_commit_write(file)==ERROR_PRIVATE;

    /*
     * Upon successful completion, zip_file_add() returns the index of
     * the new file in the archive, and zip_file_replace() returns 0.
     * Otherwise, -1 is returned and the error code in archive is set
     * to indicate the error.
    */
    check += zip_file_add(filezip,
                 (currenttitle->nome_copybook + (QString)".xml").toUtf8().constData(),
                 file,
                 ZIP_FL_OVERWRITE)==ERROR_PRIVATE;

    if(check != OK_PRIVATE)
        goto delete_;


    zip_close(filezip);
    return OK;

    delete_:
        zip_source_free(file);
        zip_close(filezip);
        return ERROR;
}

uchar savefile::saveArrIntoFile(const QByteArray &arr, const QString &path)
{
    FILE *fp;
    uchar byte;
    uint i, len;

    len = arr.length();
    fp = fopen(path.toUtf8().constData(), "w");

    if(!fp)
        return ERROR;

    for(i=0; i<len; ++i){
        byte = arr.at(i);

        if(fwrite(&byte, sizeof(byte), 1, fp)<1)
            return ERROR;
    }

    fclose(fp);
    return OK;
}

int savefile::saveArrayIntoFile(const QByteArray &arr, const QString &name_coby,
                                const QString &path, zip_t *filezip,
                                const QString &suffix, const bool closeZip)
{
    QString __fin = name_coby + suffix;
    zip_source_t *file;
    zip_error_t errore;
    int check = 0, error;
    uchar __data;
    int i;

    if(!filezip){
        filezip = zip_open(path.toUtf8().constData(), ZIP_CREATE, &error);

        if(!filezip)
            return ERROR;
    }

    file = zip_source_buffer_create(0, 0, 0, &errore);
    if(!file){
        zip_close(filezip);
        return ERROR;
    }

    zip_source_begin_write(file);

    for(i=0; i<arr.length(); ++i){
        __data = arr.at(i);

        SOURCE_WRITE(file, &__data, sizeof(__data));
    }

    check += zip_source_commit_write(file)==ERROR_PRIVATE;

    check += zip_file_add(filezip,
                 (__fin).toUtf8().constData(),
                 file,
                 ZIP_FL_OVERWRITE)==ERROR_PRIVATE;

    if(check != OK_PRIVATE)
        goto delete_;

    if(closeZip)
        zip_close(filezip);
    return OK;

    delete_:
    zip_source_free(file);

    if(closeZip)
        zip_close(filezip);
    return ERROR;
}

static int save_string(zip_source_t *file, const char *stringa){
    int size = strlen(stringa);
    SOURCE_WRITE_RETURN(file, &size, sizeof(size));

    SOURCE_WRITE_RETURN(file, stringa, sizeof(char)*size);

    return OK;
}

int save_audio_file(const char *posAudio,
                    const QString &namecopybook,
                    const QString &path){
    return savefile::saveArrayIntoFile((const QString)posAudio, namecopybook, path, nullptr, "audio.wav" );

}

static void setCurrentVersion(Document *data){
    data->versione = CURRENT_VERSION_CURRENT_TITLE;
}
