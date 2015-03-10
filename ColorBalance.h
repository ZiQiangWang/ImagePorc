#ifndef COLORBALANCE_H
#define COLORBALANCE_H

#include <QDialog>
#include "Utils.h"

class _ColorBalance;
class Picture;
namespace Ui {
class ColorBalance;
}

class ColorBalance : public QDialog
{
    Q_OBJECT

public:
    explicit ColorBalance(Picture *pic, QWidget *parent = 0);
    ~ColorBalance();

private slots:
    void on_checkBox_preserve_luminosity_toggled(bool checked);

    void on_radioButton_Shadow_clicked();

    void on_radioButton_Midtone_clicked();

    void on_radioButton_HighLight_clicked();

    void on_horizontalSlider_red_valueChanged(int value);

    void on_horizontalSlider_green_valueChanged(int value);

    void on_horizontalSlider_blue_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();

    void on_pushButton_reset_range_clicked();

signals:
    void state_changed(_ColorBalance *color_balance);
private:
    Ui::ColorBalance *ui;

    Picture *m_pic;
    int m_ToneRgn;
    _ColorBalance *color_balance;

public:
    enum
    {
        COLOR_BALANCE_SHADOWS,
        COLOR_BALANCE_MIDTONES,
        COLOR_BALANCE_HIGHLIGHTS
    };
};


#endif // COLORBALANCE_H
