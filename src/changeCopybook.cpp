#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dataread/xmlstruct.h"
#include "datawrite/savefile.h"
#include "currenttitle/redolist.h"
#include "savecopybook.h"
#include "utils/dialog_critic/dialog_critic.h"
#include "utils/areyousure/areyousure.h"
#include "setting_ui.h"
#include "restore_file/ui/setting_restore_ui.h"
#include "audioplay/aggiornotastiriascolto.h"

void MainWindow::openCopybook(QListWidgetItem *item, const bool checkCurrent){
    /* in case the user has not yet saved the file */
    if(this->m_path == ""){
        return;
    }

    if(this->player->state() == QMediaPlayer::PlayingState)
        return redolist(this);

    if(this->m_audioRecorder->state() == QAudioRecorder::RecordingState
       || this->m_audioRecorder->state() == QAudioRecorder::PausedState)
        return redolist(this);

    n_need_save _res;
    Document tmp;
    xmlstruct fileload(&m_path, nullptr, &tmp);

    savecopybook __save(this, &m_currentTitle);
    int res;

    const QString new_title = item->text();

    if(m_currentTitle != "" && checkCurrent){
        _res = needToSave(&fileload, &tmp, nullptr);

        if(_res == n_need_save::unable_load){
            return dialog_critic("We had a problem opening the current copybook");
        }

        if(_res == n_need_save::need_save){
            if (!__save.check_permission())
                return redolist(this);

        }
        m_setting->changeCopybookFile();
    }

    if(m_indice.titolo.at(
                m_indice.titolo.indexOf(new_title))
            != ""){
        fileload.setData(&m_path, &m_indice, m_currenttitle);
        res = fileload.loadfile(new_title + ".xml", true, true);

        if(res == ERROR){
            return dialog_critic("We had a problem opening the new copybook");
        }else if(res == ERROR_VERSION){
            return dialog_critic("The version you created this file with is too old");
        }else if(res == ERROR_CONTROLL){
            if(!areyousure("The file is corrupted", "Do you want to open the file anyway?"))
                return;
        }
    }
    else{
        m_currenttitle->reset();
        m_currenttitle->datatouch->reset();
    }
    m_currentTitle = new_title;
    updateTitle(m_currenttitle);

    if(!m_currenttitle->m_touch)
        ui->textEdit->setHtml(m_currenttitle->testi);

    contrUi();

    aggiornotestiriascolto(this);
    contrUi();
    ui->actionPrint->setEnabled(true);

    /* pass the pointer to the class */
    if(m_currenttitle->m_touch){
        this->m_canvas->settingdata(m_currenttitle, m_path);
        this->m_canvas->loadpixel();
        this->m_canvas->time = 0;
    }
}

void MainWindow::on_listWidgetSX_itemDoubleClicked(QListWidgetItem *item)
{
    this->openCopybook(item, true);
}
