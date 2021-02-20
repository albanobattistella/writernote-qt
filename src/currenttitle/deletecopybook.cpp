#include "deletecopybook.h"

#include "../mainwindow.h"
#include "ui_mainwindow.h"
#include "../utils/areyousure/areyousure.h"
#include <QMessageBox>
#include <QString>

#include "../update_list_copybook.h"

#include <QMediaPlayer>
#include "redolist.h"

/* funzione che gestisce l'eliminizione del copybook */
void f_deletecopybook( MainWindow *parent, const char *copybook){
    if(!areyousure(parent, "Delete copybook", "are you sure you want to delete " + (QString)copybook + "?"))
        return redolist(parent);

    if(parent->player->state() != QMediaPlayer::StoppedState)
        parent->player->stop();

    int posizione = parent->m_indice.titolo.indexOf(copybook);


    parent->m_indice.titolo.removeAt(posizione);

    parent->m_currenttitle.reset();

    parent->ui->textEdit->setEnabled(false);
    parent->ui->actionDelete_audio->setEnabled(false);
    parent->ui->actionDelete_copybook->setEnabled(false);

    parent->ui->stopriascolto->setEnabled(false);
    parent->ui->startriascoltobotton->setEnabled(false);

    parent->ui->startrecording->setEnabled(false);
    parent->ui->startriascoltobotton->setEnabled(false);

    parent->ui->textEdit->setHtml("");

}

void MainWindow::on_actionDelete_copybook_triggered()
{
    if (m_currentTitle.isEmpty())
        return;

    f_deletecopybook(this, m_currentTitle.toUtf8().constData());
    this->m_currentTitle = "";
    update_list_copybook(this);

}
