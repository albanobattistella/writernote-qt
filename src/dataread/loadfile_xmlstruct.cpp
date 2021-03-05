#include "xmlstruct.h"

#include "../images/save_images.h"
#include "../datawrite/source_read_ext.h"
#include "../utils/common_error_definition.h"

#define LOAD_STRINGA(x, y) if(load_stringa(x,y) == ERROR) goto free_;
#define LOAD_STRINGA_RETURN(x, y) if(load_stringa(x, y) == ERROR)return ERROR;

static int load_audio(QByteArray *array, zip_t *file, const char *namecopybook);

static int load_stringa(zip_file_t *f, QString *stringa){
    int temp;

    SOURCE_READ(f, &temp, sizeof(int));

    if(temp){
        char *vartempp = new char[temp + 1];

        SOURCE_READ(f, vartempp, sizeof(char)*temp);

        vartempp[temp] = '\0';

        *stringa = vartempp;

        delete [] vartempp;

    }

    return OK;
}

#define LOAD_MULTIPLESTRING(x, y, z) if(load_multiplestring(x,y,z) == ERROR) goto free_;
#define LOAD_MULTIPLESTRING_RETURN(x, y, z) if(load_multiplestring(x,y,z) == ERROR) return ERROR;

static int load_multiplestring(zip_file_t *f, QList<QString> * lista, QList<int> * data){
    int check = 0, i, lunghezza, temp;

    SOURCE_READ(f, &lunghezza, sizeof(int));
    if(!lunghezza)
        return OK;


    char *variabiletemp;

    for(i=0; i<lunghezza; i++){
        SOURCE_READ(f, &temp, sizeof(int));
        check += source_read_ext(f, &temp, sizeof(int));

        variabiletemp = new char[temp + 1];

        SOURCE_READ(f, variabiletemp, sizeof(char)*temp);

        variabiletemp[temp] = '\0';

        lista->append(variabiletemp);
        delete [] variabiletemp;
    }


    for(i=0; i<lunghezza; i++){
        SOURCE_READ(f, &temp, sizeof(int));

        data->append(temp);
    }

    return OK;
}

#define CLOSE_ZIP(x, y) zip_fclose(x);zip_close(y);
#define LOAD_IMAGE(x,y) if(load_image(x, y) != OK)goto free_;
#define LOAD_IMAGE_RETURN(x, y) if(load_image(x, y) != OK) return ERROR;

#define LOAD_BINARIO(x) if(loadbinario(x) == ERROR) goto free_;
#define LOAD_BINARIO_RETURN(x) if(loadbinario(x) == ERROR) return ERROR;

int xmlstruct::loadfile(const char *nameFile){
    currenttitle->reset();
    currenttitle->datatouch->reset();

    int err = 0;

    zip_t *filezip = zip_open(this->path_->toUtf8().constData(), 0, &err);
    if (filezip == NULL)
        return false;

    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(filezip, nameFile, 0, &st);

    zip_file_t *f = zip_fopen(filezip, nameFile, 0);
    if(f == NULL){
        zip_close(filezip);
        return false;
    }


    int temp_versione;
    SOURCE_READ_GOTO(f, &temp_versione, sizeof(int));

    if(temp_versione == 2){
#ifdef ALL_VERSION
        if(load_file_2(currenttitle, f, filezip) != OK)
            goto free_;
#else
        goto ERROR_VERSION;
#endif
    }else if(temp_versione == 3){
        if(load_file_3(currenttitle, f, filezip) != OK)
            goto free_;

    }else if(temp_versione > 3)
        goto error_new_version;


    /*
     * after we load all the file we
     * set the version of the new
     * in this way, even the files created
     * by previous versions of writernote
     * will be updated to the new versions
    */
    currenttitle->versione = CURRENT_VERSION_CURRENT_TITLE;

    CLOSE_ZIP(f, filezip);
    return OK;


    free_:
    CLOSE_ZIP(f, filezip);
    return ERROR;

    /*
     * in case we can not operate with the file because it's too old
    */
#ifndef ALL_VERSION
    error_version:
    CLOSE_ZIP(f, filezip);
    return ERROR_VERSION;
#endif
    error_new_version:
    CLOSE_ZIP(f, filezip);
    return ERROR_VERION_NEW;
}

#ifdef ALL_VERSION
int xmlstruct::load_file_2(currenttitle_class *currenttitle, zip_file_t *f, zip_t *filezip){
    /*if(currenttitle->isOkVersion())
        goto error_version;*/

    LOAD_STRINGA_RETURN(f, &currenttitle->nome_copybook);

    bool temp;
    SOURCE_READ_RETURN(f, &temp, sizeof(bool));
    if(temp)
        this->currenttitle->se_registato = audio_record::record_file;
    else
        this->currenttitle->se_registato = audio_record::not_record;


    SOURCE_READ_RETURN(f, &currenttitle->se_tradotto, sizeof(bool));

    LOAD_STRINGA_RETURN(f, &currenttitle->testi)

    LOAD_STRINGA_RETURN(f, &currenttitle->audio_position_path)

    SOURCE_READ_RETURN(f, &currenttitle->m_touch, sizeof(bool));

    if(currenttitle->m_touch){
        LOAD_BINARIO_RETURN(filezip);
    }

    LOAD_MULTIPLESTRING_RETURN(f, &currenttitle->testinohtml, &currenttitle->posizione_iniz)

    LOAD_IMAGE_RETURN(&currenttitle->immagini, f);

    return OK;
}
#endif

#define LOAD_AUDIO(x, y, z) if(load_audio(x, y, z) != OK) return ERROR;

int xmlstruct::load_file_3(currenttitle_class *currenttitle, zip_file_t *f, zip_t *filezip)
{
    LOAD_STRINGA_RETURN(f, &currenttitle->nome_copybook)

    SOURCE_READ_RETURN(f, &currenttitle->se_registato, sizeof(int));

    SOURCE_READ_RETURN(f, &currenttitle->se_tradotto, sizeof(bool));

    LOAD_STRINGA_RETURN(f, &currenttitle->testi)

    LOAD_STRINGA_RETURN(f, &currenttitle->audio_position_path)

    SOURCE_READ_RETURN(f, &currenttitle->m_touch, sizeof(bool));

    if(currenttitle->m_touch){
        LOAD_BINARIO_RETURN(filezip);
    }

    LOAD_MULTIPLESTRING_RETURN(f, &currenttitle->testinohtml, &currenttitle->posizione_iniz);

    if(currenttitle->se_registato == audio_record::record_zip)
        LOAD_AUDIO(&currenttitle->audio_data, filezip, currenttitle->nome_copybook.toUtf8().constData());

    LOAD_IMAGE_RETURN(&currenttitle->immagini, f);

    return OK;
}

/*
 * the function automatically opens and closes the file containing the audio
*/
static int load_audio(QByteArray *array, zip_t *file, const char *namecopybook){
    int size;
    void * audio_data;

    zip_file_t *f;
    f = zip_fopen(file, namecopybook, 0);
    if(f == NULL)
        return ERROR;

    SOURCE_READ_RETURN(f, &size, sizeof(int));

    SOURCE_READ_RETURN(f, audio_data, size);

    array->append((const char *)audio_data, size);

    return OK;
}
