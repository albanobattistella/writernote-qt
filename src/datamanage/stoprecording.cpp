#include "../mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>

#include "../dialog_critic.h"

#include "../audioplay/aggiornotastiriascolto.h"

#include "../setting_ui.h"
#include "spacchettamento.h"

void MainWindow::on_stoprecordingbotton_triggered()
{
    this->m_audioRecorder->stop();

    messaggio_utente("I begin to sort out the data.\nThis operation may take a while");

    spacchettamento(this);

    this->self->currenttitle.se_registato = true;

    messaggio_utente("I finished");

    settingaudio_registrazione(this, false);
    settingaudio_riascolto(this, true);

    aggiornotestiriascolto(this);

    this->ui->statusBar->clearMessage();
    this->m_canvas->time = 0;
}

