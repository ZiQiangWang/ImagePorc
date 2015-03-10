#ifndef FILTERREDEYEREMOVAL_H
#define FILTERREDEYEREMOVAL_H

#include <QDialog>

class Picture;

namespace Ui {
class FilterRedEyeRemoval;
}

class FilterRedEyeRemoval : public QDialog
{
    Q_OBJECT

public:
    explicit FilterRedEyeRemoval(Picture *pic, QWidget *parent = 0);
    ~FilterRedEyeRemoval();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(int threshold);

private:
    Ui::FilterRedEyeRemoval *ui;

    Picture *m_pic;

    int m_threshold;
};

#endif // FILTERREDEYEREMOVAL_H
