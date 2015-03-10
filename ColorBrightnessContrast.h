#ifndef COLORBRIGHTNESSCONTRAST_H
#define COLORBRIGHTNESSCONTRAST_H

#include <QDialog>

class Picture;

namespace Ui {
class ColorBrightnessContrast;
}

class ColorBrightnessContrast : public QDialog
{
    Q_OBJECT

public:
    explicit ColorBrightnessContrast(Picture *pic, QWidget *parent = 0);
    ~ColorBrightnessContrast();

private slots:
    void on_horizontalSlider_brightness_valueChanged(int value);

    void on_horizontalSlider_contrast_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();
signals:
    void state_changed(int levelB,int levelC);

private:
    Ui::ColorBrightnessContrast *ui;

    Picture *m_pic;
};

#endif // COLORBRIGHTNESSCONTRAST_H
