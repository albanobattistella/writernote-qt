#include "tabletcanvas.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "log/log_ui/log_ui.h"
#include "utils/common_script.h"

static int __page = -2;
static int __max = 0;
void MainWindow::updatePageCount(int pageCount)
{
    QString text;
    cint lenPage = _canvas->data->datatouch->lengthPage();

    ui->page->setHidden(pageCount == -1);

    text = qstr("Page: %1 of %2").arg(pageCount).arg(lenPage);

    if(__page == pageCount && lenPage == __max) return;

    __max = lenPage;
    __page = pageCount;

    this->ui->page->setText(text);
}

void TabletCanvas::updatePageCount()
{
    datastruct *__data = data->datatouch;
    const int lenPage = __data->lengthPage();
    const double deltay = __data->getPointFirstPage().y();

    for(int i = 0; i < lenPage; i++){
        const double currentHeight = __data->at(i).currentHeight();
        if(currentHeight + deltay > 0.0)
            return _parent->updatePageCount(i + 1);
    }

    _parent->updatePageCount(-1);

    NAME_LOG_EXT->write("Missing page", log_ui::possible_bug);
}
