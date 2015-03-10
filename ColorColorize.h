#ifndef COLORCOLORIZE_H
#define COLORCOLORIZE_H

#include <QDialog>

class Picture;

namespace Ui {
class ColorColorize;
}

class ColorColorize : public QDialog
{
    Q_OBJECT

public:
    explicit ColorColorize(Picture *pic, QWidget *parent = 0);
    ~ColorColorize();

private slots:

    void on_horizontalSlider_H_valueChanged(int value);

    void on_horizontalSlider_S_valueChanged(int value);

    void on_horizontalSlider_L_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();


signals:
    void state_changed(int H,int S,int L);
private:
    Ui::ColorColorize *ui;

    Picture *m_pic;

    int m_h,m_s,m_l;
};

#endif // COLORCOLORIZE_H
