﻿#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QList>
#include <QColor>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include "page.h"

/*
    IDVERTICALE -> linee verticali
    IDORIZZONALE -> linee orizzonali
*/

class frompdf;
class fromimage;

/* canvas */
struct PointSettable {
    QPointF point = QPointF(0, 0);
    bool set = false;
    bool isNotDefine() const
    {
        return !this->set;
    }
};

#define IDVERTICALE -2
#define IDORIZZONALE -1
#define IDUNKNOWN -6

#define WRIT_CHANG(arr, tmp) \
    if(arr[0] < arr[1]){ \
        tmp = arr[0]; \
        arr[0] = arr[1]; \
        arr[1] = tmp; \
    }

class datastruct
{
private:
    QPointF __last_translation;

    /*
     * make this item private for a new datastruct
    */
    QList<page> m_page;

    bool userWrittenSomething(uint frompage);

    void scala_posizionefoglio(const double scala);

    frompdf *m_pdf;
    fromimage *m_img;
    QPointF pointFirstPage = QPointF(0, 0);

    void adjustWidth(const uint width);
    void adjustHeight(const uint height);


    void triggerNewView(int page, int m_pos_ris, const bool all);

    int whichPage(const stroke &stroke) const;

    double zoom = 1.00;

public:
    void triggerNewView(const QList<int> &Page, int m_pos_ris, const bool all);
    void triggerNewView(int m_pos_ris, const bool all);
    void triggerViewIfVisible(int m_pos_ris);

    static bool isOkZoom(const double newPossibleZoom);

    double getZoom() const;
    void changeZoom(const long double zoom, class TabletCanvas *canvas);
    void increaseZoom(const long double delta, const QSize &size);

    inline QPointF getPointFirstPage() const{ return this->zoom * pointFirstPage; }
    inline QPointF getPointFirstPageNoZoom() const { return this->pointFirstPage; }

    void setPointFirstPage(const QPointF &point){ this->pointFirstPage = point; }

    void moveIfNegative(uint &p, uint &page, const uint lenPage, const uint height, const uint width) const;

    void removeAt(const uint indexPage);

    /* the draw function triggers the drawing of the points automatically */
    void append(const QList<stroke> & point, int m_pos_ris);

    /* the draw function triggers the drawing of the points automatically */
    void appendToTheTop(const QList<stroke> &point, int m_pos_ris);

    int  appendStroke(const stroke &stroke); /* return value: the page of the point */
    void appendStroke(const stroke &stroke, const int page);

    void appendToTheTop(const stroke &point, const int page);

    uint move_to_positive(uint len);

    void restoreLastTranslation(const int heightView);
    void controllForRepositioning();

    void removePointId(const QList<int> &list);
    void removePointId(int id);

    void MovePoint(const QList<int> &id, const QPointF &__t);
    bool MovePoint(QRectF &, QPointF);

    bool userWrittenSomething(datastruct *s_data);

    /*
     * return true if the user has written this point
     *
     * all the point writernote draw have id < 0
    */
    static inline bool isIdUser(const int id){ return id >= 0; }
    static inline bool isIdUser(const stroke &__point){ return isIdUser(__point.getId()); }

    bool isinside(QPointF &topleft, QPointF &bottonright, const uint index, const uint page);
    bool isinside(double x1, double y1, double x2, double y2, const uint index, const uint page);

    static bool isinside(const QPointF &topleft, const QPointF &bottonright, const point_s &__point);

    static bool isinside(const QPointF &topleft, const QPointF &bottonright, const QPointF &point);


    void adjustAll(const uint width, const uint height);
    void adjustAll(const QSize &size);

    /*
     * this function return the index of the first
     * point with idtratto == IDORIZZONALE
    */
    uint posIdOrizzonal();


    datastruct(frompdf *m_pdf, fromimage *m_img);
    //~datastruct();

    void moveNextPoint(uint &pos, uint len = 0, int id = -6);

    void reorganize();

    void changeId(uint index, uint indexPage, uint lenPage, int base = -1);

    bool isAvailable(int id) const;

    inline int maxId();

    inline bool isempty() const{
        return this->m_page.isEmpty();
    };

    static void inverso(QPointF &point);
    void repositioning();

    void scala_all(const QPointF &point, const int heightView = -1);

    void reset();
    void triggerVisibility(const double &viewSize);
    double biggerx() const noexcept;
    void removeat(const uint index, const uint page);

    bool needToCreateNewSheet();
    bool needtochangeid(const uint index, const uint page);

    double biggery() const noexcept;

    /* the function return the index of the id */
    uint positionId(int id);

    std::tuple<uint, uint> decreaseAlfa(const int id, const uchar decrese, const int len);

    void removePage(const uint page);

    inline int lastId();

    //__fast const point_s * at(const uint i, const uint page) const;
    __fast const page &     at(const uint page) const;
    __fast page *           at_mod(const uint page);
    //__fast point_s *       at_mod(const uint index, const uint page);
    __slow point_s &        at_draw(const uint indexPoint, const uint indexPage, const uint indexStroke) const;
    __fast const point_s *  lastPoint() const;
    __fast const page *     lastPage() const;
    //point_s &at_translation(const uint index, const uint page) const;

    static size_t getSizeOne();

    inline uint lengthPage() const{ return this->m_page.length();}

    void newPage(const n_style style);
    static void copy(const datastruct &src, datastruct &dest);

    inline QPointF get_size_page() const{
        if(!this->lengthPage())
            return QPointF(page::getWidth(), page::getHeight());
        const auto &page = this->at(lengthPage());
        return QPointF(page->currentWidth(), currentHeight());
    }

    inline double currentWidth() const;
    inline double currentHeight() const;
    inline double proportion() const;

    friend class xmlstruct;

    // for debug
    void checkPositionData(const QPixmap &pixmap, const QSize &maxSize);

};

/* this function does not consider the zoom */
inline double datastruct::currentHeight() const
{
    return page::getHeight();
}

inline double datastruct::proportion() const
{
    return page::getProportion();
}

inline int datastruct::maxId()
{
    static int biggerID;
    static int tmp_id;
    static uint i;

    const uint len = this->lengthPage();

    biggerID = 0;

    if(isempty())
        return biggerID;

    for(i = 0; i < len; ++i){
        tmp_id = this->m_page.at(i).maxId();

        if(tmp_id > biggerID){
            biggerID = tmp_id;
        }
    }

    return biggerID;
}

inline void datastruct::triggerVisibility(const double &viewSize)
{
    static uint i;
    static uint len;

    len = this->m_page.length();

    for(i=0; i<len; i++){
        this->m_page.operator[](i).updateFlag(this->getPointFirstPage(), zoom, viewSize);
        if(this->m_page.at(i).isVisible() && i && i<len-1){
            this->at_mod(i-1)->setVisible(true);
            this->at_mod(i+1)->setVisible(true);
        }
    }
}

inline double datastruct::biggerx() const noexcept
{
    return (page::getWidth() + this->getPointFirstPage().x())*zoom;
}

inline double datastruct::biggery() const noexcept
{
    return (at(lengthPage()-1)->currentHeight() + this->getPointFirstPage().y())*zoom;
}

inline int datastruct::lastId()
{
    return this->maxId();
}

inline const __fast page &datastruct::at(const uint page) const
{
    return &this->m_page.at(page);
}

inline page __slow *datastruct::at_mod(const uint page)
{
    return &this->m_page.operator[](page);
}

inline __slow point_s &datastruct::at_draw(const uint indexPoint, const uint indexPage, const uint indexStroke) const
{
    static point_s point;

    at(indexPage)->at_draw(indexStroke, indexPoint, getPointFirstPage(), point, zoom);

    return point;
}

inline const __slow page *datastruct::lastPage() const
{
    return &this->m_page.last();
}

/*inline point_s &datastruct::at_translation(const uint index, const uint page) const
{
    static point_s point;
    memcpy(&point, at(page)->at(index), sizeof(point_s));

    point.m_x += this->pointFirstPage.x();
    point.m_y += this->pointFirstPage.y() + page * page::getHeight();

    point.m_x *= this->zoom;
    point.m_y *= this->zoom;
    point.m_pressure *= this->zoom;

    return point;
}*/

inline size_t datastruct::getSizeOne()
{
    return sizeof(struct point_s);
}

inline void datastruct::newPage(const n_style style)
{
    page page(this->lengthPage()+1, style);
    this->m_page.append(page);
}

/* this function does not consider the zoom */
inline double datastruct::currentWidth() const
{
    return biggerx();
}

inline void datastruct::triggerNewView(int page, int m_pos_ris, const bool all)
{
    at_mod(page)->triggerRenderImage(m_pos_ris, all);
}

inline int datastruct::whichPage(const stroke &stroke) const
{
    const page *page;
    uint counterPage, len;
    len = this->lengthPage();

    for(counterPage = 0; counterPage < len; counterPage++){
        page = at(counterPage);
        if(page->currentHeight() >= stroke.at(0).m_y && page->minHeight() <= stroke.at(0).m_y){
            return counterPage;
        }
    }
    Q_ASSERT(false);
    return -1;
}

inline void datastruct::triggerNewView(const QList<int> &Page, int m_pos_ris, const bool all)
{
    for(const int page: Page){
        this->triggerNewView(page, m_pos_ris, all);
    }
}

inline void datastruct::triggerNewView(int m_pos_ris, const bool all)
{
    uint i, len;
    len = lengthPage();
    for(i = 0; i < len; i++)
        this->triggerNewView(i, m_pos_ris, all);
}

inline void datastruct::triggerViewIfVisible(int m_pos_ris)
{
    uint i, len;
    len = lengthPage();
    for( i = 0; i < len; i++)
        if(at(i)->isVisible())
            at_mod(i)->triggerRenderImage(m_pos_ris, true);
}

inline bool datastruct::isOkZoom(const double newPossibleZoom)
{
    return !(newPossibleZoom >= 2.0 || newPossibleZoom <= 0.2);
}

inline double datastruct::getZoom() const
{
    return this->zoom;
}

/* the function automatically launches the drawing for the pages
 * to which data has been added*/
inline void datastruct::append(const QList<stroke> &stroke, int m_pos_ris)
{
    QList<int> trigger;
    uint i;
    int WhichPage;
    const uint len = stroke.length();

    for(i = 0; i < len; i++){
        // get the page of the point
        WhichPage = this->appendStroke(stroke.at(i));

        // it the page is not in the list we append
        if(trigger.indexOf(WhichPage) == -1)
            trigger.append(WhichPage);
    }

    this->triggerNewView(trigger, m_pos_ris, false);
}

inline void datastruct::appendToTheTop(const QList<stroke> &point, int m_pos_ris)
{
    uint i;
    const uint len = point.length();
    int page;

    for(i = 0; i < len; i++){
        page = this->whichPage(point.at(i));
        this->appendToTheTop(point.at(i), page);
    }

    this->triggerNewView(m_pos_ris, true);
}

inline void datastruct::removeAt(const uint indexPage){
    int index = indexPage, len;
    this->m_page.removeAt(indexPage);

    len = lengthPage();

    for(; index < len; index ++){
        at_mod(index)->changeCounter(index+1);
    }

}

inline int datastruct::appendStroke(const stroke &stroke)
{
    static point_s Point;
    static int page;
    memcpy(&Point, &stroke, sizeof(point_s));

    page = this->whichPage(stroke);

    this->appendStroke(stroke, page);

    return page;
}

inline void datastruct::appendStroke(const stroke &stroke, const int page)
{
    this->at_mod(page)->append(stroke);
}

inline void datastruct::appendToTheTop(const stroke &point, const int page)
{
    this->at_mod(page)->appendToTheTop(point);
}

#ifdef DEBUGINFO
inline void datastruct::checkPositionData(const QPixmap &pixmap, const QSize &maxSize)
{
    Q_UNUSED(maxSize);
    pixmap.height();
}
#else
inline void datastruct::checkPositionData(const QPixmap &, const QSize &) {}
#endif

#endif // DATASTRUCT_H
