#include "mainwindow.h"
#include "qfilechoose.h"
#include "ui_mainwindow.h"
#include "savefile.h"
#include "utils/dialog_critic/dialog_critic.h"

static inline const QString move_file(const QString &nomevecchio, const QString &nomenuovo){
    return ("cp " + nomevecchio + " " + nomenuovo);
}

static bool c = true;

/* funzione che viene richiamata quando viene salvato */
void MainWindow::on_actionSave_File_triggered()
{
    savefile savefile_i(&m_path, _canvas->data);
    QString new_path;
    bool check;

    if(m_path == ""){
        if(!qfilechoose::getFileForSave(new_path, TYPEFILEWRITER)){
            return;
        }
        m_path = new_path;
    }

    check = savefile_i.savefile_check_file(true) == OK;

    if(!check && c)
        return dialog_critic("We had a problem while saving the file");

    c = true;
}

void MainWindow::on_actionSave_As_triggered()
{
    QString posizione = this->m_path;

    if(!qfilechoose::getFileForSave(m_path))
        /* se l'utente non ha selezionato nessun file */
       return;

    if(posizione != ""){
        int command;

        command = system(move_file(posizione, m_path).toUtf8().constData());

        if(command){
            dialog_critic("We had a problem saving a copy of " + posizione + " to " + m_path);
            m_path = posizione;
            return;
        }

        c = false;

        /*save the file in the new path -> keeping the current file the same*/
        this->on_actionSave_File_triggered();
        if(c){
            dialog_critic("We had a problem saving the copybook in " + m_path);
        }
        m_path = posizione;
    }
    else
        return this->on_actionSave_File_triggered();

}
