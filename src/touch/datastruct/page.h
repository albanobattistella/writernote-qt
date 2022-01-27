#ifndef PAGE_H
#define PAGE_H

#include <QVector>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QImage>
#include "stroke.h"
#include "log/log_ui/log_ui.h"
#include "utils/common_def.h"
#include "utils/common_script.h"
#include "utils/dialog_critic/dialog_critic.h"
#include "audioplay/audioplay.h"

#define COLOR_NULL QColor::fromRgb(255, 255, 255, 255)

enum n_style: int;

void adjustStrokePage(const QList<stroke> &List, int count, stroke *m_stroke);

constexpr bool debugPage = false;

class page
{
private:
    static constexpr uint width = 1666;
    static constexpr double proportion = 1.4141;
    static constexpr uint height = width*proportion; // correct proportions for A4 paper size

#define FLAG_PAGE_ORDERED   BIT(1) // if indicates whether the list of strokes is sort by index
#define FLAG_PAGE_BLOCK     BIT(2) // if we can't append stroke
    int flag = 0;

    bool IsVisible = true;
    int count;

    QList<stroke> m_stroke;
    stroke m_stroke_writernote;

    /* after adding data to the list, call triggernewimage,
     *  and pass as all false, in this way what is
     * to be drawn will be drawn above the current image, and
     * then strokeTmp will be added to the stroke list
    */
    QVector<stroke> strokeTmp;
    QImage imgDraw;

    void drawNewPage(n_style __style);

    
    void drawEngine(QPainter &painter, QList<stroke> &List, int m_pos_ris, bool *changeSomething, const bool use_multi_thread);
    void draw(QPainter &painter, int m_pos_ris, bool all);
    void drawStroke(QPainter &painter, const stroke &stroke, QPen &pen, const QColor &color) const;

    void mergeList();    

    void AppendDirectly(const stroke &stroke);
    bool initImg(bool flag);

    void decreseAlfa(const QVector<int> &pos, QPainter *painter, int decrese);

    static point_s at_translation(const point_s &point, int page);
    static QRect get_size_area(const QList<stroke> & item, int from, int to);

public:
    const QImage &getImg() const;

    page(const page &page);
    page(const int count, const n_style style);
    ~page() = default;

#define PAGE_SWAP_TRIGGER_VIEW BIT(1)
    void swap(QList<stroke> & stroke, const QVector<int> & pos, int flag);
    void swap(QList<stroke> & stroke, int from, int to);

    bool updateFlag(const QPointF &FirstPoint, const double zoom, const double heightView);
    void setVisible(cbool vis) const;

    __slow void at_draw(const uint IndexStroke, const uint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const;

    /* you can access point written by writernote with this funcion */
    __slow void at_draw_page(const uint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const;

    __fast int lengthStroke() const;

    bool isVisible() const;

    void removeAt(const QVector<int> & pos);
    void removeAt(cuint i);

    const stroke & last() const;
    stroke &lastMod();

    /*
     *  these 3 functions do not automatically launch
     *  the drawing of the whole sheet, they wait for
     *  the triggerRenderImage to be executed.
    */
    __fast void append(const stroke &stroke);
    __fast void append(const QList<stroke> & stroke);

    __fast const stroke       & atStroke(const uint i) const;
    __fast stroke             & atStrokeMod(const uint i);

    __fast const stroke       & get_stroke_page() const; //return the point written by writernote

    double minHeight() const;
    double currentHeight() const;
    double currentWidth() const;

    void changeCounter(const int newPage);
    void changeId(const uint i, const int newId);
    bool userWrittenSomething() const;
    void move(const uint from, const uint to);

    void triggerRenderImage(int m_pos_ris, bool all);

    void reset();
    void allocateStroke(int numAllocation);

    int save(zip_source_t *file) const;
    int load(zip_file_t *file, int ver_stroke, int len_stroke);

    void drawStroke(const stroke &stroke, int m_pos_ris);
    void drawForceColorStroke(const stroke &stroke, int m_pos_ris, const QColor &color, QPainter *painter);
    void drawForceColorStroke(const QVector<int> &pos, int m_pos_ris, const QColor &color);


    void removeAndDraw(int m_pos_ris, const QVector<int> &pos, const QRectF &area);
    void drawIfInside(int m_pos_ris, const QRectF &area);
    void drawSquare(const QRect &rect);
    void decreseAlfa(const QVector<int> &pos, int decrese);

    QRect get_size_area(const QVector<int> &pos) const;

    // block for appending
    void setBlock() const;
    void removeBlock() const;

    static void copy(const page &src, page &dest);
    Q_CONSTEXPR static double getProportion();
    Q_CONSTEXPR static double getHeight();
    Q_CONSTEXPR static double getWidth();

    Q_CONSTEXPR static double getResolutionWidth();
    Q_CONSTEXPR static double getResolutionHeigth();

#define DR_IMG_INIT_IMG BIT(1) // init the image with a image trasparent
    void drawToImage(const QVector<int> &index, QImage &img, cint flag) const;

    page &operator=(const page &other);


    friend class stroke;
    friend class datastruct;
    friend class xmlstruct;
    friend class rubber_ui;
    friend void * __page_load(void *);
    friend void adjustStrokePage(QList<stroke> &List, int count, stroke *m_stroke);
    friend class copy;
    friend void *actionRubberSingleTotal(void *_data);
};

Q_ALWAYS_INLINE void page::removeBlock() const
{
    int & tmp = (int &)flag;
    tmp &= ~FLAG_PAGE_BLOCK;
}

Q_ALWAYS_INLINE void page::setBlock() const
{
    int & tmp = (int &) flag;
    tmp |= FLAG_PAGE_BLOCK;
}

inline double page::currentHeight() const
{
    return count*page::getHeight();
}

inline double page::currentWidth() const
{
    return page::getWidth();
}

Q_ALWAYS_INLINE void page::changeCounter(const int newPage)
{
    this->count = newPage;
}

inline void page::move(const uint from, const uint to)
{
    this->m_stroke.move(from, to);
}

inline void page::reset()
{
    this->m_stroke.clear();
    this->m_stroke_writernote.reset();
    this->IsVisible = true;
    this->count = -1;
    this->strokeTmp.clear();
    this->imgDraw = QImage();
}

inline point_s page::at_translation(const point_s &point, int page)
{
    point_s tmp;
    const double ytranslation = double(page) * page::getHeight();

    memcpy(&tmp, &point, sizeof(tmp));
    tmp.m_y -= ytranslation;
    return tmp;
}

Q_ALWAYS_INLINE void page::AppendDirectly(const stroke &stroke)
{
    if(unlikely(this->flag & FLAG_PAGE_BLOCK)){
        const QString message = QString("Possible bug, appending when flag is %1").arg(QString::number(flag));
        NAME_LOG_EXT->write(message, log_ui::possible_bug);
        qDebug() << message;

#ifdef DEBUGINFO
        dialog_critic(message);
#endif
    }
    this->m_stroke.append(stroke);
}

Q_ALWAYS_INLINE const QImage &page::getImg() const
{
    return this->imgDraw;
}

Q_CONSTEXPR Q_ALWAYS_INLINE double page::getProportion()
{
    return proportion;
}

Q_CONSTEXPR Q_ALWAYS_INLINE double page::getHeight(){
    return height;
}

Q_CONSTEXPR Q_ALWAYS_INLINE double page::getWidth()
{
    return width;
}

#define PROP_RESOLUTION (double(2))
Q_CONSTEXPR force_inline double page::getResolutionWidth()
{
    return getWidth() * PROP_RESOLUTION;
}

Q_CONSTEXPR force_inline double page::getResolutionHeigth()
{
    return getHeight() * PROP_RESOLUTION;
}

force_inline bool page::updateFlag(
        const QPointF   &FirstPoint,
        cdouble         zoom,
        cdouble         heightView)
{
    double heightSec;

    W_ASSERT(zoom >= 0);
    //W_ASSERT(FirstPoint.x() <= 0.0 && FirstPoint.y() <= 0.0);
    W_ASSERT(heightView >= 0);

    heightSec = page::getHeight() * zoom;

    cdouble minH = heightSec * double(count - 1) / zoom + FirstPoint.y();
    cdouble maxH = heightSec * double(count)     / zoom + FirstPoint.y();

    if(likely( heightView <= page::getHeight() * zoom)){
        // if the page is not fully visible in a window

        IsVisible = discordant(maxH, minH);

        if(IsVisible)
            goto ret;
    }

    IsVisible = included(0.0, heightView, minH) || included(0.0, heightView, maxH);

ret:
    WDebug(debugPage, "count"   << count
             << "minH"          << minH
             << "heightView"    << heightView
             << "maxH"          << maxH
             << "zoom"          << zoom
             << IsVisible);

    return IsVisible;
}

force_inline void page::setVisible(cbool vis) const
{
    bool &_IsVisible = (bool &)IsVisible;
    _IsVisible = vis;
}

force_inline const stroke &page::atStroke(uint i) const
{
    return this->m_stroke.at(i);
}

force_inline stroke &page::atStrokeMod(const uint i)
{
    return this->m_stroke.operator[](i);
}

force_inline const stroke &page::get_stroke_page() const
{
    return this->m_stroke_writernote;
}

static force_inline void __at_draw_private(const point_s &from, point_s &to, const double zoom, const QPointF &translation)
{
    memcpy(&to, &from, sizeof(from));

    to.m_x *= zoom;
    to.m_y *= zoom;

    to.m_x += translation.x();
    to.m_y += translation.y();
}

inline void page::at_draw(const uint IndexStroke, const uint IndexPoint, const QPointF &translation,
                          point_s &point, const double zoom) const
{
    const stroke &stroke = atStroke(IndexStroke);
    const point_s &__point = stroke.at(IndexPoint);

    __at_draw_private(__point, point, zoom, translation);
}

inline void page::at_draw_page(
        const uint      IndexPoint,
        const QPointF   &translation,
        point_s         &point,
        const double    zoom) const
{
    const stroke &stroke = get_stroke_page();
    const point_s &__point = stroke.at(IndexPoint);

    __at_draw_private(__point, point, zoom, translation);
}

force_inline int page::lengthStroke() const
{
    return m_stroke.length();
}

force_inline bool page::isVisible() const
{
    return this->IsVisible;
}

inline void page::copy(
    const page  &src,
    page        &dest)
{
    //int counterStroke, lenStroke;
    //lenStroke = src.lengthStroke();
    dest.reset();

    //dest.allocateStroke(src.lengthStroke());

    dest.m_stroke               = src.m_stroke;

    dest.m_stroke_writernote = src.m_stroke_writernote;

    dest.strokeTmp              = src.strokeTmp;

    //for(counterStroke = 0; counterStroke < lenStroke; counterStroke ++){
    //    dest.atStrokeMod(counterStroke) = src.atStroke(counterStroke);
    //}

    dest.imgDraw = src.imgDraw;
    dest.IsVisible = src.IsVisible;
    dest.count = src.count;
}

force_inline void page::removeAt(const uint i)
{
    this->m_stroke.removeAt(i);
}

force_inline const stroke &page::last() const
{
    return this->m_stroke.last();
}

inline stroke &page::lastMod()
{
    return this->m_stroke.operator[](this->lengthStroke() - 1);
}

force_inline void page::append(const stroke &strokeAppend)
{
    DO_IF_DEBUG(
    int lastNewIndex = strokeTmp.length();
    );

    this->strokeTmp.append(strokeAppend);

    /* they will be automatically removed when
     * the project is compiled in release mode */
    W_ASSERT(strokeTmp.at(lastNewIndex).length() == strokeAppend.length());
}

force_inline double page::minHeight() const
{
    return (this->count-1)*this->height;
}

force_inline page::page(const page &from)
{
    page::copy(from, *this);
}

inline page &page::operator=(const page &other)
{
    if(this == &other){
        return *this;
    }

    page::copy(other, *this);
    return *this;
}

#endif // PAGE_H
