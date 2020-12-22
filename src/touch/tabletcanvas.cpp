#include "tabletcanvas.h"

#include <QCoreApplication>
#include <QPainter>
#include <QtMath>
#include <cstdlib>

#include <QDebug>

#include <QTabletEvent>
#include <QEvent>

#include "datastruct/datastruct.h"

TabletCanvas::TabletCanvas()
    : QWidget(nullptr), m_brush(m_color)
    , m_pen(m_brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{

    resize(500, 500);
    setAutoFillBackground(true);
    setAttribute(Qt::WA_TabletTracking);

    this->data = nullptr;
}

void TabletCanvas::clear()
{
    m_pixmap.fill(Qt::white);
    update();
}

void TabletCanvas::updateBrush(const QTabletEvent *event){
    int hue, saturation, value, alpha;
    this->m_color.getHsv(&hue, &saturation, &value, &alpha);

    int vValue = int(((event->yTilt() + 60.0) / 120.0) * 255);
    int hValue = int(((event->xTilt() + 60.0) / 120.0) * 255);

    switch (m_alphaChannelValuator) {
            case PressureValuator:
                m_color.setAlphaF(event->pressure());
                break;
            case TangentialPressureValuator:
                if (event->device() == QTabletEvent::Airbrush)
                    m_color.setAlphaF(qMax(0.01, (event->tangentialPressure() + 1.0) / 2.0));
                else
                    m_color.setAlpha(255);
                break;
            case TiltValuator:
                m_color.setAlpha(std::max(std::abs(vValue - 127),
                                          std::abs(hValue - 127)));
                break;
            default:
                m_color.setAlpha(255);
        }
    switch (m_colorSaturationValuator) {
            case VTiltValuator:
                m_color.setHsv(hue, vValue, value, alpha);
                break;
            case HTiltValuator:
                m_color.setHsv(hue, hValue, value, alpha);
                break;
            case PressureValuator:
                m_color.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
                break;
            default:
                ;
        }

    switch (m_lineWidthValuator) {
            case PressureValuator:
                m_pen.setWidthF(pressureToWidth(event->pressure()));
                break;
            case TiltValuator:
                m_pen.setWidthF(std::max(std::abs(vValue - 127),
                                         std::abs(hValue - 127)) / 12);
                break;
            default:
                m_pen.setWidthF(1);
        }
    if (event->pointerType() == QTabletEvent::Eraser) {
            m_brush.setColor(Qt::white);
            m_pen.setColor(Qt::white);
            m_pen.setWidthF(event->pressure() * 10 + 1);
        } else {
            m_brush.setColor(m_color);
            m_pen.setColor(m_color);
        }
}

void TabletCanvas::updateCursor(const QTabletEvent *event)
{
    QCursor cursor;
    if (event->type() != QEvent::TabletLeaveProximity) {
        if (event->pointerType() == QTabletEvent::Eraser) {
            cursor = QCursor(QPixmap(":image/images/cursor-eraser.png"), 3, 28);
        } else {
            switch (event->device()) {
            case QTabletEvent::Stylus:
                cursor = QCursor(QPixmap(":image/images/cursor-pencil.png"), 0, 0);
                break;
            case QTabletEvent::Airbrush:
                cursor = QCursor(QPixmap(":image/images/cursor-airbrush.png"), 3, 4);
                break;
            case QTabletEvent::RotationStylus: {
                QImage origImg(QLatin1String(":image/images/cursor-felt-marker.png"));
                QImage img(32, 32, QImage::Format_ARGB32);
                QColor solid = m_color;
                solid.setAlpha(255);
                img.fill(solid);
                QPainter painter(&img);
                QTransform transform = painter.transform();
                transform.translate(16, 16);
                transform.rotate(event->rotation());
                painter.setTransform(transform);
                painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                painter.drawImage(-24, -24, origImg);
                painter.setCompositionMode(QPainter::CompositionMode_HardLight);
                painter.drawImage(-24, -24, origImg);
                painter.end();
                cursor = QCursor(QPixmap::fromImage(img), 16, 16);
            } break;
            default:
                break;
            }
        }
    }
    setCursor(cursor);
}

/* funzione che viene richiamata tutte le volte che si muove qualcosa sulla tabella */
void TabletCanvas::tabletEvent(QTabletEvent *event){
    switch (event->type()) {
        case QEvent::TabletPress:
            if (!m_deviceDown) {
                updatelist(event);

                m_deviceDown = true;
                lastPoint.pos = event->pos();
                lastPoint.pressure = event->pressure();
                lastPoint.rotation = event->rotation();
            }
            break;
        case QEvent::TabletMove:
        if (event->device() == QTabletEvent::RotationStylus)
                        updateCursor(event);
            /* richiamata quando la penna scorre toccando lo schermo */
            if (m_deviceDown) {
                updateBrush(event);
                QPainter painter(&m_pixmap);
                paintPixmap(painter, event);
                lastPoint.pos = event->pos();
                lastPoint.pressure = event->pressure();
                lastPoint.rotation = event->rotation();

                updatelist(event);
            }
            break;
        case QEvent::TabletRelease:
            if (m_deviceDown && event->buttons() == Qt::NoButton)
                m_deviceDown = false;
            update();
            break;
        default:
            break;
    }
    event->accept();
}


qreal TabletCanvas::pressureToWidth(qreal pressure)
{
    return pressure * 10 + 1;
}


void TabletCanvas::resizeEvent(QResizeEvent *)
{
    initPixmap(0);
    this->isloading = true;
}
