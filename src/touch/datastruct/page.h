#ifndef PAGE_H
#define PAGE_H

#include <QList>
#include <QPointF>
#include <QDebug>
#include <QImage>
#include "stroke.h"
#include "../../log/log_ui/log_ui.h"
#include "../../utils/common_def.h"

enum n_style: int;

class page
{
private:
    static constexpr uint width = 1666;
    static constexpr double proportion = 1.4141;
    static constexpr uint height = width*proportion; // correct proportions for A4 paper size

    bool IsVisible = true;
    int count;

    QList<stroke> m_stroke;
    QList<stroke> m_stroke_writernote;

    /* after adding data to the list, call triggernewview,
     *  and pass as all false, in this way what is
     * to be drawn will be drawn above the current image, and
     * then strokeTmp will be added to the stroke list
    */
    QList<stroke> strokeTmp;

    void drawNewPage(n_style __style);

    QImage imgDraw;
    void drawEngine(QPainter &painter, QList<stroke> &List, int i, const int m_pos_ris);
    void draw(QPainter &painter, const int m_pos_ris, const bool all);

    void mergeList();

    static point_s *at_translation(const point_s &point, const int page);

public:
    const QImage &getImg() const;

    page(const int count, const n_style style);

    static double getProportion();
    static double getHeight();
    static double getWidth();

    static double getResolutionWidth();
    static double getResolutionHeigth();

    void updateFlag(const QPointF &FirstPoint, const double zoom, const double heightView);
    void setVisible(const bool vis);

    __slow void at_draw(const uint IndexStroke, const uint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const;

    /* you can access point written by writernote with this funcion */
    __slow void at_draw_page(const uint IndexStroke, const uint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const;

    __fast uint lengthStroke() const;
    __fast uint lengthStrokePage() const; // len point written by writernote

    bool isVisible() const;
    static void copy(const page &src, page &dest);
    void removeAt(const uint i);
    int maxId() const;
    const stroke & last() const;

    /*
     *  these 3 functions do not automatically launch
     *  the drawing of the whole sheet, they wait for
     *  the triggerRenderImage to be executed.
    */
    __fast void append(const stroke &point);

    __slow void appendToTheTop(const stroke &point);

    __fast const stroke       & atStroke(const uint i) const;
    __fast stroke             & atStrokeMod(const uint i);

    __fast const stroke       & atStrokePage(const uint i) const; //return the point written by writernote

    double minHeight() const;
    double currentHeight() const;
    double currentWidth() const;

    void changeCounter(const int newPage);
    void changeId(const uint i, const int newId);
    bool needtochangeid(const int IndexStroke, const int indexInStroke) const;
    bool userWrittenSomething() const;
    void move(const uint from, const uint to);

    void triggerRenderImage(int m_pos_ris, const bool all);

    void moveToUserPoint(int &i) const;

    void reset();
    void allocateStroke(int numAllocation);

    friend class stroke;
};

inline double page::currentHeight() const
{
    return count*page::getHeight();
}

inline double page::currentWidth() const
{
    return page::getWidth();
}

inline void page::move(const uint from, const uint to)
{
    this->m_stroke.move(from, to);
}

inline void page::moveToUserPoint(int &i) const
{
    static int which = -1;
    if(which != -1){
        i = which;
        return;
    }

    int len;
    len = this->lengthStroke();
    for(i = 0; i < len; i++)
        if(atStroke(i).isIdUser())
            break;

    which = i;
}

inline void page::reset()
{
    this->m_stroke.clear();
}

inline point_s *page::at_translation(const point_s &point, const int page)
{
    static point_s tmp;
    const double ytranslation = double(page)*page::getHeight();

    memcpy(&tmp, &point, sizeof(point_s));
    tmp.m_y -= ytranslation;
    return &tmp;
}

inline const QImage &page::getImg() const
{
    return this->imgDraw;
}

inline double page::getProportion()
{
    return proportion;
}

inline double page::getHeight(){
    return height;
}

inline double page::getWidth()
{
    return width;
}

#define PROP_RESOLUTION double(2)
inline double page::getResolutionWidth()
{
    return getWidth() * PROP_RESOLUTION;
}

inline double page::getResolutionHeigth()
{
    return getHeight() * PROP_RESOLUTION;
}

inline void page::updateFlag(const QPointF &FirstPoint, const double zoom, const double heightView)
{
    static double heightSec;
    static double translation;

    heightSec = double(height)*zoom;
    translation = -FirstPoint.y();

    IsVisible =  ((heightSec*count) >= translation) && ((heightSec*(count-1)) <= translation);
    IsVisible = (IsVisible) ? IsVisible : (count-1)*heightSec <= heightView && (count-1)*heightSec >= translation;
    IsVisible = true;
}

inline void page::setVisible(const bool vis)
{
    this->IsVisible = vis;
}

inline const stroke &page::atStroke(uint i) const
{
    return this->m_stroke.at(i);
}

inline stroke &page::atStrokeMod(const uint i)
{
    return this->m_stroke.operator[](i);
}

inline const stroke &page::atStrokePage(const uint i) const
{
    return this->m_stroke_writernote.at(i);
}

static inline void __at_draw_private(const point_s &from, point_s &to, const double zoom, const QPointF &translation)
{
    memcpy(&to, &from, sizeof(point_s));

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

inline void page::at_draw_page(const uint IndexStroke, const uint IndexPoint, const QPointF &translation, point_s &point, const double zoom) const
{
    const stroke &stroke = this->m_stroke_writernote.at(IndexStroke);
    const point_s &__point = stroke.at(IndexPoint);

    __at_draw_private(__point, point, zoom, translation);
}

inline uint page::lengthStroke() const
{
    return m_stroke.length();
}

inline uint page::lengthStrokePage() const
{
    return this->m_stroke_writernote.length();
}

inline bool page::isVisible() const
{
    return this->IsVisible;
}

inline void page::copy(const page &src, page &dest){
    int counterStroke, lenStroke;
    lenStroke = src.lengthStroke();
    dest.reset();

    dest.allocateStroke(src.lengthStroke());

    for(counterStroke = 0; counterStroke < lenStroke; counterStroke ++){
        dest.atStrokeMod(counterStroke) = src.atStroke(counterStroke);
    }

    dest.imgDraw = src.imgDraw;
    dest.IsVisible = src.IsVisible;
    dest.count = src.count;
}

inline void page::removeAt(const uint i)
{
    this->m_stroke.removeAt(i);
}

inline int page::maxId() const
{
    uint i;
    uint len;
    int id;

    len = lengthStroke();
    id = 0;

    for(i = 0; i < len; i++){
        const stroke &stroke = atStroke(i);

        if(stroke.getId() > id)
            id = stroke.getId();
    }

    return id;
}

inline const stroke &page::last() const
{
    return this->m_stroke.last();
}

inline void page::append(const stroke &strokeAppend)
{
    int lastNewIndex = strokeTmp.length();
    this->strokeTmp.append(stroke());
    this->strokeTmp.operator[](lastNewIndex) = strokeAppend;
    //this->m_stroke.append(stroke);
}

inline void page::appendToTheTop(const stroke &stroke)
{
    int start;

    this->moveToUserPoint(start);
    m_stroke.insert(start, stroke);

}

inline double page::minHeight() const
{
    return (this->count-1)*this->height;
}

#endif // PAGE_H
