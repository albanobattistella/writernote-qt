#include "ui_highlighter.h"
#include "ui_ui_highlighter.h"

#include <QSettings>
#include "../../utils/setting_define.h"
#include "../pen/pen_ui.h"
#include "../datastruct/datastruct.h"

#include <QDebug>

ui_highlighter::ui_highlighter(QWidget *parent, bool *same, pen_ui *pen) :
    QDialog(parent),
    ui(new Ui::ui_highlighter)
{
    ui->setupUi(this);

    same_data = same;
    m_pen = pen;

    ui->button_pressure->setCheckable(true);
    ui->button_size->setCheckable(true);

    ui->slider_alfa->setMinimum(0);
    ui->slider_alfa->setMaximum(127);

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    ui->slider_size->setMinimum(1);
    ui->slider_size->setMaximum(100);

    loadSettings();
}

ui_highlighter::~ui_highlighter()
{
    saveSettings();

    delete ui;
}

void ui_highlighter::loadSettings()
{
    QSettings setting(ORGANIZATIONAME, APPLICATION_NAME);
    setting.beginGroup(GROUPNAME_HIGHLIGHTER);

    m_data.alfa = setting.value(KEY_HIGHLIGHTER_ALFA, 50).toInt();
    m_data.size = setting.value(KEY_HIGHLIGHTER_SIZE, 100).toInt();
    m_data.pressure = setting.value(KEY_HIGHLIGHTER_SPESS, true).toBool();
    m_data.tratto_sotto = setting.value(KEY_HIGHLIGHTER_SOTTO, true).toBool();

    setting.endGroup();
    updateList();
}

void ui_highlighter::saveSettings()
{
    QSettings setting(ORGANIZATIONAME, APPLICATION_NAME);
    setting.beginGroup(GROUPNAME_HIGHLIGHTER);

    setting.setValue(KEY_HIGHLIGHTER_ALFA, m_data.alfa);
    setting.setValue(KEY_HIGHLIGHTER_SIZE, m_data.size);
    setting.setValue(KEY_HIGHLIGHTER_SPESS, m_data.pressure);
    setting.setValue(KEY_HIGHLIGHTER_SOTTO, m_data.tratto_sotto);

    setting.endGroup();
}

bool ui_highlighter::event(QEvent *event)
{
    if(event->type() == QEvent::WindowDeactivate)
        this->hide();

    return QWidget::event(event);
}

void ui_highlighter::updateList()
{
    bool pressure;

    if(!same_data)
        return;

    ui->same_data->setChecked(*same_data);

    ui->slider_alfa->setValue(m_data.alfa);

    if(*same_data){
        ui->slider_size->setValue(m_pen->get_size_private());
        pressure = m_pen->IsPressure();
    }else{
        pressure = m_data.pressure;
        ui->slider_size->setValue(m_data.size);
    }

    ui->button_pressure->setChecked(pressure);
    ui->button_size->setChecked(!pressure);
    ui->slider_size->setEnabled(!pressure);
}


void ui_highlighter::on_slider_actionTriggered(int action)
{
    m_data.alfa = action;
}

double ui_highlighter::getSize(const double pressure){
    if(*same_data){
        return m_pen->getSize(pressure)*ADD;
    }
    return (m_data.pressure) ? pressure*ADD : m_data.size*ADD;
}

void ui_highlighter::moveAll(datastruct *data)
{
    uint i, len;
    QList<point_s > * __p = data->get_list();

    len = __data.length();

    for(i=1; i<=len; ++i){
        __p->move(__data.at(i-1), i);
    }

    __data.clear();
}

void ui_highlighter::on_button_size_clicked()
{
    if(*same_data){
        m_pen->setType(true);
    }else{
        m_data.pressure = 0;
    }

    m_pen->list_update();
    updateList();
}


void ui_highlighter::on_button_pressure_clicked()
{
    if(*same_data){
        m_pen->setType(false);
    }else{
        m_data.pressure = 1;
    }

    m_pen->list_update();
    updateList();
}


void ui_highlighter::on_slider_size_actionTriggered(int action)
{
    if(*same_data){
        m_pen->setWidthTratto(action);
    }else{
        m_data.pressure = action;
    }

}


void ui_highlighter::on_same_data_stateChanged(int arg1)
{
    qDebug() << arg1 << "ui_highlighter::on_same_data_stateChanged";
    *same_data = arg1;

    updateList();
    m_pen->list_update();
}

void ui_highlighter::on_checkbox_up_stateChanged(int arg1)
{
    m_data.tratto_sotto = arg1;

    updateList();
}

