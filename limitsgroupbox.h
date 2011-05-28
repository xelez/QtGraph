#ifndef LIMITSGROUPBOX_H
#define LIMITSGROUPBOX_H

#include <QGroupBox>

namespace Ui {
    class LimitsGroupBox;
}

class LimitsGroupBox : public QGroupBox {
    Q_OBJECT
public:
    LimitsGroupBox(QWidget *parent = 0);
    ~LimitsGroupBox();
    bool isValid();
    double limitFrom();
    double limitTo();

public slots:
    void valuesChanged(double val);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::LimitsGroupBox *ui;
    QPalette paletteError, paletteNormal;
    bool validLimits;

private slots:
    void on_pbSymmetry_toggled(bool checked);
};

#endif // LIMITSGROUPBOX_H
