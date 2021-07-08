#ifndef AUDIOPLAY_PLAY_BOTTON_FUNCTION_CPP
#define AUDIOPLAY_PLAY_BOTTON_FUNCTION_CPP

#include "../mainwindow.h"
#include "ui_mainwindow.h"
#include "cambioiconariascolto.h"

#include <QIcon>

void cambioiconariascolto(MainWindow *parent){
    QIcon icon;
    if(parent->player->state() == QMediaPlayer::PlayingState || parent->player->mediaStatus() == QMediaPlayer::EndOfMedia)
        icon.addFile(QString::fromUtf8(":image/images/play.png"), QSize(), QIcon::Normal, QIcon::Off);
    else
        icon.addFile(QString::fromUtf8(":image/images/pause-recording.png"), QSize(), QIcon::Normal, QIcon::Off);

    parent->ui->start_play->setIcon(icon);
}

/* funzione play in basso a sinistra */
void MainWindow::on_start_play_clicked()
{
    cambioiconariascolto(this);
    if(this->player->state() == QMediaPlayer::PlayingState)
        this->player->pause();
    else
        this->player->play();

}

#endif //AUDIOPLAY_PLAY_BOTTON_FUNCTION_CPP
