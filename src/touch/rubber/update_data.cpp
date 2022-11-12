#include "rubber_ui.h"
#include "ui_rubber_ui.h"

void rubber_ui::update_data()
{
    ui->size_slider->setValue(_size_gomma);

    cbool m_type = (this->_type_gomma == RubberMethod::type_rubber::total);

    ui->totale_button->setChecked(m_type);
    ui->partial_button->setChecked(!m_type);

    ui->size_slider->setEnabled(true);

}
