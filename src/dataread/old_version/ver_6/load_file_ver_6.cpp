#include "dataread/xmlstruct.h"
#include "datawrite/source_read_ext.h"
#include "utils/common_error_definition.h"
#include "images/fromimage.h"
#include "frompdf/frompdf.h"

#ifdef ALL_VERSION
int xmlstruct::load_file_6(Document *doc, zip_file_t *f, zip_t *filezip, const bool LoadPdf, const bool LoadImg){
    int tmp;
    uchar controllo_parita = 0;
    fromimage::load_res res_img;

    SOURCE_READ_RETURN_SIZE(f, &tmp, sizeof(int));
    doc->se_registato = static_cast<Document::n_audio_record>(tmp);

    LOAD_STRINGA_RETURN(f, doc->audio_position_path)

    SOURCE_READ_RETURN_SIZE(f, &doc->count_pdf, sizeof(doc->count_pdf));
    SOURCE_READ_RETURN_SIZE(f, &doc->count_img, sizeof(doc->count_img));

    tmp = loadbinario_1(filezip);
    if(tmp == ERROR)
        return tmp;
    else if(tmp == ERROR_CONTROLL)
        /* we want to continue to load the file, but we need to return we had a problem */
        controllo_parita = 1;

    if(LoadImg){
        res_img = doc->m_img->load(filezip, f);
        if(res_img != fromimage::load_res::ok){
            return ERROR;
        }
    }

#ifdef PDFSUPPORT
    if(LoadPdf){
        auto res = doc->m_pdf->load(filezip, f, nullptr);
        if(res != frompdf::ok)
            return ERROR;
    }
#endif

    if(controllo_parita)
        return ERROR_CONTROLL;

    return OK;
}

#endif //ALL_VERSION
