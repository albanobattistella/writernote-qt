#include "WImage.h"
#include "utils/WCommonScript.h"
#include "touch/dataTouch/page/Page.h"

#ifdef USE_QT
WImage::WImage(const std::string &path, const char *format):
    QImage(path, format)
{
    this->fill(Qt::transparent);
}
#else
#endif

#ifdef USE_QT
size_t WImage::save_and_size(WByteArray &arr) const
{

    W_ASSERT(arr.size() == 0);
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    QImage::save(&buffer, "PNG");
    buffer.close();
    return arr.size();
}
#endif

size_t WImage::get_size_in_file() const
{
    WByteArray arr;
    const auto s = this->save_and_size(arr);
    return s;
}

#ifdef USE_QT

WImage::WImage(int width, int height, QImage::Format format) : QImage(width, height, format)
{
}

WImage::WImage(int page, bool consideringResolution) :
    QImage(consideringResolution
               ? Page::getResolutionWidth()
               : Page::getWidth(),
           static_cast<int>(consideringResolution
                                ? Page::getResolutionHeigth()
                                : Page::getHeight()
                            ) * page,
           WImage::Format_ARGB32)
{
    W_ASSERT(page > 0);
    const auto res = QImage::isNull();
    W_ASSERT(!res);
}
#else

#endif