#include "limitsgroupbox.h"
#include "ui_limitsgroupbox.h"
#include <cmath>

LimitsGroupBox::LimitsGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::LimitsGroupBox)
{
    ui->setupUi(this);
    paletteNormal = palette();
    paletteError = paletteNormal;
    paletteError.setColor(QPalette::Text, Qt::red);
    paletteError.setColor(QPalette::HighlightedText, Qt::red);

    validLimits = true;
    on_pbSymmetry_toggled(true);
}

LimitsGroupBox::~LimitsGroupBox()
{
    delete ui;
}

void LimitsGroupBox::changeEvent(QEvent *e)
{
    QGroupBox::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LimitsGroupBox::valuesChanged(double val)
{
    if (ui->pbSymmetry->isChecked()) {
        val = abs(val);
        ui->sbFrom->setValue(-val);
        ui->sbTo->setValue(+val);
    }

    bool tmp = ui->sbFrom->value() < ui->sbTo->value();
    if (tmp!=validLimits)
        setPalette( (tmp) ? paletteNormal : paletteError );

    validLimits = tmp;
}

void LimitsGroupBox::on_pbSymmetry_toggled(bool checked)
{
    if (checked) {
        ui->sbFrom->setMaximum(0.0);
        ui->sbTo->setMinimum(0.0);
        double delta = limitTo() - limitFrom();
        ui->sbFrom->setValue(-fabs(delta/2));
    } else {
        ui->sbFrom->setMaximum(1000.0);
        ui->sbTo->setMinimum(-1000.0);
    }

}

bool LimitsGroupBox::isValid() {
    return validLimits;
}

double LimitsGroupBox::limitFrom() {
    return ui->sbFrom->value();
}

double LimitsGroupBox::limitTo() {
    return ui->sbTo->value();
}
