#include "../tabletcanvas.h"
#include <QDebug>

/* automenta lo zoom */
void TabletCanvas::zoomin(datastruct *datastruct_){
    /* if it is called by paintevent */
    if(!datastruct_){
        datastruct_ = this->data;

        this->isloading = true;
    }
    if(datastruct_->zoom >= 2.0)
        return;

    if(m_pixmap.width() < width())
        this->initPixmap();
    else{
        int i, len = datastruct_->x.length();

        for(i=0; i < len; i ++){
            datastruct_->x[i] *= 1.05;
            datastruct_->x[i] -= 1;

            datastruct_->y[i] *= 1.05;
            datastruct_->y[i] -= 1;
        }
    }

    data->zoom += 0.05;

}
