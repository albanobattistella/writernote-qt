#include "../mainwindow.h"
#include "ui_mainwindow.h"
#include "../utils/dialog_critic/dialog_critic.h"

/* funzione che gestiste il riascolto dell'audio
 * viene richiamata quando l'audio viene riprodotto
*/
void MainWindow::riascoltoaudioprogressivo(qint64 position){
    if(this->player->state() != QMediaPlayer::PlayingState)
        return;

    ui->statusBar->showMessage(tr("%1 second").arg(position/1000));

    this->m_canvas->riascolto(position/1000);

    this->ui->audioSlider->blockSignals(true);

    int duration = player->duration();
    if(!duration)
        return user_message("The audio seems blank");

    this->ui->audioSlider->setValue((position*100)/duration);

    this->ui->audioSlider->blockSignals(false);

}
