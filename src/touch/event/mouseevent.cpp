#include "../tabletcanvas.h"
#include <QWheelEvent>

#include <QDebug>

void TabletCanvas::wheelEvent(QWheelEvent *event)
{
    short int move = event->delta();

    this->ismoving.delta = move;
    this->ismoving.ismoving_check = true;

    event->accept();
    update();
}
