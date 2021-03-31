#include "square.h"
#include <QPen>
#include <QPainter>
#include "../datastruct/datastruct.h"

square::square()
{
    penna.setStyle(Qt::DotLine);
    penna.setWidth(2);
    penna.setColor(QColor::fromRgb(30, 90, 255));
    this->reset();
}

void square::reset(){
    this->pointinit = this->lastpoint = this->pointfine = QPoint(-1, -1);
    check = false;
}

QRectF square::disegno(QPainter &painter, QPointF puntodifine){
    painter.setPen(penna);

    QRectF recttemp;
    recttemp.setTopLeft(pointinit);
    recttemp.setBottomRight(puntodifine);
    painter.drawRect(recttemp);

    /* in questo modo ritorna solamente la porzione di pixmap che deve essere ricaricata */
    QRectF rect = QRectF(pointinit, pointfine);

    pointfine = puntodifine;

    return rect;
}

/* la funzione capisce se all'interno del quadrato della selezione c'è qualcosa
 * in caso salva l'id del tratto e setta la variabile this->check = true, in caso contrario
 * la setta = false e fa il return
*/
bool square::find(datastruct *data){
    unsigned int i, len;

    len = data->m_point.length();

    this->check = false;

    const point_s * __point;

    for(i=0;i<len; i++){
        __point = & data->m_point.at(i);
        if(data->isinside(pointinit, pointfine, __point)){

            if(m_id.indexOf(__point->idtratto))
                m_id.append(__point->idtratto);

            this->check = true;
        }
    }

    return check;
}

/* la funzione prendere l'elemento più in alto a sinistra e più in basso a destra */
QRectF square::drawsquare(QPainter &painter, datastruct *data){
    unsigned int i, len;
    double maxx, maxy, minx, miny;

    maxx = maxy = 0;

    minx = data->m_point.at(data->positionId(idtratto)).m_x;
    miny = data->m_point.at(data->positionId(idtratto)).m_y;

    const point_s * __point;

    len = data->m_point.length();
    for(i=0; i<len; i++){
        __point = & data->m_point.at(i);

        if(__point->idtratto == idtratto){
            if(__point->m_x < minx)
                minx = __point->m_x;
            else if(__point->m_x > maxx)
                maxx = __point->m_x;


            if(__point->m_y < miny)
                miny = __point->m_y;
            else if(__point->m_y > maxy)
                maxy = __point->m_y;
        }
    }

    pointinit.setX(minx);
    pointinit.setY(miny);

    pointfine.setX(maxx);
    pointfine.setY(maxy);

    this->check = true;

    return this->disegno(painter, this->pointfine);

}

/* la funzione resistuisce
 * vero se è intero il punto è interno
*/
bool square::isinside(QPointF point){
    if(!this->check)
        return false;

    return datastruct::isinside(pointinit, pointfine, point);

}


