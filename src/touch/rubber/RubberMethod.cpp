#include "RubberMethod.h"
#include "touch/tabletcanvas.h"
#include "touch/multi_thread_data.h"
#include "touch/rubber/rubber_ui.h"
#include "touch/dataTouch/stroke/StrokeNormal.h"

constexpr bool rubber_debug = false;

struct RubberPrivateData{
    QVector<int>    *data_find;
    Page            *__page;
    WLine           line;
    datastruct      *data;
    QVector<int>    *data_to_remove;

    int             al_find;
    bool            highlighter_delete;
    QRect           area;
};

static volatile int     __m_size_gomma;
static WMutex           single_mutex;
static WMutex           mutex_area;
static thread_group_sem *thread_group;

static void (*functionToCall)(DataPrivateMuThread *);

void *idle_rubber(void *arg)
{
    auto *data = (struct DataPrivateMuThread *)arg;

    for(;;){
        sem_wait(thread_group->get_pass_sem());

        if(unlikely(thread_group->needToDelete())){
            return nullptr;
        }

        if(unlikely(data->id >= thread_group->get_create())){
            goto wait;
        }

        functionToCall(data);
        wait:
        sem_post(thread_group->get_finish_sem());
        sem_wait(thread_group->get_all_finish_sem());
    }
}

RubberMethod::RubberMethod()
{
    WNew(thread_group, thread_group_sem, ());
    thread_group->startLoop(idle_rubber);
}

static inline not_used QRectF rubber_get_area(const QPointF &p1, const QPointF &p2)
{
    const auto &size = __m_size_gomma;

    QRectF area = datastruct_rect(p1, p2);
    const QPointF tmp = QPointF(size, size);

    W_ASSERT(__m_size_gomma >= 0);
    W_ASSERT(area.topLeft().x() >= 0.);
    W_ASSERT(area.topLeft().y() >= 0.);

    area = QRectF(area.topLeft() - tmp,
                  area.bottomRight() + tmp);

    return area;
}

void RubberMethod::initRubber(const QPointF &point)
{
    auto *_canvas = core::get_canvas();
    datastruct *data = _canvas->getDoc();
    _last = true;
    _last = data->adjustPoint(point);
}

static force_inline void draw_null(Page *_page, const QVector<int> &point,
                                   const QVector<int> &stroke, bool is_left)
{
    Q_ASSERT(point.size() == stroke.size());

    WCommonScript::for_each(point, stroke, [&_page, is_left](
                                const int indexPoint,
                                const int indexStroke)
    {
        auto &stroke = _page->atStrokeMod(indexStroke);

        W_ASSERT(stroke.type() == Stroke::COMPLEX_NORMAL);

        _page->lock();
        _page->drawForceColorStroke(stroke, -1, COLOR_NULL, nullptr);
        _page->unlock();

        if (is_left) {
            ((StrokeNormal*) &stroke)->removeAt(0, indexPoint);
        } else {
            ((StrokeNormal*) &stroke)->removeAt(indexPoint, -1);
        }

        _page->lock();
        _page->drawStroke(stroke, -1);
        _page->unlock();
    });
}

void actionRubberSinglePartial(DataPrivateMuThread *data)
{
    auto *private_data = (RubberPrivateData *)data->extra;

    QVector<int> stroke_to_remove;
    QVector<int> stroke_mod_point,          stroke_mod_stroke;
    QVector<int> stroke_mod_rigth_point,    stroke_mod_rigth_stroke;
    QVector<int> stroke_mod_left_point,     stroke_mod_left_stroke;

    int from, to, _index;

    Page *_page             = private_data->__page;
    datastruct *_datastruct = private_data->data;
    const auto &area        = private_data->line;

    from = data->from;
    to = data->to;

    stroke_to_remove.reserve(12);

    W_ASSERT(from <= to);

    for(; from < to; from ++){
        Stroke & stroke = _page->atStrokeMod(from);

        if(unlikely(stroke.isEmpty())){
            stroke_to_remove.append(from);
            continue;
        }

        _index = 0;
        while(true){
            int lenPoint;
            const int index = stroke.is_inside(area, _index, __m_size_gomma, true);

            if(index < 0)
                break;

            lenPoint = stroke.length();
            W_ASSERT(stroke.is_normal());

            if(index < 3){
                if(stroke.length() - index < 3){
                    stroke_to_remove.append(data->from);

                    // we need to exit the current stroke
                    break;
                }

                W_ASSERT(stroke.length() >= 2);

                stroke_mod_left_point.append(index);
                stroke_mod_left_stroke.append(from);
                //stroke.removeAt(0, index);

                goto out;
            }

            if(index + 3 > lenPoint){
                if(stroke.length() < 3)
                    stroke_to_remove.append(data->from);

                stroke_mod_rigth_point.append(index);
                stroke_mod_rigth_stroke.append(from);
                //stroke.removeAt(index, lenPoint - 1);

                break;
            }

            W_ASSERT(lenPoint >= 6);

            stroke_mod_point.append(index);
            stroke_mod_stroke.append(from);

            out:
            _index = index + 1;
        }
    }

    // we don't need to do this operation
    // in order to the list
    {
        int i = stroke_mod_stroke.length();
        W_ASSERT(stroke_mod_stroke.size() == stroke_mod_point.length());

        for(i-- ; i >= 0; i --){
            cint indexStroke    = stroke_mod_stroke.at(i);
            cint indexPoint     = stroke_mod_point.at(i);

            _datastruct->changeIdThreadSave(indexPoint, _page->atStrokeMod(indexStroke), *_page);
        }
    }


    single_mutex.lock();

    draw_null(_page, stroke_mod_left_point,  stroke_mod_left_stroke,   true);
    draw_null(_page, stroke_mod_rigth_point, stroke_mod_rigth_stroke,  false);

    // append to the list the index of the stroke to be remove
    private_data->data_to_remove->append(stroke_to_remove);

    single_mutex.unlock();
}

void actionRubberSingleTotal(DataPrivateMuThread *data)
{
    RubberPrivateData *private_data = (RubberPrivateData *)data->extra;

    QVector<int> index_selected;
    cint data_already_len   = private_data->al_find;

    Page *_page             = private_data->__page;
    QVector<int> *_al_find  = private_data->data_find;
    const auto &area        = private_data->line;
    index_selected.reserve(32);

    Q_ASSERT(data->from <= data->to);

    for(; data->from < data->to; data->from++){
        auto &stroke = _page->atStrokeMod(data->from);
        int index;

        if(WCommonScript::is_present_in_list(_al_find->constData(), data_already_len, data->from))
            continue;

        index = stroke.is_inside(area, 0, __m_size_gomma, false);

        if(index < 0){
            continue;
        }

        if(unlikely(stroke.is_highlighter())){
            private_data->highlighter_delete = true;

            const auto currentArea = stroke.getBiggerPointInStroke();

            mutex_area.lock();

            private_data->area = datastruct::get_bigger_rect(currentArea, private_data->area);

            mutex_area.unlock();
        }

        index_selected.append(data->from);

    }

    if(index_selected.isEmpty()){
        return;
    }

    single_mutex.lock();

    _al_find->append(index_selected);
    private_data->data->decreaseAlfa(index_selected, _page->_count - 1);

    single_mutex.unlock();
}

void RubberMethod::actionRubber(const QPointF &__lastPoint, type_rubber typeRubber, int size_gomma)
{
    auto *_canvas = core::get_canvas();
    datastruct *data = _canvas->getDoc();
    int lenStroke, count, indexPage, thread_create;
    cbool isTotal = (typeRubber == RubberMethod::total);
    const QPointF &lastPoint = data->adjustPoint(__lastPoint);
    auto *dataThread = thread_group->get_thread_data();
    RubberPrivateData dataPrivate;

    W_ASSERT(_last.isSet());

    if(isTotal){
        functionToCall = actionRubberSingleTotal;
    }else{
        functionToCall = actionRubberSinglePartial;
    }

    /* se è la seconda volta che viene chiamata la funzione
    * allora dobbiamo solamente settare _base, altrimenti
    * dobbiamo controllare se si sta rimuovendo qualcosa
    * dalla pagina precedente o dalla successiva
    */
    indexPage = _base;
    count = 0;
    dataPrivate.highlighter_delete = false;
    dataPrivate.area = QRect();

    if(unlikely(_base < 0)){
        this->_base = data->whichPage(lastPoint);
        _data_to_remove.append(QVector<int>());
        indexPage = _base;
    }else{
        const auto now = data->whichPage(lastPoint);
        int i;

        if(likely(now == _base))
            goto out1;

        /*
         * Il punto che ha selezionato adesso l'utente
         * si trova su una pagina precedente a quella che ha
         * selezionato prima
        */
        if(now < _base)
        {
            for(i = now; i < _base; i++)
            {
                _data_to_remove.insert(0, QVector<int>());
            }

            _base = now;
            count = 0;
        }
        else if(now > _base)
        {
            indexPage = now;
            count = now - _base;

            if(!(count < _data_to_remove.length()))
            {
                for(i = _base; i < now; i++)
                {
                    _data_to_remove.append(QVector<int>());
                }
            }
        }

        W_ASSERT(now >= 0);
    }

    out1:

    if(!is_image_not_null(indexPage, &data->at_mod(indexPage), lastPoint, _last, size_gomma)){
        WDebug(rubber_debug, "It's null");
        goto save_point;
    }

    WDebug(rubber_debug, "It's not null");


    // l'utente ha prima selezionato un punto su una pagina x,
    // e poi ne ha selezionato un altro su una pagina o x-1, o x+1
    if(unlikely(data->whichPage(lastPoint) != data->whichPage(_last))){
        goto save_point;
    }

    dataPrivate.data        = data;
    dataPrivate.line       = WLine(_last, lastPoint);

    __m_size_gomma = size_gomma;

    dataPrivate.__page = &data->at_mod(indexPage);

    lenStroke = dataPrivate.__page->lengthStroke();

    if(unlikely(!lenStroke))
        goto out2;

    // we trigger the copy if the page is shared
    // we can't do after
    dataPrivate.__page->atStrokeMod(0);

    dataPrivate.data_find      = &_data_to_remove.operator[](count);
    dataPrivate.data_to_remove = dataPrivate.data_find;
    dataPrivate.al_find        = dataPrivate.data_find->length();

    thread_create = DataPrivateMuThreadInit(dataThread, &dataPrivate,
                                            thread_group->get_max(),
                                            lenStroke, DATA_PRIVATE_FLAG_SEM);

    thread_group->postAndWait(thread_create);

    if(unlikely(dataPrivate.highlighter_delete)){
        dataPrivate.__page->drawIfInside(-1, dataPrivate.area);
    }

    if(!isTotal){
        dataPrivate.__page->removeAt(*dataPrivate.data_to_remove);

        dataPrivate.__page->mergeList();

    }


    out2:
    if(!isTotal){
        _base = -1;
        _data_to_remove.clear();
    }

    save_point:
    _last = lastPoint;
}

bool RubberMethod::is_image_not_null(int index, const Page *page,
                                     const QPointF &from, const QPointF &to,
                                     int delta)
{
    int i, j;

    W_ASSERT(index >= 0);
    W_ASSERT(page);
    W_ASSERT(delta > 0);

    const auto &img = page->getImg();

    const auto ymin = qMin(from.y(), to.y());
    const auto ymax = qMax(from.y(), to.y());
    const auto xmin = qMin(from.x(), to.x());
    const auto xmax = qMax(from.x(), to.x());

    const int x = xmin - delta;
    const int y = ymin - Page::getHeight() * index - delta;
    W_ASSERT(y >= 0. and y <= Page::getHeight());

    delta *= 2;

    if(unlikely(img.isNull()))
        return false;

    for(i = 0; i < delta + int(xmax - xmin); i++)
    {
        for(j = 0; j < delta + int(ymax - ymin); j++)
        {
            const QPoint target = QPoint(x + i, y + j) * PROP_RESOLUTION;
            const QRgb pix = img.pixel(target);

            //WDebug(rubber_debug, name << "pixel" << pix);

            if(pix){
                WDebug(rubber_debug, "yes");
                return true;
            }
        }
    }

    WDebug(rubber_debug, "No");
    return false;
}

int RubberMethod::endRubber(type_rubber method)
{
    datastruct *data = core::get_canvas()->getDoc();
    int i, len = _data_to_remove.length();
    int index_mod = -1;

    W_ASSERT(data);

    if(method == type_rubber::total){

        for(i = 0; i < len; i ++){
            QVector<int> &arr = _data_to_remove.operator[](i);
            Page &page = data->at_mod(i + _base);

            if(unlikely(arr.isEmpty()))
                continue;

            if(unlikely(index_mod >= 0)){
                index_mod = -2;
            }

            if(unlikely(index_mod == -1)){
                index_mod = i + _base;
            }

            WCommonScript::order_vector(arr);

            const auto rect = data->get_size_area(arr, i + _base);
            page.removeAndDraw(-1, arr, rect);
        }

        _data_to_remove.clear();
    }

    this->reset();
    return index_mod;
}

RubberMethod::~RubberMethod()
{
    WDelete(thread_group);
}
