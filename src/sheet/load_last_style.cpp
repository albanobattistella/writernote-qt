#include "load_last_style.h"

#include <QSettings>
#include <QtCore/QDataStream>
#include <QtCore/QSettings>

/*static QDataStream& operator<<(QDataStream& out, const style_struct& v) {
    out << v.nx << v.ny << v.colore;
    return out;
}

static QDataStream& operator>>(QDataStream& in, style_struct& v) {
    in >> v.nx;
    in >> v.ny;

    int i;
    for(i=0; i<4; i++)
        in >> v.colore[i];

    return in;
}*/

style_struct * load_last_style(){
    style_struct default_setting;

    default_setting.colore[1] = default_setting.colore[0] = 0;
    default_setting.colore[2] = 255;
    default_setting.colore[3] = 1;

    default_setting.nx = default_setting.ny = 20;

    style_struct *style_temp = new style_struct;

    QSettings setting("writernote", "style");
    setting.beginGroup("style");

    QVariant value = setting.value("style_form", QVariant::fromValue(default_setting));

    *style_temp = value.value<style_struct>();

    setting.endGroup();

    return style_temp;
}

int save_last_style(style_struct *style_v){
    QSettings setting("writernote", "style");
    setting.beginGroup("style");

    setting.setValue("style_form", QVariant::fromValue(*style_v));

    setting.endGroup();

    return 1;
}


