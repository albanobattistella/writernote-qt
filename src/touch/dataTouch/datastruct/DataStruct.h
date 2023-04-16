﻿#pragma once

#include <QList>
#include <QColor>
#include <QDebug>
#include <QPixmap>
#include "touch/dataTouch/page/Page.h"
#include "utils/WCommonScript.h"
#include <csignal>
#include <utility>
#include "pthread.h"
#include "utils_datastruct.h"
#include "core/WMutex.h"

/*
    IDVERTICALE -> linee verticali
    IDORIZZONALE -> linee orizzonali
*/

class frompdf;
class fromimage;


#define IDVERTICALE -2
#define IDORIZZONTALE -1
#define IDUNKNOWN -6

#define WRIT_CHANG(arr, tmp)    \
    if(arr[0] < arr[1]){        \
        tmp = arr[0];           \
        arr[0] = arr[1];        \
        arr[1] = tmp;           \
    }

class DataStruct
{
private:
    QPointF _last_translation;
    QVector<Page> _page;
    QPointF _pointFirstPage = QPointF(0., 0.);
    double _zoom = 1.;

    // todo --> move this mutex to page
    WMutex _changeIdMutex;
    WMutex _changeAudioMutex;

    bool userWrittenSomething(uint frompage);

    [[nodiscard]] bool isOkTranslate(const QPointF &point, cbool isZoom) const;

    void triggerNewView(int page, int m_pos_ris, cbool all);

    void newPage(int num);

    int _pageVisible = -1;

    void __changeId(int indexPoint, Stroke &stroke, Page &page, cbool useThreadSafe);
public:
    DataStruct();
    ~DataStruct() = default;

    void triggerNewView(const QList<int> &Page, int m_pos_ris, cbool all);
    void triggerNewView(int m_pos_ris, cbool all);
    void triggerIfNone(int m_pos_ris);
    void triggerViewIfVisible(int m_pos_ris);

    void changeZoom(double zoom, int heightScreen);

    auto constBegin()   const { return this->_page.constBegin(); }
    auto constEnd()     const { return this->_page.constEnd(); }
    auto begin()              { return this->_page.begin(); }
    auto end()                { return this->_page.end(); }

    void drawIfInside(const QRect &area);

    [[nodiscard]] constexpr auto getZoom() const noexcept { return this->_zoom; };
    [[nodiscard]] constexpr force_inline QPointF getPointFirstPage() const { return _zoom * _pointFirstPage; }
    [[nodiscard]] constexpr force_inline QPointF getPointFirstPageNoZoom() const { return _pointFirstPage; }

    [[nodiscard]] int whichPage(const QPointF &point) const;
    [[nodiscard]] int whichPage(const Stroke &stroke) const;

    void setPointFirstPage(const QPointF &point)
    {
        _pointFirstPage = point;
        W_ASSERT(point.x() <= 0.);
        W_ASSERT(point.y() <= 0.);
    }

    void removeAt(unsigned indexPage);

    /* the draw function triggers the drawing of the points automatically */
    void append(const QList<std::shared_ptr<Stroke>>& stroke, int m_pos_ris);

    int  appendStroke(const std::shared_ptr<Stroke>&); /* return value: the page of the point */
    void appendStroke(std::shared_ptr<Stroke> stroke, int page);

    void restoreLastTranslation(int heightView);

    void removePointIndex(QList<QVector<int> > &pos, cint base, cbool __isOrder);
    void removePointIndex(QVector<int> &pos, cint page, cbool isOrder);

    void MovePoint(const QList<QVector<int>> & pos, cint base, const QPointF &translation);
    void MovePoint(const QVector<int> & pos, cint page, const QPointF &translation);

#   define DATASTRUCT_MUST_TRASLATE_PATH BIT(1)
    static void MovePoint(QList<Stroke> &stroke, const QPointF &translation, int flag);

    [[nodiscard]] bool userWrittenSomething() const;
    static bool userWrittenSomething(const DataStruct &data1, const DataStruct &data2);

    [[nodiscard]] int adjustStroke(Stroke &stroke) const;

    void moveNextPoint(uint &pos, uint len = 0, int id = -6);

    void changeIdThreadSave(int indexPoint, Stroke &stroke, Page &page);

    [[nodiscard]] constexpr QPointF adjustPointReverce(const QPointF &pointDatastruct) const;
    [[nodiscard]] constexpr QPointF adjustPoint(const QPointF &pointRealTouch) const;

    [[nodiscard]] bool isempty_touch() const;
    void reset_touch();
    void triggerVisibility(cdouble viewSize);
    [[nodiscard]] double biggerx() const noexcept;

    [[nodiscard]] bool needToCreateNewSheet() const;

    [[nodiscard]] double biggery() const noexcept;

    void decreaseAlfa(const QVector<int> &pos, int page);
    void removePage(int page);

    __fast [[nodiscard]] const Page &     at(unsigned page) const;
    __fast Page &           at_mod(cint page);

    [[nodiscard]] Point at_draw_page(cint indexPoint, const Page &Page) const;
    static Point at_draw_page(cint indexPoint, const Page &Page, const QPointF &PointFirstPageWithZoom, cdouble zoom);

    __fast [[nodiscard]] const Page &     lastPage() const;


    [[nodiscard]] int lengthPage() const{ return _page.length(); }
    void newPage(n_style style);

    [[nodiscard]] QPointF get_size_page() const{ return QPointF(Page::getWidth(), Page::getHeight()); }

    __fast [[nodiscard]] QRectF get_size_area(const QVector<int> &pos, int page) const;
    __fast [[nodiscard]] QRectF get_size_area(const QList<QVector<int>> &pos, int base) const;
    //__slow QRectF get_size_area(const QList<int> & id) const;

    [[nodiscard]] int getFirstPageVisible() const;

    void setVisible(int from, int to);
    [[nodiscard]] double currentWidth() const;
    void moveToPage(int page);
    [[nodiscard]] int getLastPageVisible() const;
    void newViewAudio(int newTime);

    [[nodiscard]] int get_range_visible() const;
    void insertPage(const Page &Page, int index);

    [[nodiscard]] static bool isOkZoom(double newPossibleZoom);
    static void copy(const DataStruct &src, DataStruct &dest);
    static force_inline void inverso(QPointF &point) {point *= -1.0;};
    static QPointF inverso(const QPointF &point) { return (-1) * point; };
    static QRectF get_bigger_rect(const QRectF &first, const QRectF &second);
    static QRect get_bigger_rect(const QRect &first, const QRect &second);

    friend class xmlstruct;
    friend class TestingCore;

protected:
    void controllForRepositioning(QPointF &translateTo);
    void increaseZoom(double delta, const QSize &size, QPointF& res);
    void adjustAll(uint width, uint height, QPointF &res);
    void adjustAll(const QSize &size, QPointF &res);
    void adjustHeight(cdouble height, QPointF& translatoTo);
    void adjustWidth(cdouble width, QPointF& translatoTo);

    void setZoom(typeof(DataStruct::_zoom) newZoom);
    void setPageVisible(int page);

    virtual void scala_all(const QPointF &point, int heightView = -1);
};

inline void DataStruct::triggerVisibility(cdouble viewSize)
{
    int from, to;
    cint len = lengthPage();
    QPointF _init(0, 0);
    QPointF _end(0, viewSize - 0.1);

    if(un(!len))
        return;

    _init = this->adjustPoint(_init);
    _end = this->adjustPoint(_end);

    from = this->whichPage(_init);
    to = this->whichPage(_end);

    if(un(to < 0 || from < 0)){
        from = 0;
        to = len - 1;
    }

    W_ASSERT(from >= 0);
    W_ASSERT(to >= 0);

    W_ASSERT(from <= to);
    W_ASSERT(to < len);

    this->setVisible(from, to);
}

inline double DataStruct::biggerx() const noexcept
{
    return (Page::getWidth() + this->getPointFirstPageNoZoom().x()) * _zoom;
}

force_inline bool DataStruct::needToCreateNewSheet() const
{
    cint len = this->lengthPage();

    if(un(len < 2))
        return true;

    if(at(len - 1).lengthStroke())
        return true;
    if(at(len - 2).lengthStroke())
        return true;

    return false;
}

force_inline double DataStruct::biggery() const noexcept
{
    const auto &last = lastPage();
    const auto zoom = this->getZoom();
    return (last.currentHeight() + this->getPointFirstPageNoZoom().y()) * zoom;
}

inline const __fast Page &DataStruct::at(const uint page) const
{
    return _page.at(page);
}

inline Page &DataStruct::at_mod(cint page)
{
    W_ASSERT(page >= 0 and page < this->lengthPage());
    return _page.operator[](page);
}

// this function is not threadSave
force_inline __slow Point DataStruct::at_draw_page(
        cint indexPoint,
        const Page &Page) const
{
    return DataStruct::at_draw_page(indexPoint, Page, getPointFirstPage(), _zoom);
}

force_inline Point DataStruct::at_draw_page(
        cint indexPoint,    const Page &Page,
        const QPointF &PointFirstPageWithZoom,
        cdouble zoom)
{
    Point point;

    Page.at_draw_page(indexPoint, PointFirstPageWithZoom, point, zoom);

    return point;
}

force_inline const Page &DataStruct::lastPage() const
{
    return this->_page.last();
}

inline void DataStruct::newPage(const n_style style)
{
    _page.append(Page(lengthPage() + 1, style));

    this->lastPage().setVisible(un(lengthPage() <= 2));
    //triggerVisibility(page::getHeight() * lengthPage());
}

inline QRectF DataStruct::get_size_area(const QList<QVector<int>> & pos, int base) const
{
    QRectF result;
    int i, len;

    if(un(pos.isEmpty()))
        return result;

    len = pos.length();
    result = get_size_area(pos.first(), base);

    for(i = 1; i < len; i ++){
        const auto &vec = pos.at(i);

        if(un(vec.isEmpty()))
            continue;

        const auto tmp = this->get_size_area(vec, base + i);
        result = DataStruct::get_bigger_rect(result, tmp);
    }

    return result;
}

inline QRectF DataStruct::get_size_area(
        const QVector<int>  &pos,
        int                 __page) const
{
    const Page & page = at(__page);
    return page.get_size_area(pos);
}

inline int DataStruct::getFirstPageVisible() const
{
    /* the reason this function
     * is constant is because
     *  we don't want QList to copy all pages
     *  when they are shared. */
    int &__pageVisible = (int &) _pageVisible;
    int i, len;
    int find;

    len = this->lengthPage();
    find = 0;

    if(un(_pageVisible < 0)){
        for(i = 0; i < len; i++){
            if(at(i).isVisible()){
                __pageVisible = i;
                find = 1;
                break;
            }
        }
    }else{
        find = 1;
    }

    if(un(!find)){
        //log_write->write("Impossibile to find first page visible", log_ui::critic_error);
        __pageVisible = 0;
        for(const auto &page : _page){
            page.setVisible(true);
        }
    }

    return __pageVisible;
}

/* this function does not consider the zoom */
inline double DataStruct::currentWidth() const
{
    return biggerx();
}

/*
 * Return the number of page visible
*/
inline int DataStruct::get_range_visible() const
{
    int i, len, count;

    len = this->lengthPage();
    i = this->getFirstPageVisible();
    count = 0;

    for(; i < len; i++){
        if(un(!at(i).isVisible()))
            break;

        count ++;
    }

    return count;

}

force_inline bool DataStruct::isOkTranslate(const QPointF &point, cbool isZoom) const
{
    const auto x = _pointFirstPage.x();
    const auto y = _pointFirstPage.y();

    if(!isZoom){
        return  x + point.x() <= 0. and
                y + point.y() <= 0.;
    }

    return x + point.x() / _zoom <= 0. and
           y + point.y() / _zoom <= 0.;
}

force_inline void DataStruct::triggerNewView(int page, int m_pos_ris, cbool all)
{
    at_mod(page).triggerRenderImage(m_pos_ris, all);
}

/**
 * @return >= 0 && < this->length()
*/
inline int DataStruct::whichPage(const Stroke &stroke) const
{
    int i;
    const auto &big = stroke.getBiggerPointInStroke();

    const auto &point = big.topLeft();
    i = this->whichPage(point);

    if (un(i < 0)) {
        const auto &point = big.bottomRight();
        i = this->whichPage(point);
    }

    W_ASSERT(i >= 0 and i < this->lengthPage());
    return i;
}

inline void DataStruct::triggerNewView(const QList<int> &Page, int m_pos_ris, cbool all)
{
    for(const int page: Page){
        this->triggerNewView(page, m_pos_ris, all);
    }
}

inline void DataStruct::triggerNewView(int m_pos_ris, cbool all)
{
    uint i, len;
    len = lengthPage();
    for(i = 0; i < len; i++)
        this->triggerNewView(i, m_pos_ris, all);
}

inline void DataStruct::triggerViewIfVisible(int m_pos_ris)
{
    uint i, len;
    len = lengthPage();
    for( i = 0; i < len; i++)
        if(at(i).isVisible())
            at_mod(i).triggerRenderImage(m_pos_ris, true);
}

[[nodiscard]] force_inline bool DataStruct::isOkZoom(const double newPossibleZoom)
{
    return !(newPossibleZoom >= 2.0 || newPossibleZoom <= 0.3);
}

inline int DataStruct::whichPage(const QPointF &point) const
{
    const double heigth = Page::getHeight();
    const not_used auto debug_which = false;

    W_ASSERT(point.y() >= 0.);

    const auto index = WCommonScript::diff(point.y() / heigth);

    W_ASSERT(index >= 0 and index < this->lengthPage());
    return index;
}

/* the function automatically launches the drawing for the pages
 * to which data has been added*/
inline void DataStruct::append(const QList<std::shared_ptr<Stroke>> &stroke, int m_pos_ris)
{
    QList<int> trigger;

    for (const auto &ref : qAsConst(stroke)) {
        /// get the page of the point
        const int WhichPage = this->appendStroke(ref);

        /// it the page is not in the list we append
        if (trigger.indexOf(WhichPage) == -1) {
            trigger.append(WhichPage);
        }
    }

    this->triggerNewView(trigger, m_pos_ris, false);
}

inline void DataStruct::removeAt(const uint indexPage){
    int index = indexPage, len;
    this->_page.removeAt(indexPage);

    Q_ASSERT(indexPage < (uint)this->lengthPage());

    len = lengthPage();

    for(; index < len; index ++){
        at_mod(index).setCount(index + 1);
    }

}

inline int DataStruct::appendStroke(const std::shared_ptr<Stroke>& stroke)
{
    int page;

    W_ASSERT(stroke);

    page = this->adjustStroke(*stroke);

    this->appendStroke(stroke, page);

    return page;
}

inline void DataStruct::appendStroke(std::shared_ptr<Stroke> stroke, const int page)
{
    this->at_mod(page).append(std::move(stroke));
}

inline QRectF DataStruct::get_bigger_rect(
        const QRectF    &first,
        const QRectF    &second)
{
    QPointF resultTopLeft(first.topLeft());
    QPointF resultBottomRight(first.bottomRight());

    const QPointF SecTopLeft(second.topLeft());
    const QPointF SecBottomRight(second.bottomRight());

    if(resultTopLeft.x() > SecTopLeft.x())
        resultTopLeft.setX(SecTopLeft.x());

    if(resultTopLeft.y() > SecTopLeft.y())
        resultTopLeft.setY(SecTopLeft.y());

    if(resultBottomRight.x() < SecBottomRight.x())
        resultBottomRight.setX(SecBottomRight.x());

    if(resultBottomRight.y() < SecBottomRight.y())
        resultBottomRight.setY(SecBottomRight.y());

    return QRectF(resultTopLeft, resultBottomRight);
}

force_inline QRect DataStruct::get_bigger_rect(const QRect &first, const QRect &second)
{
    QRectF __first(first);
    QRectF __second(second);
    return DataStruct::get_bigger_rect(__first, __second).toRect();
}

inline void DataStruct::setZoom(typeof(DataStruct::_zoom) newZoom)
{
    W_ASSERT(isOkZoom(newZoom));
    _zoom = newZoom;
}

inline void DataStruct::setPageVisible(int page)
{
    W_ASSERT(page == -1 or (page >= 0 and page < this->lengthPage()));
    _pageVisible = page;
}

inline int DataStruct::adjustStroke(Stroke &stroke) const
{
    int page;

    stroke.adjust(_zoom);

    page = this->whichPage(stroke);

    return page;
}

constexpr force_inline QPointF DataStruct::adjustPoint(const QPointF &pointTouchUser) const
{
    return (pointTouchUser / getZoom() - this->getPointFirstPageNoZoom());
}

constexpr force_inline QPointF DataStruct::adjustPointReverce(const QPointF &pointDatastruct) const
{
    return (pointDatastruct + this->getPointFirstPageNoZoom()) * getZoom();
}

force_inline bool DataStruct::isempty_touch() const
{
    return un(_page.isEmpty());
}

force_inline void DataStruct::setVisible(int from, int to)
{
    W_ASSERT(from <= to);
    int i = 0;

    for(auto &page : _page){
        page.setVisible(i >= from and i <= to);
        i++;
    }

    this->_pageVisible = -1;
}

inline void DataStruct::decreaseAlfa(const QVector<int> &pos, int index)
{
    at_mod(index).decreseAlfa(pos, 4);
}
