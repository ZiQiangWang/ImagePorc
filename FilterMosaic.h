#ifndef FILTERMOSAIC_H
#define FILTERMOSAIC_H

#include <QDialog>

class Picture;
namespace Ui {
class FilterMosaic;
}

class FilterMosaic : public QDialog
{
    Q_OBJECT

public:
    explicit FilterMosaic(Picture *pic, QWidget *parent = 0);
    ~FilterMosaic();

private slots:
    void on_horizontalSlider_x_valueChanged(int value);

    void on_horizontalSlider_y_valueChanged(int value);

    void on_checkBox_toggled(bool checked);

    void on_pushButton_reset_clicked();

    void proc_accept();

signals:
    void state_changed(int x,int y);

private:
    Ui::FilterMosaic *ui;

    Picture *m_pic;

    int m_x,m_y;
};

#endif // FILTERMOSAIC_H
