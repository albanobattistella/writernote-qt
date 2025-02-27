#ifndef PAINT_H
#define PAINT_H

#include <QPainter>
#include <QImage>
#include <QSize>
#include <QPointF>

#include "touch/datastruct/datastruct.h"
#include "touch/datastruct/page.h"

#define DRAW_SINGLE_LOAD_DEF 1.0
/*
 * the function is useful when you have an
 * image with strokes and you want to
 * draw it on the surface, without having
 * to copy the drawing rules used in
 * TabletCanvas :: load ()
*/
void singleLoad(QPainter        &painter,
        const QImage    &img,
        const QSize     &sizeRect,
        const QPointF   &PointFirstPage,
        cdouble counterPage,
        cdouble         m);

/*
 * @countHeigth is use when we what to
 * create a target rect that to create
 * a targetRect that spans multiple
 * pages, as in square
*/

#define DRAW_CREATE_SIZE_RECT_DEF_COUNTER_HEIGTH 1
#define DRAW_CREATE_SIZE_RECT_DEF_PRO 1.0
inline QSize createSizeRect(
        const datastruct    *data,
        const int           countHeigth,
        const double        pro)
{
    const auto zoom = data->getZoom();
    return QSize(page::getWidth(), page::getHeight() * countHeigth) * zoom * pro;
}

#endif // PAINT_H
