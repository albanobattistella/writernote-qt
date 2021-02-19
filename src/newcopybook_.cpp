#include "newcopybook_.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datawrite/savefile.h"

#include "currenttitle/currenttitle_class.h"
#include "utils/posizione_binario.h"

/*funzione che gestisce la creazione di un nuovo copybook, non fa il controllo se esiste*/
bool newcopybook_(MainWindow *parent, QString& stringa){
    parent->self->indice.titolo.append(stringa);

    currenttitle_class tempcopybook;
    tempcopybook.reset();

    tempcopybook.nome_copybook = stringa;


    savefile save_(&parent->self->path, &tempcopybook);
    if(!save_.savefile_check_file() || !save_.savefile_check_indice(&parent->self->indice)){
        return false;
    }

    return true;

}

