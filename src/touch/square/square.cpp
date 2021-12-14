#include "square.h"
#include <QPen>
#include <QPainter>
#include "../../currenttitle/document.h"
#include <QDebug>
#include "../../images/fromimage.h"
#include "../property/property_control.h"
#include "../tabletcanvas.h"

square::square(QObject *parent, property_control *property):
    QObject(parent)
{
    Q_ASSERT(parent);
    Q_ASSERT(property);

    this->m_property = property;
    this->m_copy = new copy();
    this->canvas = (TabletCanvas *) parent;

    QObject::connect(m_property, &property_control::ActionSelection, this, &square::actionProperty);

    penna.setStyle(Qt::DotLine);
    penna.setWidth(2);
    penna.setColor(QColor::fromRgb(30, 90, 255));
    this->reset();
}

square::~square()
{
    delete this->m_copy;
}

void square::updatePoint(const QPointF &__point)
{
    if(!pointinit.set){
        pointinit.point = __point;
        pointinit.set = true;

        /* we don't need yet to draw somethings */
        __need_reload = false;
        in_box = false;
        return;
    }

    pointfine.point = __point;
    __need_reload = true;

}

/* la funzione capisce se all'interno del quadrato della selezione c'è qualcosa
 * in caso salva l'id del tratto e setta la variabile this->check = true, in caso contrario
 * la setta = false e fa il return
*/
bool square::find(Document *doc){
    datastruct *data = doc->datatouch;
    uint counterStroke, pageCounter, lenStroke;
    bool tmp_find;
    const QPointF &translation = data->getPointFirstPage();
    const uint lenPage = data->lengthPage();

    const QPointF &topLeft = data->adjustPoint(pointinit.point);
    const QPointF &bottomRight = data->adjustPoint(pointfine.point);

    this->in_box = false;
    this->point_touch = true;

    this->adjustPoint();

    /* point selected by user */
    for(pageCounter = 0; pageCounter < lenPage; pageCounter++){
        lenStroke = data->at(pageCounter).lengthStroke();

        for(counterStroke = 0; counterStroke < lenStroke; counterStroke++){
            const stroke &stroke = data->at(pageCounter).atStroke(counterStroke);

            if(datastruct::isinside(topLeft + translation , bottomRight + translation, stroke)){
                if(m_id.indexOf(stroke.getId()) == -1) {
                    m_id.append(stroke.getId());
                }

                this->in_box = true;
            }

        }
    }

    /* image selected by user */
    const int lenImg = doc->m_img->m_img.length();
    for(int counterImg = 0; counterImg < lenImg; counterImg++){
        tmp_find = false;
        const auto &ref = doc->m_img->m_img.at(counterImg);

        if(datastruct::isinside(topLeft, bottomRight, ref.i))
            tmp_find = true;

        if(datastruct::isinside(topLeft, bottomRight, ref.f))
            tmp_find = true;

        if(!tmp_find)
            continue;

        this->m_index_img.append(counterImg);
        this->in_box = true;
    }

    findObjectToDraw();

    if(!in_box){
        reset();
    }else{
        __need_reload = true;
    }

    return in_box;
}

/* la funzione resistuisce
 * vero se è intero il punto è interno
*/
bool square::isinside(const QPointF &point){
    if(!this->in_box)
        return false;

    return datastruct::isinside(pointinit.point, pointfine.point, point);
}

void square::needReload(QPainter &painter, const QWidget *pixmap){
    QPoint middle;
    int flag;
    const QPoint &translation = -pixmap->mapFromGlobal(QPoint(0, 0));

    //qDebug() << "square::needReload in_box:" << in_box;

    if(!this->__need_reload) return;

    //qDebug() << point_touch;

    if(this->point_touch){
        middle = QPoint(translation.x() + this->pointinit.point.x(),
                        pointinit.point.y() + translation.y() - m_property->height());
        flag = this->calculate_flags();

        //qDebug() << "square::needReload show" << flag << middle;

        this->m_property->Show(middle, flag);

    }

    painter.setPen(this->penna);

    painter.drawRect(QRectF(pointinit.point, pointfine.point));
}

/*
 * la funzione viene richiamata quando dobbiamo
 * spostare un po' di oggetti nella lista m_id
 * in questo caso si analizza quando c'è un id
 *  uguale, e si sposta tutto il tratto
*/

void square::findObjectToDraw()
{
    //uint counterStroke, counterPage, lenPoint, lenStroke, counterPoint;
    datastruct *data = canvas->data->datatouch;
    //const uint lenPage = data->lengthPage();

    QRectF sizeData;

    if (this->m_id.length() == 0)
        goto img;

    sizeData = data->get_size_area(m_id);

    // find the first point

    this->pointinit.point = sizeData.topLeft();
    this->pointfine.point = sizeData.bottomRight();

    /*for(counterPage = 0; counterPage < lenPage; counterPage ++){
        lenStroke = data->at(counterPage).lengthStroke();

        for(counterStroke = 0; counterStroke < lenStroke; counterStroke ++){
            const int id = data->at(counterPage).atStroke(counterStroke).getId();
            const point_s &__point = data->at_draw(0, counterPage, counterStroke);

            if(m_id.indexOf(id) != -1){
                pointinit.point.setX(__point.m_x);
                pointinit.point.setY(__point.m_y);

                pointfine.point.setX(__point.m_x);
                pointfine.point.setY(__point.m_y);

                break;
            }
        }
    }

    for(counterPage = 0; counterPage < lenPage; counterPage ++){
        lenPoint = data->at(counterPage).lengthStroke();
        for(; counterStroke < lenPoint; counterStroke++){

            const uint lenPoint = data->at(counterPage).atStroke(counterStroke).length();
            const int id = data->at(counterPage).atStroke(counterStroke).getId();
            for(counterPoint = 0; counterPoint < lenPoint; counterPoint ++){
                const point_s &__point = data->at_draw(counterPoint, counterPage, counterStroke);

                if(this->m_id.indexOf(id) != -1){
                    if(__point.m_x < pointinit.point.x())
                        pointinit.point.setX(__point.m_x);

                    if(__point.m_x > pointfine.point.x())
                        pointfine.point.setX(__point.m_x);

                    if(__point.m_y < pointinit.point.y())
                            pointinit.point.setY(__point.m_y);
                    if(__point.m_y > pointfine.point.y())
                        pointfine.point.setY(__point.m_y);
                }
            }

        }
    }*/

    img:
    for(int counterImg = 0; counterImg < m_index_img.length(); counterImg ++){
        const int index = this->m_index_img.at(counterImg);
        const auto &ref = canvas->data->m_img->m_img.at(index);

        if(ref.i.x() < pointinit.point.x())
            pointinit.point.setX(ref.i.x());
        else if(ref.f.x() > pointfine.point.x())
            pointfine.point.setX(ref.f.x());

        if(ref.i.y() < pointinit.point.y())
            pointinit.point.setY(ref.i.y());
        else if(ref.f.y() > pointfine.point.y())
            pointfine.point.setY(ref.f.y());
    }
}

/*
 * the function is call when check is set to true
*/
void square::move(const QPointF &punto){
    QPointF __point;
    Document *data = canvas->data;
    QList<int> PageModify;

    if(!in_box){
        return this->reset();
    }

    if(!lastpoint.set){
        lastpoint.point = punto;
        lastpoint.set = true;
        return;
    }

    if(!datastruct::isinside(pointinit.point, pointfine.point, punto)){
        return this->reset();
    }

    __point = lastpoint.point - punto;

    datastruct::inverso(__point);

    data->datatouch->MovePoint(m_id, __point, &PageModify);
    data->m_img->moveImage(m_index_img, __point);

    lastpoint.point = punto;

    pointinit.point = pointinit.point + __point;
    pointfine.point = pointfine.point + __point;

    __need_reload = true;
    data->datatouch->triggerNewView(PageModify, -1, true);
}

void square::actionProperty(property_control::ActionProperty action)
{
    int flags = 0, dontcall_copy = 0;
    QList<int> page;
    datastruct &data = *canvas->data->datatouch;

    switch (action) {
        case property_control::ActionProperty::__copy:{
            flags = SELECTION_FLAGS_COPY;
            break;
        }
        case property_control::ActionProperty::__cut:{
            flags = SELECTION_FLAGS_CUT;
            break;
        }
        case property_control::ActionProperty::__paste:{
            flags = SELECTION_FLAGS_PASTE;
            break;
        }
        case property_control::ActionProperty::__delete:{
            data.removePointId(m_id, &page);
            dontcall_copy = 1;
            break;
        }
       default:
           std::abort();
    }

    if(!dontcall_copy)
        this->m_copy->selection(data, this->m_id, flags, page);
    else
        this->reset();

    data.triggerNewView(page, -1, true);

    if(!page.isEmpty())
        canvas->call_update();
}

/*
 * la funcione cambia i punti, in caso l'utente non abbia tracciato il
 * rettangolo da in alto a sinistra a in alto a destra
*/
void square::adjustPoint()
{
    double tmp;
    QPointF &topLeft = pointinit.point;
    QPointF &bottomRight = pointfine.point;
    if(!pointinit.set || !pointinit.set)
        return;

    if(topLeft.x() > bottomRight.x()){
        tmp = bottomRight.x();
        bottomRight.setX(topLeft.x());
        topLeft.setX(tmp);
    }

    if(topLeft.y() > bottomRight.y()){
        tmp = bottomRight.y();
        bottomRight.setY(topLeft.y());
        topLeft.setY(tmp);
    }
}
