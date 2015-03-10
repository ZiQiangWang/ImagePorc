#ifndef FILTERZOOMBLUR_H
#define FILTERZOOMBLUR_H

#include <QDialog>

class _ZoomBlur;
class Picture;

namespace Ui {
class FilterZoomBlur;
}

class FilterZoomBlur : public QDialog
{
    Q_OBJECT

public:
    explicit FilterZoomBlur(Picture *pic, QWidget *parent = 0);
    ~FilterZoomBlur();

private slots:

    void on_horizontalSlider_focus_valueChanged(int value);

    void on_horizontalSlider_center_x_valueChanged(int value);

    void on_doubleSpinBox_center_x_valueChanged(const QString &value);

    void on_horizontalSlider_center_y_valueChanged(int value);

    void on_doubleSpinBox_center_y_valueChanged(const QString &value);

    void on_pushButton_reset_center_clicked();

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(_ZoomBlur *blur);

private:
    Ui::FilterZoomBlur *ui;

    _ZoomBlur *blur;

    Picture *m_pic;
};

#endif // FILTERZOOMBLUR_H
