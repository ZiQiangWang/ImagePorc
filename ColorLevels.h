#ifndef COLORLEVELS_H
#define COLORLEVELS_H

#include <QDialog>
//namespace cv {
//    class Mat;
//}
#include <opencv2/core/core.hpp>

class _Levels;
class Picture;
class PixmapItem;
class Histogram;

namespace Ui {
class ColorLevels;
}

class ColorLevels : public QDialog
{
    Q_OBJECT

public:
    explicit ColorLevels(Picture *pic, PixmapItem *item, QWidget *parent = 0);
    ~ColorLevels();

public slots:

    void proc_accept();

signals:

    void state_changed(_Levels *levels);

private slots:

    void onColorPicker(int *rgb);

    void set_min_intput_high();

    void set_max_intput_low();

    void on_horizontalSlider_low_input_valueChanged(int value);

    void on_horizontalSlider_gamma_valueChanged(int value);

    void on_doubleSpinBox_gamma_valueChanged(const QString &value);

    void on_horizontalSlider_high_input_valueChanged(int value);

    void on_horizontalSlider_low_output_valueChanged(int value);

    void on_horizontalSlider_high_output_valueChanged(int value);

    void on_comboBox_channels_currentIndexChanged(int index);

    void on_pushButton_reset_channel_clicked();

    void on_pushButton_reset_clicked();

    void on_pushButton_black_clicked(bool checked);

    void on_pushButton_white_clicked(bool checked);

    void on_pushButton_black_2_clicked(bool checked);

    void on_pushButton_gray_clicked(bool checked);

    void on_pushButton_white_2_clicked(bool checked);

    void on_pushButton_auto_clicked();

    void onClose();

private:
    void resetButton();

private:
    Ui::ColorLevels *ui;

    _Levels *levels;
    int m_channel;
    Picture *m_pic;
    PixmapItem *m_item;
    Histogram *m_hist;
    cv::Mat histImage[4];
};

#endif // COLORLEVELS_H
