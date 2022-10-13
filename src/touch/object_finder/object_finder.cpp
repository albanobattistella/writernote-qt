#include "object_finder.h"
#include "mainwindow.h"
#include "touch/object_finder/model_finder/model_finder.h"
#include "touch/dataTouch/stroke/StrokePre.h"

// è lo stesso stroke definito nel file tabletevent.cpp
extern StrokePre __tmp;

object_finder::object_finder(QObject *parent)
    : QObject{parent}
{
    W_ASSERT(parent);
    W_ASSERT(parent->objectName() == "TabletCanvas");

    _canvas = (TabletCanvas *)parent;

    _timer = new QTimer(this);
    QObject::connect(_timer, &QTimer::timeout, this, &object_finder::endTimer);
    _timer->setSingleShot(true);
}


void object_finder::endTimer()
{
    auto &stroke = __tmp;

    if (likely(stroke.is_normal())){
        WDebug(debug, "call");

        model::find(stroke);

        _canvas->call_update();
    }
}
