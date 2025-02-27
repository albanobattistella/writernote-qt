#include "laser.h"
#include "touch/tabletcanvas.h"

laser::laser(QObject *parent):
    QObject(parent)
{
    WNew(_timer, QTimer, ());
    QObject::connect(_timer, &QTimer::timeout, this, &laser::endTimer);
    _canvas = (TabletCanvas *)parent;

    W_ASSERT(_canvas->objectName() == "TabletCanvas");

}

laser::~laser()
{
    WDelete(_timer);
}
