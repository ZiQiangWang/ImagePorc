#ifndef TOOLHISTOGRAM_H
#define TOOLHISTOGRAM_H

#include <QDialog>
#include <QDialog>
//namespace cv {
//    class Mat;
//}
#include <opencv2/core/core.hpp>

class Layer;
class Histogram;

namespace Ui {
class ToolHistogram;
}

class ToolHistogram : public QDialog
{
    Q_OBJECT

public:
    explicit ToolHistogram(Layer *layer,QWidget *parent = 0);
    ~ToolHistogram();

private slots:

    void set_slider_max();

    void set_slider_min();

    void on_comboBox_currentIndexChanged(int index);

    void calculate();

private:
    Ui::ToolHistogram *ui;

    Histogram *hist;
    cv::Mat histImage[4];
};

#endif // TOOLHISTOGRAM_H
