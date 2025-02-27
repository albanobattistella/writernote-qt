#ifndef SETTING_RESTORE_UI_H
#define SETTING_RESTORE_UI_H

#include <QDialog>
#include <QString>

struct __data{
    /* true enable */
#if defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE)
    const bool autosave = true;
#else
    bool autosave : 1;
    bool temp_file : 1;
    uint t_temp_file;
#endif
    uint t_autosave;

    bool remove_file : 1; /* false do not remove the file after writernote close */

};

namespace Ui {
class setting_restore_ui;
}

class Document;

class setting_restore_ui : public QDialog
{
    Q_OBJECT

public:
    explicit setting_restore_ui(QWidget *parent,
                                class TabletCanvas *curr,
                                QString *pp);
    ~setting_restore_ui();

    struct __data getData () const
    {
        return m_data;
    }

    void setData(const struct __data *data){
        memcpy(&m_data, data, sizeof(struct __data));
        updateWindow();
    }

/* if we are in android or ios we don't want to save a tmp file, because we already set autosave to true */
#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    /* call this function when change copybook or close writernote */
    void changeCopybookFile(){
        if(m_data.remove_file
                && m_data.temp_file
                && (*m_path != ""
                || tmp_path != "")){
            deleteFile();
            tmp_path = "";
        }
    }
#endif

    QString get_curr_name_tmp() const{
        return tmp_path;
    }

private:

    /* when the user has not selected a location to save the file, it is automatically saved as a temporary file in the /home/user/.writernote folder */
    void decidePath();

    void deleteFile();

    QString tmp_path;
    QString *m_path;

    void startTimerSetting();

    QTimer *m_first;
    QTimer *m_sec;

    class TabletCanvas *_canvas;

    void updateWindow();

    struct __data m_data;

    void loadData();
    void saveData();

    Ui::setting_restore_ui *ui;

signals:
    /* emit when user change somethings */
    void dataChange();

private slots:
    /* autosave */
    void firstTimer();

    /* tmp file */
    void secondTimer();

    void on_pushButton_ok_clicked();
    void on_pushButton_close_clicked();

#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    void on_spinBox_temp_valueChanged(int arg1);
#endif
    void on_spinBox_autosave_valueChanged(int arg1);

#if !(defined(ANDROID_WRITERNOTE) || defined(IOS_WRITERNOTE))
    void on_checkBox_autosave_stateChanged(int arg1);
    void on_checkBox_temp_stateChanged(int arg1);
#endif
    void on_checkBox_remove_stateChanged(int arg1);
};

#endif // SETTING_RESTORE_UI_H
