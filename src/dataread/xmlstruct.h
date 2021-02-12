#ifndef XMLSTRUCT_H
#define XMLSTRUCT_H

/*Non vengono importati, servono a xmlstruct.h per dare i suggerimenti durante la modifica*/
#include "../mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <cstdlib>

#include <string>
#include <fstream>
#include <streambuf>
#include <QList>
#include <QStringList>
#include "../self_class.h"

#include "../touch/datastruct/datastruct.h"

#include <zip.h>

class xmlstruct{
private:
    void UnZipFile();
    QString *path_;

    indice_class *indice = nullptr;
    currenttitle_class *currenttitle = nullptr;

public:
    std::string pathFile = "";
    std::string text = "";

    int checksum=0, start= 0;

    bool loadindice();
    /*void loadfile(const char *);*/
    bool loadfile(const char *);

    xmlstruct(QString *path_U, indice_class *indice_U, currenttitle_class *currenttitle_U);

    int posizione(const int posizionestringa, const char *find);

    bool exist();
    bool createempty();
    char *readfile(const char *path, const char *namefile);

    void stringa_decode(const char *, const char *, QStringList *);
    void stringa_decode_int(const char *, const char *, QList<int>*);
    void decode_checksum();

    bool loadbinario(struct zip *);

    /* funzione che gestisce la scrittura di testinohtml */
    void textdecode(QList<int> *);

    int findstart();

    bool decode();
};

#endif
