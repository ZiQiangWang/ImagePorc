#ifndef FILTERSURFACEBLUR_H
#define FILTERSURFACEBLUR_H

#include <QDialog>

class Picture;

namespace Ui {
class FilterSurfaceBlur;
}

class FilterSurfaceBlur : public QDialog
{
    Q_OBJECT

public:
    explicit FilterSurfaceBlur(Picture *pic, QWidget *parent = 0);
    ~FilterSurfaceBlur();

private slots:

    void on_horizontalSlider_radius_valueChanged(int value);

    void on_horizontalSlider_threshold_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(int radius,int threshold);

private:
    Ui::FilterSurfaceBlur *ui;

    int m_radius;

    int m_threshold;

    Picture *m_pic;
};

#endif // FILTERSURFACEBLUR_H
