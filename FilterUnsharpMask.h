#ifndef FILTERUNSHARPMASK_H
#define FILTERUNSHARPMASK_H

#include <QDialog>

class _UnsharpMask;
class Picture;

namespace Ui {
class FilterUnsharpMask;
}

class FilterUnsharpMask : public QDialog
{
    Q_OBJECT

public:
    explicit FilterUnsharpMask(Picture *pic, QWidget *parent = 0);
    ~FilterUnsharpMask();

private slots:
    void on_horizontalSlider_radius_valueChanged(int value);

    void on_horizontalSlider_threshold_valueChanged(int value);

    void on_horizontalSlider_amount_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(_UnsharpMask *m_sharp);

private:
    Ui::FilterUnsharpMask *ui;

    Picture *m_pic;

    _UnsharpMask *m_sharp;
};

#endif // FILTERUNSHARPMASK_H
