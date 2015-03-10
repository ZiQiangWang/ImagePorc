#ifndef FILTERMOTIONBLUR_H
#define FILTERMOTIONBLUR_H

#include <QDialog>

class Picture;
class _MotionBlur;

namespace Ui {
class FilterMotionBlur;
}

class FilterMotionBlur : public QDialog
{
    Q_OBJECT

public:
    explicit FilterMotionBlur(Picture *pic, QWidget *parent = 0);
    ~FilterMotionBlur();

public slots:

    void proc_accept();

signals:

    void state_changed(_MotionBlur *blur);

private slots:
    void on_horizontalSlider_angle_valueChanged(int value);

    void on_horizontalSlider_length_valueChanged(int value);

    void on_checkBox_toggled(bool checked);

    void on_pushButton_reset_clicked();


private:
    Ui::FilterMotionBlur *ui;

    Picture *m_pic;

    _MotionBlur *blur;
};

#endif // FILTERMOTIONBLUR_H
