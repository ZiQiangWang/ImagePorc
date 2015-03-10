#ifndef FILTERRADIALBLUR_H
#define FILTERRADIALBLUR_H

#include <QDialog>

class Picture;
class _RadialBlur;

namespace Ui {
class FilterRadialBlur;
}

class FilterRadialBlur : public QDialog
{
    Q_OBJECT

public:
    explicit FilterRadialBlur(Picture *pic, QWidget *parent = 0);
    ~FilterRadialBlur();

private slots:
    void on_horizontalSlider_angle_valueChanged(int value);

    void on_horizontalSlider_center_x_valueChanged(int value);

    void on_doubleSpinBox_center_x_valueChanged(const QString &value);

    void on_horizontalSlider_center_y_valueChanged(int value);

    void on_doubleSpinBox_center_y_valueChanged(const QString &value);

    void on_pushButton_reset_center_clicked();

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(_RadialBlur *blur);

private:
    Ui::FilterRadialBlur *ui;

    _RadialBlur *blur;

    Picture *m_pic;
};

#endif // FILTERRADIALBLUR_H
