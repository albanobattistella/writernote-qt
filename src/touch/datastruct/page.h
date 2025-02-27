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
#include "pthread.h"

#define COLOR_NULL QColor::fromRgb(255, 255, 255, 255)
#define Define_PEN(pen) QPen pen(QBrush(), 1.0, Qt::SolidLine, Qt::MPenCapStyle, Qt::RoundJoin);
#define TEMP_COLOR QColor::fromRgb(105, 105, 105, 255)
#define TEMP_TICK 1
#define TEMP_N_X 40
#define TEMP_SQUARE 40

#define __is_ok_count() W_ASSERT(this->_count > 0);

#define SetRenderPainter(painter) painter.setRenderHint(QPainter::Antialiasing, true);

#define Define_PAINTER_p(painter, ___img) \
    QPainter painter; \
    if(!painter.begin(&___img)) { \
        if(debug_enable()){ \
            std::abort(); \
        } \
    }; \
    SetRenderPainter(painter);

#define Define_PAINTER(painter) Define_PAINTER_p(painter, _imgDraw)

#define End_painter(painter) if(!painter.end()) { if(debug_enable()){ std::abort(); }  };


enum n_style: int;

void adjustStrokePage(const QList<stroke> &List, int count, stroke *m_stroke);

constexpr bool debugPage = false;

class page
{
private:
    static constexpr uint width = 1666;
    static constexpr double proportion = 1.4141;
    static constexpr uint height = width * proportion; // correct proportions for A4 paper size

    pthread_mutex_t _img, _append_load;
    bool            _IsVisible = true;
    int             _count;
    QList<stroke>   _stroke;
    stroke          _stroke_writernote;

    /* after adding data to the list, call triggernewimage,
     *  and pass as all false, in this way what is
     * to be drawn will be drawn above the current image, and
     * then strokeTmp will be added to the stroke list
    */
    QVector<stroke> _strokeTmp;
    QImage          _imgDraw;

    void drawNewPage(n_style __style);

    
    void drawEngine(QPainter &painter, QList<stroke> &List, int m_pos_ris, bool *changeSomething, const bool use_multi_thread);
    void draw(QPainter &painter, int m_pos_ris, bool all);
    void drawStroke(QPainter &painter, const stroke &stroke, QPen &pen, const QColor &color) const;

    void mergeList();    

    void AppendDirectly(const stroke &stroke);
    bool initImg(bool flag);

    void decreseAlfa(const QVector<int> &pos, QPainter *painter, int decrese);

    static point_s at_translation(const point_s &point, cint page);
    static QRect get_size_area(const QList<stroke> & item, int from, int to);

public:
    const QImage &getImg() const;

    page();
    page(const page &page);
    page(const int count, const n_style style);
    ~page();

#define PAGE_SWAP_TRIGGER_VIEW BIT(1)
    void swap(QList<stroke> & stroke, const QVector<int> & pos, int flag);
    void swap(QList<stroke> & stroke, int from, int to);

    bool updateFlag(const QPointF &FirstPoint, const double zoom, const double heightView);
    void setVisible(cbool vis) const;

    __slow void at_draw(const uint IndexStroke, const uint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const;

    /* you can access point written by writernote with this funcion */
    __slow void at_draw_page(cint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const;

    __fast int lengthStroke() const;

    bool isVisible() const;

    void removeAt(const QVector<int> & pos);
    void removeAt(cint i);

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

    void changeId(const uint i, const int newId);
    bool userWrittenSomething() const;
    void move(const uint from, const uint to);

    void triggerRenderImage(int m_pos_ris, bool all);

    int getCount() const;

    void reset();
    void allocateStroke(int numAllocation);

    int save(zip_source_t *file, cbool saveImg) const;
    int load(zip_file_t *file, int ver_stroke);

    void drawStroke(const stroke &stroke, int m_pos_ris);
    void drawForceColorStroke(const stroke &stroke, cint m_pos_ris, const QColor &color, QPainter *painter);
    void drawForceColorStroke(const QVector<int> &pos, int m_pos_ris, const QColor &color);

    void removeAndDraw(int m_pos_ris, const QVector<int> &pos, const QRectF &area);
    void drawIfInside(int m_pos_ris, const QRectF &area);
    void drawSquare(const QRect &rect);
    void decreseAlfa(const QVector<int> &pos, int decrese);

    QRect get_size_area(const QVector<int> &pos) const;

    // block for appending
    void lock() const;
    void unlock() const;

    void setCount(int count);

    static void copy(const page &src, page &dest);
    Q_CONSTEXPR static double getProportion();
    Q_CONSTEXPR static double getHeight();
    Q_CONSTEXPR static double getWidth();
    Q_CONSTEXPR static QSize getResolutionSize();

    Q_CONSTEXPR static double getResolutionWidth();
    Q_CONSTEXPR static double getResolutionHeigth();

#define DR_IMG_INIT_IMG BIT(1) // init the image with a image trasparent
    void drawToImage(const QVector<int> &index, QImage &img, cint flag) const;

    page &operator=(const page &other);


    friend class stroke;
    friend class page_file;
    friend class stroke_drawer;
    friend class page_index_cache;
    friend class datastruct;
    friend class xmlstruct;
    friend class rubber_ui;
    friend void * __page_load(void *);
    friend void adjustStrokePage(QList<stroke> &List, int count, stroke *m_stroke);
    friend class copy;
    friend void actionRubberSingleTotal(struct DataPrivateMuThread *_data);
};

force_inline void page::unlock() const
{
    pthread_mutex_unlock((pthread_mutex_t *)&_img);
}

force_inline void page::lock() const
{
    pthread_mutex_lock((pthread_mutex_t *)&_img);
}

force_inline double page::currentHeight() const
{
    return _count * page::getHeight();
}

force_inline double page::currentWidth() const
{
    return page::getWidth();
}

force_inline void page::move(const uint from, const uint to)
{
    this->_stroke.move(from, to);
}

force_inline int page::getCount() const
{
    return this->_count;
}

force_inline void page::reset()
{
    this->_stroke.clear();
    this->_stroke_writernote.reset();
    this->_IsVisible = true;
    this->_count = -1;
    this->_strokeTmp.clear();
    this->_imgDraw = QImage();
}

inline point_s page::at_translation(const point_s &point, cint page)
{
    point_s tmp;
    const double ytranslation = double(page) * page::getHeight();

    if(unlikely(!page)){
        return point;
    }

    memcpy(&tmp, &point, sizeof(tmp));
    tmp._y -= ytranslation;
    return tmp;
}

force_inline void page::AppendDirectly(const stroke &stroke)
{
    this->_stroke.append(stroke);
}

force_inline const QImage &page::getImg() const
{
    return this->_imgDraw;
}

Q_CONSTEXPR force_inline double page::getProportion()
{
    return proportion;
}

Q_CONSTEXPR force_inline double page::getHeight()
{
    return height;
}

Q_CONSTEXPR force_inline double page::getWidth()
{
    return width;
}

Q_CONSTEXPR force_inline QSize page::getResolutionSize()
{
    return QSize(page::getResolutionWidth(), page::getResolutionHeigth());
}

#define PROP_RESOLUTION (2.)
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

    cdouble minH = heightSec * double(_count - 1) / zoom + FirstPoint.y();
    cdouble maxH = heightSec * double(_count)     / zoom + FirstPoint.y();

    if(likely( heightView <= page::getHeight() * zoom)){
        // if the page is not fully visible in a window

        _IsVisible = discordant(maxH, minH);

        if(_IsVisible)
            goto ret;
    }

    _IsVisible = included(0.0, heightView, minH) || included(0.0, heightView, maxH);

ret:
    WDebug(debugPage, "count"   << _count
             << "minH"          << minH
             << "heightView"    << heightView
             << "maxH"          << maxH
             << "zoom"          << zoom
             << _IsVisible);

    return _IsVisible;
}

force_inline void page::setVisible(cbool vis) const
{
    bool &__IsVisible = (bool &)_IsVisible;
    __IsVisible = vis;
}

force_inline const stroke &page::atStroke(uint i) const
{
    __is_ok_count();
    return this->_stroke.at(i);
}

force_inline stroke &page::atStrokeMod(const uint i)
{
    __is_ok_count();
    return this->_stroke.operator[](i);
}

force_inline const stroke &page::get_stroke_page() const
{
    __is_ok_count();
    return this->_stroke_writernote;
}

static force_inline void __at_draw_private(const point_s &from, point_s &to, const double zoom, const QPointF &translation)
{
    memcpy(&to, &from, sizeof(from));

    to._x *= zoom;
    to._y *= zoom;

    to._x += translation.x();
    to._y += translation.y();
}

inline void page::at_draw(const uint IndexStroke, const uint IndexPoint, const QPointF &translation,
                          point_s &point, const double zoom) const
{
    const stroke &stroke = atStroke(IndexStroke);
    const point_s &__point = stroke.at(IndexPoint);

    __at_draw_private(__point, point, zoom, translation);
}

inline void page::at_draw_page(
        cint IndexPoint,
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
    return _stroke.length();
}

force_inline bool page::isVisible() const
{
    return this->_IsVisible;
}

inline void page::copy(
    const page  &src,
    page        &dest)
{
    //int counterStroke, lenStroke;
    //lenStroke = src.lengthStroke();
    dest.reset();

    //dest.allocateStroke(src.lengthStroke());

    dest._stroke                    = src._stroke;
    dest._stroke_writernote         = src._stroke_writernote;
    dest._strokeTmp                 = src._strokeTmp;
    dest._imgDraw                   = src._imgDraw;
    dest._IsVisible                 = src._IsVisible;
    dest._count                     = src._count;
}

force_inline void page::removeAt(cint i)
{
    W_ASSERT(!(i < 0 || i >= _stroke.size()));

    this->_stroke.removeAt(i);
}

force_inline const stroke &page::last() const
{
    __is_ok_count();
    return this->_stroke.last();
}

inline stroke &page::lastMod()
{
    __is_ok_count();
    return this->_stroke.operator[](this->lengthStroke() - 1);
}

force_inline void page::append(const stroke &strokeAppend)
{
    DO_IF_DEBUG(
    int lastNewIndex = _strokeTmp.length();
    );

    __is_ok_count();
    W_ASSERT(!strokeAppend.isEmpty());

    this->_strokeTmp.append(strokeAppend);

    /* they will be automatically removed when
     * the project is compiled in release mode
    */
    if(strokeAppend.is_normal()){
        W_ASSERT(stroke::cmp(strokeAppend, _strokeTmp.at(lastNewIndex)));
    }
}

force_inline double page::minHeight() const
{
    return (this->_count - 1) * this->height;
}

force_inline page::page(const page &from)
{
    pthread_mutex_init(&_img, NULL);
    pthread_mutex_init(&_append_load, NULL);
    page::copy(from, *this);
}

force_inline page::~page()
{
    pthread_mutex_destroy(&_img);
    pthread_mutex_destroy(&_append_load);
}

inline page &page::operator=(const page &other)
{
    if(this == &other){
        return *this;
    }

    page::copy(other, *this);
    return *this;
}

force_inline void page::drawForceColorStroke(
        const stroke    &stroke,
        cint            m_pos_ris,
        const QColor    &color,
        QPainter        *painter)
{
    Define_PEN(pen);
    cbool needDelete = (bool) (!painter);

    if(needDelete){
        if(unlikely(initImg(false)))
            return this->triggerRenderImage(m_pos_ris, true);

        WNew(painter, QPainter, ());

        if(!painter->begin(&this->_imgDraw))
            std::abort();
        painter->setRenderHint(QPainter::Antialiasing, true);
    }

    this->drawStroke(*painter, stroke, pen, color);
    W_ASSERT(painter->isActive());

    if(needDelete){
        painter->end();
        WDelete(painter);
    }
}

#endif // PAGE_H
