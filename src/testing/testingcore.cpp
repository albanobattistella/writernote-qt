#include "testingcore.h"
#include "mainwindow.h"
#include "touch/tabletcanvas.h"
#include "utils/common_script.h"
#include "datawrite/savefile.h"

#if defined(DEBUG_THREAD) || defined(DEBUGINFO)
TestingCore::TestingCore(MainWindow *__parent)
{
    parent = __parent;
}

int TestingCore::startTesting()
{
    return 0;
}

int TestingCore::createPage(
        Document &doc,
        int page,
        int lenStroke,
        int lenPerStroke,
        bool is_same_pressure,
        const QPointF &point,
        const double deltaPerPoint,
        const double deltaPerStroke,
        int last_id)
{
    int i, k;
    double x, y;
    datastruct *data = doc.datatouch;
    double base_x, base_y;

    last_id ++;

    for(i = 0; i < lenStroke; i++){
        stroke __tmp;

        base_x = double(i) * deltaPerStroke;
        base_y = double(i) * deltaPerStroke;

        x = point.x();
        y = point.y();

        for(k = 0; k < lenPerStroke; k++){
            point_s point;
            point.m_x = x + deltaPerPoint + base_x;
            point.m_y = y + deltaPerPoint + page::getHeight() + base_y;

            x += deltaPerPoint;
            y += deltaPerPoint + double(page * page::getHeight());

            if(is_same_pressure){
                point.pressure = 10;
            }else{
                point.pressure = x;
            }

            __tmp.append(point);
        }

        __tmp.setColor(Qt::black);
        __tmp.setId(last_id ++);

        data->at_mod(page).append(__tmp);
    }
    return last_id;
}

#define T_PAGE_LEN 10
#define T_STROKE_IN_PAGE 1000
#define T_POINT_IN_STROKE 300
void TestingCore::createDocument(Document *doc)
{
    int i, id = 0;
    if(!doc){
        doc = parent->m_canvas->data;
    }

    for(i = 0; i < T_PAGE_LEN; i++){
        page page(i+1, n_style::square);
        doc->datatouch->m_page.append(page);
        id = this->createPage(*doc, i, T_STROKE_IN_PAGE, T_POINT_IN_STROKE, false, QPointF(0, 0), 0.1, 1, id);

        doc->datatouch->m_page.operator[](i).triggerRenderImage(-1, true);
    }

    for(i = 0; i < T_PAGE_LEN; i++){
        qDebug() << doc->datatouch->at(i).lengthStroke();
    }
}

int TestingCore::createAndSave(
        const QString   &pos,
        Document        *doc)
{
    if(!doc)
        doc = parent->m_canvas->data;
    createDocument(doc);

    savefile __tmp(pos, doc);
    return __tmp.savefile_check_file();
}

#endif
