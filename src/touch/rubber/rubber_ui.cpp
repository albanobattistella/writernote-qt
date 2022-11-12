#include "rubber_ui.h"
#include "ui_rubber_ui.h"
#include "utils/WCommonScript.h"
#include "touch/multi_thread_data.h"
#include "mainwindow.h"
#include "ui/uicore.h"
#include <QSettings>
#include "utils/setting_define.h"

constexpr auto not_used rubber_debug = false;

rubber_ui::rubber_ui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rubber_ui)
{
    ui->setupUi(this);

    this->load_settings();

    UiCore::makePop(this);

    ui->totale_button->setCheckable(true);
    ui->partial_button->setCheckable(true);

    this->reset();
}

rubber_ui::~rubber_ui()
{
    this->save_settings();

    delete ui;
}

bool rubber_ui::event(QEvent *event)
{
    if(unlikely(event->type() == QEvent::WindowDeactivate))
        this->hide();

    return QWidget::event(event);
}

void rubber_ui::on_totale_button_clicked()
{
    this->_type_gomma = RubberMethod::type_rubber::total;

    this->update_data();
}

void rubber_ui::on_partial_button_clicked()
{
    _type_gomma = RubberMethod::type_rubber::partial;

    this->update_data();
}

/**
 * return -1 if no page is mod
 * return -2 if multiple pages have changed
 * return the index of the page mod
*/
int rubber_ui::endRubber()
{
    return this->_executer.endRubber(this->_type_gomma);
}

/**
 * this function is call by tabletEvent
 * it returns true if it actually deleted something, otherwise it returns false
*/
void rubber_ui::actionRubber(const QPointF &lastPoint)
{
    return this->_executer.actionRubber(lastPoint, this->_type_gomma, this->_size_gomma);
}

void rubber_ui::initRubber(const QPointF &point)
{
    return this->_executer.initRubber(point);
}

void rubber_ui::save_settings() const
{
    QSettings setting(ORGANIZATIONAME, APPLICATION_NAME);
    setting.beginGroup(GROUPNAME_RUBBER);

    setting.setValue(KEY_RUBBER_SIZE, _size_gomma);
    setting.setValue(KEY_RUBBER_TYPE, _type_gomma);

    setting.endGroup();

}

void rubber_ui::load_settings()
{
    QSettings setting(ORGANIZATIONAME, APPLICATION_NAME);
    setting.beginGroup(GROUPNAME_RUBBER);

    _size_gomma = setting.value(KEY_RUBBER_SIZE, DEFAULT_GOMMA_SIZE).toInt();
    _type_gomma = static_cast<RubberMethod::type_rubber>(
            setting.value(KEY_RUBBER_TYPE, RubberMethod::type_rubber::total).toInt()
    );

    ui->totale_button->setCheckable(true);
    ui->partial_button->setCheckable(true);

    setting.endGroup();

    this->update_data();
}
