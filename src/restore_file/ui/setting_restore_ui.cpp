#include "setting_restore_ui.h"
#include "ui_setting_restore_ui.h"
#include "utils/setting_define.h"
#include "datawrite/savefile.h"
#include "utils/dialog_critic/dialog_critic.h"
#include "utils/path/get_path.h"
#include "restore_file/get_name_tmp.h"
#include "utils/time/current_time.h"
#include "utils/common_def.h"
#include "touch/tabletcanvas.h"
#include "core/core.h"
#include "mainwindow.h"

#include <QSettings>
#include <QTimer>
#include <QFile>

#define MAX_SAVE 3600
#define MIN_SAVE 1

extern bool need_save_auto;
extern bool need_save_tmp;

setting_restore_ui::setting_restore_ui(QWidget *parent,
                                       TabletCanvas *canvas,
                                       QByteArray *pp) :
    QDialog(parent),
    m_data(true,
           true,
           5,
           5,
           true),
    ui(new Ui::setting_restore_ui)
{
    ui->setupUi(this);

    m_first = new QTimer(this);
    m_sec = new QTimer(this);

    QObject::connect(m_first, &QTimer::timeout, this, &setting_restore_ui::firstTimer, Qt::ConnectionType::QueuedConnection);
    QObject::connect(m_sec, &QTimer::timeout, this, &setting_restore_ui::secondTimer, Qt::ConnectionType::QueuedConnection);

    ui->spinBox_autosave->setMaximum(MAX_SAVE);
    ui->spinBox_autosave->setMinimum(MIN_SAVE);

    ui->spinBox_temp->setMaximum(MAX_SAVE);
    ui->spinBox_temp->setMinimum(MIN_SAVE);

    ui->checkBox_autosave->setCheckable(true);
    ui->checkBox_temp->setCheckable(true);

    this->_canvas = canvas;
    this->m_path = pp;

    loadData();

    startTimerSetting();
    
}

void setting_restore_ui::startTimerSetting()
{
    m_first->stop();
    m_sec->stop();

    if(m_data.autosave){
        m_first->start(m_data.t_autosave*1000);
    }

#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    if(m_data.temp_file){
        m_sec->start(m_data.t_temp_file*1000);
    }
#endif
}

setting_restore_ui::~setting_restore_ui()
{
    saveData();
    delete ui;
}

void setting_restore_ui::decidePath()
{

}

void setting_restore_ui::deleteFile()
{
    const QString ff = (*m_path != "") ? get_name_tmp::get(*m_path) : tmp_path;

    if(!QFile::exists(ff)){
        return;
    }

    if(!QFile::remove(ff)){
        user_message(QApplication::tr("I had a problem removing the temp file in %1").arg(ff));
    }
}

void setting_restore_ui::updateWindow()
{
    ui->checkBox_autosave->setChecked(m_data.autosave);
    ui->spinBox_autosave->setValue(m_data.t_autosave);
#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    ui->checkBox_temp->setChecked(m_data.temp_file);
    ui->spinBox_temp->setValue(m_data.t_temp_file);
#else
    ui->checkBox_temp->setHidden(true);
    ui->checkBox_temp->setHidden(true);
    ui->spinBox_temp->setHidden(true);
#endif

    ui->checkBox_remove->setChecked(m_data.remove_file);
}

void setting_restore_ui::loadData()
{
    QSettings setting(ORGANIZATIONAME, APPLICATION_NAME);
    setting.beginGroup(GROUPNAME_INT);

#if (defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    m_data.t_autosave = setting.value(KEY_INT_AUTOSAVE_TIME, 5).toUInt();
    m_data.remove_file = setting.value(KEY_INT_REMOVE_FILE_ENABLE, true).toBool();
#else
    m_data = __data(
        setting.value(KEY_INT_AUTOSAVE_ENABLE, false).toBool(),
        setting.value(KEY_INT_TMP_ENABLE, true).toBool(),
        setting.value(KEY_INT_TMP_TIME, 5).toUInt(),

        setting.value(KEY_INT_AUTOSAVE_TIME, 5).toUInt(),
        setting.value(KEY_INT_REMOVE_FILE_ENABLE, true).toBool()
    );
#endif

    setting.endGroup();

    this->updateWindow();
}

void setting_restore_ui::saveData()
{
    QSettings setting(ORGANIZATIONAME, APPLICATION_NAME);
    setting.beginGroup(GROUPNAME_INT);

    setting.setValue(KEY_INT_AUTOSAVE_TIME, m_data.t_autosave);

#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    setting.setValue(KEY_INT_AUTOSAVE_ENABLE, m_data.autosave);

    setting.setValue(KEY_INT_TMP_ENABLE, m_data.temp_file);
    setting.setValue(KEY_INT_TMP_TIME, m_data.t_temp_file);
#endif

    setting.setValue(KEY_INT_REMOVE_FILE_ENABLE, m_data.remove_file);

    setting.endGroup();
}

// first timer -> autosave
void setting_restore_ui::firstTimer()
{
    savefile ff(m_path, _canvas->getDoc());
    int res;

    if(!need_save_auto){
        goto start_timer;
    }

    res = ff.savefile_check_file(false) == OK;

    if(!res){
        dialog_critic(QApplication::tr("We had a problem saving the file in %1").arg(*m_path));
    }

    start_timer:
    need_save_auto = false;
    startTimerSetting();

}

QString setting_restore_ui::adjustString(const QString &str)
{
    auto res = str;
    res.replace(":", "").replace(" ", "");
    return res;
}

static int try_save = 0;

// second timer -> tmp file
void setting_restore_ui::secondTimer()
{
    int res;
    QByteArray path;
    savefile ff(&path, _canvas->getDoc());

    /**
     * We don't want to save the file
     * while the user is writing
     * */
    if(core::get_canvas()->isWriting())
        goto restart;

    if(!need_save_tmp)
        goto start_timer;

    if(!m_path->isEmpty()){
        path = get_name_tmp::get(*m_path);
    }else{
        if(tmp_path.isEmpty()){
            const auto day = adjustString(current_day_string());
            const auto time = adjustString(current_time_string());
            path = get_path(path::tmp_file_not_save);

            tmp_path = qstr("%1/.writernote_unsave_%2%3" APP_EXT).arg(path, day, time);
        }
    }

    if(!tmp_path.isEmpty())
        path = tmp_path.toUtf8();

    //qDebug() << "Save tmp file in: " << path;

    res = ff.savefile_check_file(false) == OK;

    if(!res){
        if(try_save > 5){
            const auto m = qstr("We had a problem saving the temporary file in %1 for %2 times").arg(path).arg(try_save);
            dialog_critic(m);
        }
        try_save ++;
    }else{
        try_save = 0;
    }

start_timer:
    need_save_tmp = false;

restart:
    startTimerSetting();
}

void setting_restore_ui::on_pushButton_ok_clicked()
{
    emit dataChange();
    this->close();
}

void setting_restore_ui::on_pushButton_close_clicked()
{
    this->close();
}


#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
void setting_restore_ui::on_spinBox_temp_valueChanged(int arg1)
{
    m_data.t_temp_file = arg1;
    if(m_data.t_temp_file > m_data.t_autosave){
        m_data.t_autosave = m_data.t_temp_file;
    }

    updateWindow();
}
#endif

void setting_restore_ui::on_spinBox_autosave_valueChanged(int arg1)
{
    m_data.t_autosave = arg1;

#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    if(m_data.t_autosave < m_data.t_temp_file){
        m_data.t_temp_file = m_data.t_autosave;
    }
#endif

    updateWindow();
}


#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
void setting_restore_ui::on_checkBox_autosave_stateChanged(int arg1)
{
    /* arg1 == 0 the user disable */

    m_data.autosave = arg1;
}

void setting_restore_ui::on_checkBox_temp_stateChanged(int arg1)
{
    m_data.temp_file = arg1;
}
#endif

void setting_restore_ui::on_checkBox_remove_stateChanged(int arg1)
{
    m_data.remove_file = arg1;
}

__data::__data(bool autosave, bool temp_file, unsigned int t_temp_file, unsigned int t_autosave, bool remove_file)
    : autosave(autosave)
    , temp_file(temp_file)
    , t_temp_file(t_temp_file)
    , t_autosave(t_autosave)
    , remove_file(remove_file)
{
}
