#ifndef COLORTHRESHOLD_H
#define COLORTHRESHOLD_H

#include <QDialog>

class Layer;
class Picture;

namespace Ui {
class ColorThreshold;
}

class ColorThreshold : public QDialog
{
    Q_OBJECT

public:
    explicit ColorThreshold(Picture *pic,QWidget *parent = 0);
    ~ColorThreshold();

private slots:

    void on_horizontalSlider_low_valueChanged(int value);

    void on_horizontalSlider_up_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void set_slider_max();

    void set_slider_min();

    void proc_accept();

signals:

    void state_changed(int value,int max);

private:
    Ui::ColorThreshold *ui;
    Picture *m_pic;
    int m_min,m_max;
};

#endif // COLORTHRESHOLD_H
