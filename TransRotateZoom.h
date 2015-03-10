#ifndef TRANSROTATEZOOM_H
#define TRANSROTATEZOOM_H

#include <QDialog>

class _RotateZoom;
class Picture;

namespace Ui {
class TransRotateZoom;
}

class TransRotateZoom : public QDialog
{
    Q_OBJECT

public:
    explicit TransRotateZoom(Picture *pic, QWidget *parent = 0);

    ~TransRotateZoom();

    void Transform();

public slots:

    void proc_accept();

signals:
    void state_changed(_RotateZoom *rotate);

private slots:

    void on_horizontalSlider_rotate_valueChanged(int value);

    void on_horizontalSlider_per_rotate_valueChanged(int value);

    void on_horizontalSlider_perspective_valueChanged(int value);

    void on_horizontalSlider_offset_x_valueChanged(int value);

    void on_doubleSpinBox_offset_x_valueChanged(const QString &value);

    void on_horizontalSlider_offset_y_valueChanged(int value);

    void on_doubleSpinBox_offset_y_valueChanged(const QString &value);

    void on_horizontalSlider_zoom_valueChanged(int value);

    void on_doubleSpinBox_zoom_valueChanged(const QString &value);

    void on_pushButton_reset_clicked();

    void on_pushButton_reset_rotate_clicked();

    void on_pushButton_reset_offset_clicked();

    void on_pushButton_reset_zoom_clicked();

private:
    Ui::TransRotateZoom *ui;

    _RotateZoom *rotate;

    Picture *m_pic;
};

#endif // TRANSROTATEZOOM_H
