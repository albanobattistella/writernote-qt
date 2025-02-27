﻿#include "page.h"
#include "sheet/fast-sheet/fast_sheet_ui.h"
#include "touch/tabletcanvas.h"
#include <QPainter>
#include "utils/time/current_time.h"
#include <QPainterPath>
#include "utils/common_error_definition.h"
#include "datawrite/source_read_ext.h"
#include "time.h"
#include "utils/common_script.h"
#include "touch/multi_thread_data.h"
#include "testing/memtest.h"
#include <QPaintDevice>
#include "touch/datastruct/page_file.h"
#include "datawrite/source_read_ext.h"

#define PAGE_THREAD_MAX 16

static force_inline double widthToPressure(double v) { return v/10.0; };

static void setStylePrivate(
    bool            &fast,
    n_style         res,
    style_struct_S  &style)
{
    if(res == n_style::empty){
        res = n_style::square;
    }

    if(res == n_style::line){
        fast = true;

        style.nx = TEMP_N_X;
        style.ny = 0;
    }
    else if(res == n_style::white){
        /* we set the color manually */
        style.colore.fromColor(Qt::black);

        style.nx = 1;
        style.ny = 1;
        style.thickness = 1;
    }else{
        fast = true;

        style.nx = TEMP_SQUARE*(page::getHeight() / page::getWidth());
        style.ny = TEMP_SQUARE;
    }
}

static force_inline void __initImg(QImage &img)
{
    img = QImage(page::getResolutionWidth(), page::getResolutionHeigth(), QImage::Format_ARGB32);
    W_ASSERT(!img.isNull());
}

page::page(const int count, const n_style style)
{
    this->_count = count;
    this->_IsVisible = true;
    drawNewPage(style);
    this->mergeList();

    pthread_mutex_init(&_img, NULL);
    pthread_mutex_init(&_append_load, NULL);
}

static inline void drawLineOrizzontal(
        stroke          &stroke,
        style_struct_S  &style,
        cdouble         &last,
        double          &deltax,
        cdouble         &width_p,
        cdouble         &ct_del)
{
    int i;
    point_s point;

    W_ASSERT(ct_del > 0);

    for(i = 0; i < style.nx; i++){
        point._x = 20;
        point._y = last + deltax - 20;

        stroke.append(point, stroke_append_default);

        point._x = width_p - 20;
        stroke.append(point, stroke_append_default);

        deltax += ct_del;
    }
}

static inline void drawLineVertical(
        stroke          &stroke,
        style_struct_S  &style,
        const double    &last,
        double          &deltay,
        const double    height_p)
{
    const double ct_del = deltay;
    int i;
    point_s point;

    W_ASSERT(height_p);
    W_ASSERT(ct_del);

    for(i = 0; i < style.ny; i++){
        point._x = deltay  - 20;
        point._y = last    + 20; /* corrisponde to 0 */

        stroke.append(point, stroke_append_default);

        point._y = height_p + last - 20;
        stroke.append(point, stroke_append_default);

        deltay += ct_del;
    }
}

void page::drawNewPage(n_style __style)
{
    bool fast = false;
    double deltax, deltay, ct_del;
    struct style_struct_S style;
    cdouble width_p    = this->getWidth();
    cdouble height_p   = this->getHeight();
    cdouble last = (_count - 1)*page::getHeight();

    stroke &stroke = this->_stroke_writernote;

    setStylePrivate(fast, __style, style);

    if(likely(fast)){
        style.colore.fromColor(TEMP_COLOR);
        style.thickness =  TEMP_TICK;
    }

    if(unlikely(style.thickness <= 0.0))
        style.thickness = 1;

    stroke.setMetadata(-1, style.colore);

    if(unlikely(style.nx <= 0))
        style.nx = 1;

    if(unlikely(style.ny <= 0))
        style.ny = 1;

    deltax = height_p / (double)style.nx;
    deltay = width_p / (double)style.ny;

    ct_del = deltax;

    drawLineOrizzontal( stroke, style, last, deltax, width_p, ct_del);
    drawLineVertical(   stroke, style, last, deltay, height_p);

    stroke.__setPressureFirstPoint(    widthToPressure(style.thickness));
}

void page::swap(
        QList<stroke>       &list,
        const QVector<int>  &pos,
        int                 flag)
{
    int i, index;
    QRectF area;

#ifdef DEBUGINFO
    if(unlikely(!is_order_vector(pos))){
        qDebug() << "List not order" << __FILE__ << __FUNCTION__;
    }
#endif

    if(!(flag & PAGE_SWAP_TRIGGER_VIEW)){
        i = pos.length();
        for(i--; i >= 0; i--){
            index = pos.at(i);
            this->swap(list, index, index+1);
        }
        return;
    }

    for(cint ref : pos){
        const stroke & stroke = atStroke(ref);
        list.append(stroke);
    }

    area = this->get_size_area(pos);

    this->removeAndDraw(-1, pos, area);
}

/*
 * this function mantain the item already in list
*/
void page::swap(QList<stroke> & list,
                int             from,
                int             to)
{
    W_ASSERT(from >= to);
    DO_IF_DEBUG(
    int drop = 0;
    QList<int> itemDrop;
    );

    for(to --; from <= to; to --){
        list.append(_stroke.takeAt(to));

        DO_IF_DEBUG_ENABLE(debugPage, drop ++);
        DO_IF_DEBUG_ENABLE(debugPage, itemDrop.append(to));
    }

    DO_IF_DEBUG_ENABLE(debugPage,
        qDebug() << "Page::swap" << _count - 1 << drop << "Item drop, list" << itemDrop;
            )
}

/* the list should be order */
void page::removeAt(const QVector<int> &pos)
{
    int i;
    if(unlikely(!is_order_vector(pos))){
        DO_IF_DEBUG(std::abort());
        order_vector((QVector<int> &)(pos));
    }

    i = pos.length();
    for(i--; i >= 0; i--){
        this->removeAt(i);
    }
}

void page::append(const QList<stroke> &stroke)
{
    for(const auto & __tmp : qAsConst(stroke)){
        this->append(__tmp);
    }
}

void page::drawStroke(
        QPainter        &painter,
        const stroke    &stroke,
        QPen            &m_pen,
        const QColor    &color) const
{
    const bool isRubber = (color == COLOR_NULL);
    const bool isHigh = stroke.get_alfa() < 255;
    const auto last_comp_mode = painter.compositionMode();
    constexpr not_used bool measureTime = false;
    constexpr not_used bool debColor = false;
    cint page = _count - 1;

    m_pen.setColor(color);

    if(unlikely(!painter.isActive())){
        if(debug_enable()){
            qDebug() << "page::drawStroke" << "painter not active";
            W_ASSERT(false);
        }
        else{
            log_write->write("Painter not active", log_ui::possible_bug);
        }
        return;
    }

    if(unlikely(isRubber)){
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
    }else if(isHigh){
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    stroke.draw(painter, isRubber, page, m_pen, PROP_RESOLUTION);

    if(unlikely(isRubber)){
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    painter.setCompositionMode(last_comp_mode);

    W_ASSERT(painter.isActive());
}

struct page_thread_data{
    QVector<int>      * to_remove;
    pthread_mutex_t   * append;
    QPainter          * painter;
    QList<stroke>     * m_stroke;
    int                 m_pos_ris;
    const page        * parent;
};

/*The only way to draw in thread safe on a qpainter is to draw an image,
 *  as it is very fast compared to drawing the stroke, first we draw
 *  all the strokes on the images, residing in the thread stack stacks,
 *  and then we draw them on top the original painter
*/
void * __page_load(void *__data)
{
    auto *  _data = (struct DataPrivateMuThread *)__data;
    auto *  extra = (struct page_thread_data *)_data->extra;
    QImage img;
    Define_PEN(m_pen);
    pthread_mutex_t *mutex = extra->append;
    int m_pos_ris = extra->m_pos_ris;
    const page *page = extra->parent;

    __initImg(img);
    Define_PAINTER_p(painter, img);

    for(; _data->from < _data->to; _data->from ++){
        const auto &ref = extra->m_stroke->at(_data->from);

        if(unlikely(ref.isEmpty())){
            pthread_mutex_lock(mutex);

            extra->to_remove->append(_data->from);

            pthread_mutex_unlock(mutex);

            continue;
        }

        const QColor &color = ref.getColor(
            (unlikely(m_pos_ris != -1))
                    ?
                        (
                          (ref.getPosizioneAudio() > m_pos_ris)
                            ? 4
                            : 1
                        )
                    :
                        1
        );

        page->drawStroke(painter, ref, m_pen, color);
    }

    End_painter(painter);

    // the source image has the same size as img
    pthread_mutex_lock(mutex);

    W_ASSERT(extra->painter->isActive());
    extra->painter->drawImage(img.rect(), img, img.rect());

    pthread_mutex_unlock(mutex);

    return NULL;
}

void page::drawEngine(
        QPainter        &painter,
        QList<stroke>   &List,
        int             m_pos_ris,
        bool            *changeSomething,
        cbool           use_multi_thread)
{
    int i, threadCount;

    QVector<int> to_remove;

    pthread_t thread[PAGE_THREAD_MAX];
    struct DataPrivateMuThread threadData[PAGE_THREAD_MAX];
    struct page_thread_data extraData;

    extraData.append        = &_append_load;
    extraData.painter       = &painter;
    extraData.to_remove     = &to_remove;
    extraData.m_stroke      = &List;
    extraData.m_pos_ris     = m_pos_ris;
    extraData.parent        = this;

    if(use_multi_thread){
        threadCount = DataPrivateMuThreadInit(threadData, &extraData, PAGE_THREAD_MAX, List.length(), ~DATA_PRIVATE_FLAG_SEM);

        for(i = 0; i < threadCount; i++){
            pthread_create(&thread[i], NULL, __page_load, &threadData[i]);
        }
        for(i = 0; i < threadCount; i++){
            pthread_join(thread[i], NULL);
        }
    }else{
        threadData->extra   = &extraData;
        threadData->from    = 0;
        threadData->to      = List.length();

        __page_load(&threadData[0]);
    }

    if(unlikely(to_remove.length())){
        order_vector(to_remove);

        if(likely(changeSomething))
            *changeSomething = true;

        log_write->write("Stroke is empty", log_ui::type_write::possible_bug);

        this->removeAt(to_remove);
    }
}

inline void page::draw(
    QPainter    &painter,
    int         m_pos_ris,
    bool        all)
{
    auto list = _strokeTmp.toList();
    bool changeSomething = true;

    W_ASSERT(painter.isActive());

    if(unlikely(all)){
        this->drawEngine(painter, this->_stroke, m_pos_ris, NULL, true);
    }

    if(list.length()){
        this->drawEngine(painter, list, m_pos_ris, &changeSomething, false);
    }

    W_ASSERT(painter.isActive());

    if(unlikely(changeSomething)){
        this->_strokeTmp = QVector<stroke>::fromList(list);
    }

    this->mergeList();
}

void page::mergeList()
{
    int i;
    const int len = this->_strokeTmp.length();
    int index = _stroke.length();

    for(i = 0; i < len; i++){
        const stroke &stroke = _strokeTmp.at(i);

        _stroke.append(stroke);

        index ++;
    }

    _strokeTmp.clear();
}

void page::drawToImage(
    const QVector<int>  &index, 
    QImage              &img,
    cint                flag) const
{
    Define_PEN(pen);

    if(flag & DR_IMG_INIT_IMG){
        __initImg(img);
    }

    Define_PAINTER_p(painter, img);

    for (const int __index : index){
        const stroke &stroke = atStroke(__index);
        this->drawStroke(painter, stroke, pen, stroke.getColor());
    }

    W_ASSERT(painter.isActive());
    End_painter(painter);
}

bool page::userWrittenSomething() const
{
    return this->_stroke.length();
}

bool page::initImg(bool flag)
{
    flag = _imgDraw.isNull() || flag;
    if(flag)
        __initImg(_imgDraw);

    return flag;
}

void page::decreseAlfa(const QVector<int> &pos, QPainter * painter, int decrese)
{
    int i = pos.length();
    uint color;
    Define_PEN(m_pen);

    for(i --; i >= 0; i--){
        stroke &stroke = atStrokeMod(pos.at(i));
        color = stroke.get_alfa();

        stroke.setAlfaColor(color / decrese);

        if(painter){
            this->drawStroke(*painter, stroke, m_pen, COLOR_NULL);
            this->drawStroke(*painter, stroke, m_pen, stroke.getColor());
        }
    }

    W_ASSERT(painter->isActive());
}

/*
 * all --> indicates if all the points must be drawn from scratch, if false it is drawn over the old image
*/
void page::triggerRenderImage(int m_pos_ris, bool all)
{
    __is_ok_count();

    all = initImg(all);

    Define_PAINTER(painter);

    this->draw(painter, m_pos_ris, all);

    W_ASSERT(painter.isActive());
    End_painter(painter);

    /*return;
    if(!imgDraw.save("~/Scrivania/tmp_foto/foto"+current_time_string()+".png", "PNG", 0))
        std::abort();*/
}

#define MakeCTRL(point) \
    if(datastruct::isinside(topLeft1, bottomRight1, point)) \
        return true;

static force_inline int __is_inside_square(
        const QRectF    &rect1,
        const QRectF    &rect2)
{
    const QPointF &topLeft1     = rect1.topLeft();
    const QPointF &bottomRight1 = rect1.bottomRight();

    MakeCTRL(rect2.bottomRight());
    MakeCTRL(rect2.bottomLeft());

    MakeCTRL(rect2.topRight());
    MakeCTRL(rect2.topLeft());

    return false;
}

static force_inline int is_inside_squade(
        const QRectF    &rect1,
        const QRectF    &rect2)
{
    // testing
    return rect1.intersects(rect2);
    return __is_inside_square(rect1, rect2) || __is_inside_square(rect2, rect1);
}

void page::removeAndDraw(
        int                 m_pos_ris,
        const QVector<int>  &pos,
        const QRectF        &area)
{
    int i = pos.length();
    this->drawForceColorStroke(pos, m_pos_ris, COLOR_NULL);

    for(i --; i >= 0; i --){
        removeAt(pos[i]);
    }

    drawIfInside(m_pos_ris, area);
}

void page::drawIfInside(int m_pos_ris, const QRectF &area)
{
    int index = lengthStroke() - 1;
    Define_PAINTER(painter);

    for(; index >= 0; index --){
        const stroke &stroke = this->atStroke(index);

        if(unlikely(is_inside_squade(stroke.getBiggerPointInStroke(), area))){
            drawForceColorStroke(stroke, m_pos_ris, stroke.getColor(1.0), &painter);
        }
    }

    End_painter(painter);
}

#define PAGE_DRAW_SQUARE_ADJUST(point, function) \
    point._x = rect.function().x(); \
    point._y = rect.function().y(); \
    at_translation(point, this->_count - 1);

void page::drawSquare(const QRect &rect)
{
    QRect tmp;
    QBrush brush(COLOR_NULL, Qt::SolidPattern);
    Define_PEN(pen);
    Define_PAINTER(painter);

    // we need to adjust the rect to our img
    {
        point_s point1, point2;
        PAGE_DRAW_SQUARE_ADJUST(point1, topLeft);
        PAGE_DRAW_SQUARE_ADJUST(point2, bottomRight);

        tmp = QRect(point1.toQPointF(1.0).toPoint(),
                    point2.toQPointF(1.0).toPoint());
    }

    pen.setColor(COLOR_NULL);
    pen.setBrush(brush);

    painter.setPen(pen);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(tmp, brush);
    End_painter(painter);
}

void page::decreseAlfa(const QVector<int> &pos, int decrese)
{
    bool needInit = initImg(false);

    if(unlikely(needInit)){
        WDebug(debugPage, "Warning: page not draw");
        return this->triggerRenderImage(-1, true);
    }

    Define_PAINTER(painter);

    this->decreseAlfa(pos, &painter, decrese);

    End_painter(painter);
}

QRect page::get_size_area(const QList<stroke> &item, int from, int to)
{
    QRect result;

    if(unlikely(from >= to)){
        return QRect();
    }

    result = item.at(from).getBiggerPointInStroke();

    for(; from < to; from ++){
        const QRect tmp = item.at(from).getBiggerPointInStroke();
        result = datastruct::get_bigger_rect(result, tmp);
    }

    return result;
}

page::page()
{
    this->_count = -1;
}

QRect page::get_size_area(const QVector<int> &pos) const
{
    QRect result;
    int len = pos.length();
    QRect tmp;

    if(unlikely(!len)){
        return QRect();
    }

    len --;
    result = atStroke(pos.first()).getBiggerPointInStroke();

    for(; len >= 0; len --){
        tmp = atStroke(pos.at(len)).getBiggerPointInStroke();
        result = datastruct::get_bigger_rect(result, tmp);
    }

    return result;
}

void page::setCount(int newCount)
{
    int i = this->lengthStroke();
    int delta = newCount - this->_count;
    cdouble deltaY = page::getHeight() * delta;

    if(unlikely(this->_count < 0)){
        _count = newCount;
        return;
    }

    for(i --; i >= 0; i--){
        this->atStrokeMod(i).scale(QPointF(0., deltaY));
    }

    this->_stroke_writernote.scale(QPointF(0., deltaY));

    this->_count = newCount;

    __is_ok_count();
}

void page::drawForceColorStroke(const QVector<int> &pos, int m_pos_ris, const QColor &color)
{
    if(initImg(false))
        return this->triggerRenderImage(m_pos_ris, true);

    Define_PAINTER(painter);

    for(const auto &index : pos){
        const stroke &stroke = atStroke(index);
        this->drawForceColorStroke(stroke, m_pos_ris, color, &painter);
    }

    W_ASSERT(painter.isActive());
    End_painter(painter);
}

void page::allocateStroke(int numAllocation)
{
    for(int i = 0; i < numAllocation; i++){
        this->_stroke.append(stroke());
    }
}

#define DO_CTRL(function) \
    err = function; \
    if(unlikely(err != OK)) \
        return err;

int page::save(zip_source_t *file, cbool saveImg) const
{
    return page_file::save(this, file, saveImg);
}

int page::load(zip_file_t *file, int ver_stroke)
{
    return page_file::load(*this, ver_stroke, file);
}

void page::drawStroke(const stroke &stroke, int m_pos_ris)
{
    drawForceColorStroke(stroke, m_pos_ris, stroke.getColor(1.0), NULL);
}
