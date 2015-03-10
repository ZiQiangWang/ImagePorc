#ifndef COLORCURVE_H
#define COLORCURVE_H

#include <QDialog>
#include <opencv2/core/core.hpp>

class Picture;
class Histogram;
class _Curve;

namespace Ui {
class ColorCurve;
}

class ColorCurve : public QDialog
{
    Q_OBJECT

public:
    explicit ColorCurve(Picture *pic,QWidget *parent = 0);
    ~ColorCurve();

private slots:

    void show_position(QPoint pos);

    void on_comboBox_currentIndexChanged(int index);

    void onValueChanged(QVector<double> value);

    void on_pushButton_reset_channel_clicked();

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(_Curve *curve);

private:
    Ui::ColorCurve *ui;

    Picture *m_pic;
    Histogram *hist;
    cv::Mat histImage[4];
    _Curve *m_curve;
    int m_channel;
};

#endif // COLORCURVE_H
