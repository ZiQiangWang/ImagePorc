#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <QDialog>
#include <opencv2/core/core.hpp>

class Picture;

namespace Ui {
class Perspective;
}

class Perspective : public QDialog
{
    Q_OBJECT

public:
    explicit Perspective(Picture *pic,QWidget *parent = 0);
    ~Perspective();

private slots:
    void on_pushButton_reset_clicked();

    void slider_changed(int value);

    void proc_accept();

signals:
    void state_changed(const cv::Mat &t);
private:
    Ui::Perspective *ui;
    int m_width;
    int m_height;
    cv::Point2f src[4]; //源图像中的点
    cv::Point2f dst[4]; //目标图像中的点
    cv::Mat t;  //变换矩阵

    Picture *m_pic;
};

#endif // PERSPECTIVE_H
