#ifndef UI_RESTORE_H
#define UI_RESTORE_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include "../../datawrite/savefile.h"
#include "../../dataread/xmlstruct.h"
#include "../../indice_class.h"
#include "../../currenttitle/document.h"

namespace Ui {
class ui_restore;
}

class ui_restore : public QDialog
{
    Q_OBJECT

public:
    explicit ui_restore(QWidget *parent = nullptr,
                        QString path = "");
    ~ui_restore();

private slots:
    void on_close_button_clicked();

    void on_pushButton_open_clicked();

    void on_ok_restore_clicked();

private:
    static void removeNotWriternote(QStringList &l);
    void updateList();

    savefile *m_save = nullptr;
    xmlstruct *m_xml = nullptr;
    indice_class *m_ind;
    Document *m_curr;

    QStringList __l;

    QString path;

    Ui::ui_restore *ui;
};

#endif // UI_RESTORE_H
