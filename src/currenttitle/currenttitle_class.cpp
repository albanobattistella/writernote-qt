#include "currenttitle_class.h"
#include <QString>
#include <QStringList>

#include <QDebug>

#include "../touch/datastruct/datastruct.h"

currenttitle_class::currenttitle_class(){
    qDebug() << "Inizializzo datastruct";
    this->datatouch = new datastruct;
}

void currenttitle_class::reset(){
    this->versione=0;
    this->se_registato = this->se_tradotto = false;
    this->audio_position_path = "";
    this->testi.clear();
    this->testinohtml.clear();
    this-> posizione_iniz.clear();
}
