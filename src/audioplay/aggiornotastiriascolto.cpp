#include "aggiornotastiriascolto.h"

#include "../mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QString>

#include "../utils/dialog_critic/dialog_critic.h"
#include "../setting_ui.h"

#include "../dataread/xmlstruct.h"

void aggiornotestiriascolto(MainWindow *parent){
    if(parent->m_currenttitle.se_registato != audio_record::not_record){

        if(parent->m_currenttitle.se_registato == audio_record::record_file){
            QFile file(parent->m_currenttitle.audio_position_path);
            if(!file.exists())
                return dialog_critic("Audio " + parent->m_currenttitle.audio_position_path + " didn't exist");
        }

        settingaudio_registrazione(parent, false);

        /* enable play, pause botton for play audio alreay record */
        parent->ui->stopriascolto->setEnabled(true);
        parent->ui->startriascoltobotton->setEnabled(true);

        /* volume and audio slider */
        parent->ui->audioSlider->setEnabled(true);
        parent->ui->volumeSlider->setEnabled(true);

        /* abilita il bottone della mano e lo segna unchecked, in caso sia cliccato per qualche motivo*/
        parent->ui->actionListen_current_audio->setEnabled(true);

        if(parent->m_currenttitle.se_registato == audio_record::record_file){
            if(load_audio(&parent->m_currenttitle.audio_data, parent->m_currenttitle.nome_copybook, parent->m_path) != OK)
                return dialog_critic("We had a problem loading the audio");

            parent->player->setMedia(QMediaContent(QUrl::fromAce(parent->m_currenttitle.audio_data)));
        }
        else
            parent->player->setMedia(QUrl::fromLocalFile(parent->m_currenttitle.audio_position_path));

        parent->ui->actionDelete_audio->setEnabled(true);
    }
    else{
        /* abilita i tasti per la registrazione */
        settingaudio_registrazione(parent, true);
        settingaudio_riascolto(parent, false);

        parent->ui->actionDelete_audio->setEnabled(false);

        //parent->ui->actionListen_current_audio->setChecked(false);
        parent->ui->actionListen_current_audio->setEnabled(false);
    }

    if(parent->m_currenttitle.se_tradotto){

    }
}
