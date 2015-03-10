#ifndef COLORHUESATURATION_H
#define COLORHUESATURATION_H

#include <QDialog>

class Picture;
class _HueSaturation;

namespace Ui {
class ColorHueSaturation;
}

class ColorHueSaturation : public QDialog
{
    Q_OBJECT

public:
    explicit ColorHueSaturation(Picture *pic, QWidget *parent = 0);
    ~ColorHueSaturation();

public slots:

    void proc_accept();

signals:

    void state_changed(_HueSaturation *hs);

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_horizontalSlider_overlap_valueChanged(int value);

    void on_horizontalSlider_hue_valueChanged(int value);

    void on_horizontalSlider_lightness_valueChanged(int value);

    void on_horizontalSlider_saturation_valueChanged(int value);

    void on_pushButton_reset_color_clicked();

    void on_pushButton_reset_clicked();

private:
    Ui::ColorHueSaturation *ui;

    Picture *m_pic;
    _HueSaturation *hs;
    int hue[7],lightness[7],saturation[7];
    int overlap;
    int color;
};

enum
{
  ALL_HUES,
  RED_HUES,
  YELLOW_HUES,
  GREEN_HUES,
  CYAN_HUES,
  BLUE_HUES,
  MAGENTA_HUES
};

#endif // COLORHUESATURATION_H
