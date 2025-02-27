#include "page_file.h"
#include "datawrite/source_read_ext.h"
#include "touch/datastruct/page.h"
#include <QImage>

#ifdef ALL_VERSION

int page_file::load_ver_0(page &_page, zip_file_t *file)
{
    constexpr int ver_stroke = 0;
    int i, k, len_stroke, err = OK;

    // user stroke
    SOURCE_READ_RETURN_SIZE(file, &len_stroke, sizeof(len_stroke));

    for(i = 0; i < len_stroke; i++){
        _page._stroke.append(stroke());
        stroke &ref = _page._stroke.last();
        err = ref.load(file, ver_stroke);

        if(unlikely(err != OK))
            return err;
    }

    // writernote stroke [for page style]
    SOURCE_READ_RETURN_SIZE(file, &len_stroke, sizeof(len_stroke));

    for(i = 0; i < len_stroke; i++){
        stroke __tmp;
        cint res = __tmp.load(file, ver_stroke);
        if(unlikely(res != OK))
            return res;

        if(unlikely(res == PAGE_POINT)){
            cint len = __tmp.length();
            for(k = 0; k < len; k ++){
                _page._stroke_writernote.append(__tmp.at(k), __tmp.getPressure(k));
            }
        }
    }

    // remove empty stroke
    for(int i = _page.lengthStroke() - 1; i >= 0; i--){
        if(_page.atStroke(i).length() == 0){
            _page._stroke.removeAt(i);
        }
    }

    return OK;
}

int page_file::load_ver_1(page &_page, zip_file_t *file)
{
    constexpr int ver_stroke = 1;
    int i, len_stroke, err = OK;
    SOURCE_READ_RETURN_SIZE(file, &len_stroke, sizeof(len_stroke));

    for(i = 0; i < len_stroke; i++){
        _page._stroke.append(stroke());
        stroke &ref = _page._stroke.last();
        err = ref.load(file, ver_stroke);

        if(unlikely(err != OK))
            return err;
    }

    _page._stroke_writernote.load(file, ver_stroke);
    if(_page._stroke_writernote.length() && _page._stroke_writernote.getPressure() > 10){
        _page._stroke_writernote.__setPressureFirstPoint(1.5);
    }

    return err;
}

#endif // ALL_VERSION

int page_file::load_ver_2(page &_page, zip_file_t *file)
{
    constexpr int ver_stroke = 2;
    int err, i, len_stroke;
    QByteArray arr;
    size_t size;
    void *_raw;

    SOURCE_READ_RETURN_SIZE(file, &len_stroke, sizeof(len_stroke));

    for(i = 0; i < len_stroke; i++){
        _page._stroke.append(stroke());
        stroke &ref = _page._stroke.last();
        err = ref.load(file, ver_stroke);

        if(unlikely(err != OK))
            return err;
    }

    err = _page._stroke_writernote.load(file, ver_stroke);

    if(unlikely(err != OK))
        return err;

    SOURCE_READ_RETURN_SIZE(file, &size, sizeof(size));

    _raw = WMalloc(size);
    SOURCE_READ_RETURN_SIZE(file, _raw, size);
    arr = QByteArray::fromRawData((cchar *)_raw, size);

    _page._imgDraw.loadFromData(arr, "PNG");

    WFree(_raw);

    return OK;
}

int page_file::load(page &_page, int ver_stroke, zip_file_t *file)
{
    switch (ver_stroke) {
#ifdef ALL_VERSION
    case 0:
        return page_file::load_ver_0(_page, file);
    case 1:
        return page_file::load_ver_1(_page, file);
#endif
    case 2:
        return page_file::load_ver_2(_page, file);
    default:
        return ERROR_VERSION;
    }
}

int page_file::save(const page *_page, zip_source_t *file, cbool saveImg)
{
    int i, err = OK;
    int len = _page->_stroke.length();
    size_t size;
    QByteArray arr;
    QBuffer buffer(&arr);

    /* stroke len */
    SOURCE_WRITE_GOTO_SIZE(file, &len, sizeof(len));

    for(i = 0; i < len; i++){
        err = _page->atStroke(i).save(file);
        if(unlikely(err != OK))
            return err;
    }

    err = _page->_stroke_writernote.save(file);
    if(unlikely(err != OK))
        return err;

    if(unlikely(saveImg)){
        buffer.open(QIODevice::WriteOnly);
        _page->_imgDraw.save(&buffer, "PNG");
        buffer.close();
    }

    size = arr.size();

    SOURCE_WRITE_GOTO_SIZE(file, &size, sizeof(size));
    SOURCE_WRITE_GOTO_SIZE(file, arr.constData(), size);

    return OK;

    // for SOURCE_WRITE_GOTO macro
delete_:
    return ERROR;
}
