#include "object_finder.h"
#include "mainwindow.h"
#include "currenttitle/document.h"
#include "pthread.h"

// è lo stesso stroke definito nel file tabletevent.cpp
extern stroke __tmp;

object_finder::object_finder(QObject *parent)
    : QObject{parent}
{
    W_ASSERT(parent);
    W_ASSERT(parent->objectName() == "MainWindow");

    _canvas = ((MainWindow *)parent)->m_canvas;

    _timer = new QTimer(this);
    QObject::connect(_timer, &QTimer::timeout, this, &object_finder::endTimer);
}

object_finder::~object_finder()
{

}

void object_finder::endTimer()
{
    stroke &stroke = __tmp;
    int i;
    const QRectF &rect = stroke.getBiggerPointInStroke();


}
