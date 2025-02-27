#include "touch/tabletcanvas.h"
#include "touch/event/itspossibletoscroll.h"
#include "time.h"
#include <chrono>
#include <ctime>
#include <QDebug>
#include <QTimer>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

static long last_time, __last_time;
static QTimer * timer = nullptr;
static double speed_x, speed_y;
static long delta_time;

static int how_time = 0;
extern bool block_scrolling;

static inline long current_time(){
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

#define controll_speed(speed, x, comparison) \
    speed = (speed comparison x) ? x : speed;

#define def_comp 1000.0

#define cont_scoll(speed, valore) \
    (std::abs(speed) >= valore)

#define def_val 1.0

static inline void dec_speed(double &var, double scrolling_speed);

void TabletCanvas::scrollKinetic(QPointF first, QPointF second)
{
    int delta_x, delta_y;

    if(!timer){
        timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, [=]{
            double __delta;
            _ismoving.set = 0;
            _ismoving.point = QPointF(0, 0);

            if(likely(how_time)){
                delta_time /= this->m_scrolling_speed;
                timer->start(10);

                dec_speed(speed_x, m_scrolling_speed);
                dec_speed(speed_y, m_scrolling_speed);

                __delta = (double) speed_x;
                if(cont_scoll(__delta, def_val)
                        && scroll::x(data->datatouch, _pixmap.width(), __delta)){
                    _ismoving.set = 1;
                }

                _ismoving.point.setX(__delta);

                __delta = (double) speed_y;
                if(cont_scoll(__delta, def_val)
                        && scroll::y(data->datatouch, _pixmap.height(), __delta)){
                    _ismoving.set = 1;
                }

                _ismoving.point.setY(__delta);

                if(unlikely(!_ismoving.set)){
                    //qDebug() << "timer stop";
                    timer->stop();
                    return;
                }

                _ismoving.point.setY(__delta);

                this->ismoving_f();


            }else{
                timer->stop();
            }

            -- how_time;

        });
    }

    timer->stop();

    delta_time = last_time - __last_time;

    ++ delta_time;

    delta_x = first.x() - second.x();
    delta_y = first.y() - second.y();

    speed_x = delta_x / delta_time;
    speed_y = delta_y / delta_time;

    speed_x *= -25.0;
    speed_y *= -25.0;

    how_time = m_scrolling_speed * 100.0;

    timer->start(1);
}

void TabletCanvas::updateTimeScroll()
{
    if(likely(timer))
        timer->stop();

    __last_time = last_time;
    last_time = current_time();
}

static inline void dec_speed(double &var, double scrolling_speed)
{
    uchar cont = 1;
    scrolling_speed /= 5.0;
    if(var > (double)scrolling_speed){
        var -= (double)scrolling_speed;
        cont = 0;
    }
    else{
        if(var < - (double)scrolling_speed){
            var += (double)scrolling_speed;
            cont = 0;
        }
    }

    if(cont){
        var = 0.0;
    }
}
