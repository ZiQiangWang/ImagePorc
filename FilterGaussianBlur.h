#ifndef FILTERGAUSSIANBLUR_H
#define FILTERGAUSSIANBLUR_H

#include <QDialog>

class Picture;

namespace Ui {
class FilterGaussianBlur;
}

class FilterGaussianBlur : public QDialog
{
    Q_OBJECT

public:
    explicit FilterGaussianBlur(Picture *pic, QWidget *parent = 0);
    ~FilterGaussianBlur();

private slots:
    void on_checkBox_toggled(bool checked);

    void on_horizontalSlider_x_valueChanged(int value);

    void on_horizontalSlider_y_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(int x,int y);

private:
    Ui::FilterGaussianBlur *ui;

    Picture *m_pic;

    int m_x,m_y;
};

#endif // FILTERGAUSSIANBLUR_H
