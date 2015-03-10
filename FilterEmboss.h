#ifndef FILTEREMBOSS_H
#define FILTEREMBOSS_H

#include <QDialog>

class Picture;
class _Emboss;

namespace Ui {
class FilterEmboss;
}

class FilterEmboss : public QDialog
{
    Q_OBJECT

public:
    explicit FilterEmboss(Picture *pic, QWidget *parent = 0);
    ~FilterEmboss();

private slots:
    void on_horizontalSlider_azimuth_valueChanged(int value);

    void on_horizontalSlider_elevation_valueChanged(int value);

    void on_horizontalSlider_depth_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void on_radioButton_bumpmap_toggled(bool checked);

    void on_radioButton_emboss_toggled(bool checked);

    void proc_accept();

signals:
    void state_changed(_Emboss *emboss);

private:
    Ui::FilterEmboss *ui;

    Picture *m_pic;

    _Emboss *m_emboss;
};

#endif // FILTEREMBOSS_H
