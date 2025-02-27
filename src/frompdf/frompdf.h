#ifndef FROMPDF_H
#define FROMPDF_H

#include <QtGlobal>

#ifndef PDFSUPPORT
class frompdf{
public:
    uchar var;
    frompdf(void *){};
};
#endif

#ifdef PDFSUPPORT

#include "touch/datastruct/datastruct.h"
#include "currenttitle/document.h"
#include "zip.h"
#include <QString>
#include <QList>
#include <QImage>
#include <QMap>
#include <QPainter>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
# include "poppler-qt5.h"
#else
# include "poppler-qt6.h"
#endif

#include "../images/fromimage.h"
#define IMG_PDF_HEIGHT 292
#define IMG_PDF_WIDTH 210

#define SUFFIX_PDF "_pdf_"

class TabletCanvas;

class Pdf{
public:
    Pdf()
    {
        img = QList<QImage>();
        topLeft = QPointF();
    }

    Pdf(const Pdf &pdf)
    {
        this->img = pdf.img;
        this->topLeft = pdf.topLeft;
    }

    ~Pdf() = default;

    QList<QImage> img;
    
    /* top left */
    QPointF topLeft;
};

class frompdf
{
private:
    Document *m_data;

    QVector<Pdf> m_image;

    static inline QString getName(const uint i){
        return SUFFIX_PDF + QString::number(uint(i));
    }

public:
    static void copy(const frompdf &src, frompdf &dest)
    {
        dest.m_image = src.m_image;
    }

    static bool isvalid(QString &pos);

    void translation(const QPointF &point);
    inline void translation(const double x, const double y)
    {
        translation(QPointF(x, y));
    }

    frompdf(Document *doc);
    ~frompdf() = default;

    enum load_res: uchar{
        ok,
        not_valid_pdf,
        not_valid_page,
        no_metadata,
        no_valid_path
    };

    inline void reset(){
        m_image.clear();
        this->m_data->count_pdf = 0;
    }

    QStringList get_name_pdf();

    /* return true if all load correctly */
    bool load(const QStringList &path, QMap<load_res, uchar> &index, TabletCanvas *canvas);
    load_res load(const QString &, const bool clear, TabletCanvas *canvas);
    /* it load from a zip_t file all the pdf for the current copybook */
    load_res load(zip_t *filezip, zip_file_t *file, TabletCanvas *);
    load_res load_from_row(const QByteArray &, const bool clear,
                           const bool FirstLoad, const uchar IndexPdf,
                           TabletCanvas *canvas);

    void resizing(TabletCanvas *canvas, const uint lenPdf);

    load_res save(zip_t *filezip, const QStringList &path, const QString &path_writernote_file);
    load_res save(zip_t *filezip, const QString &path, const QString &path_writernote_file);

    load_res save_metadata(zip_source_t *file);

    inline void draw(QPainter &painter, const double delta,
                     const bool IsExportingPdf) const;

    uchar insert_pdf(QString &pos,
                     const PointSettable *point);

    void addPdf(QString &pos,
                const PointSettable *point,
                const QString &path_writernote,
                TabletCanvas *canvas);

    uint resolution = 500;//72
private:
    void adjast(const uchar indexPdf);
    load_res load_metadata(zip_file_t *file);

    /*
     * this function only append a pdf to
     * the list
    */
    void init_FirstLoad();
};

force_inline void frompdf::draw(QPainter &painter, const double delta, const bool IsExportingPdf) const
{
    int i, k, len_img;
    QRectF size;
    const Pdf *pdf;
    const int len = this->m_image.length();

    if(!len)
        return;

    double prop;
    {
        const auto &target = m_image.at(0).img.at(0);
        prop = double(target.height()) / double(target.width());
    }

    const double x = m_data->datatouch->currentWidth() * delta;
    const double y = x * prop;

    for(i = 0; i < len; ++i){
        pdf = &this->m_image.at(i);
        len_img = pdf->img.length();
        size = QRectF(pdf->topLeft*delta, QSizeF(x, y));

        for(k = 0; k < len_img; k++){
            const auto &img = pdf->img.at(k);
            fromimage::draw(painter, size, img);

            size.setY(size.y() + y);

            size.setHeight(y);

            if(unlikely(IsExportingPdf)){
                size.setWidth(x);
            }
        }
    }

}

#endif // PDFSUPPORT
#endif // FROMPDF_H
