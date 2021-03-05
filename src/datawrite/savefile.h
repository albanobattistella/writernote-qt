#ifndef SAVEFILE_H
#define SAVEFILE_H

#include "../currenttitle/currenttitle_class.h"
#include "../indice_class.h"

#include <zip.h>
#include "../utils/common_error_definition.h"

#define WRITE_ON_SIZE(x,y,z,filezip) if(zip_source_write(x,y,z)==-1)return freezip(x,filezip);

class savefile
{
private:
    QString *path;
    currenttitle_class *currenttitle;

    int salvabinario(zip_t *);

    int savefile_check_2(zip_source_t *file, currenttitle_class *currenttitle, zip_t *filezip);

public:

    void setting_data(currenttitle_class *m_current){currenttitle = m_current;}

    savefile(QString *path, currenttitle_class *currenttitle);
    int savefile_check_indice(indice_class *);
    int savefile_check_file();

};

#endif // SAVEFILE_H
