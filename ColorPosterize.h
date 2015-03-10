#ifndef COLORPOSTERIZE_H
#define COLORPOSTERIZE_H

#include <QDialog>

class Picture;

namespace Ui {
class ColorPosterize;
}

class ColorPosterize : public QDialog
{
    Q_OBJECT

public:
    explicit ColorPosterize(Picture *pic, QWidget *parent = 0);
    ~ColorPosterize();

private slots:

    void on_pushButton_reset_clicked();

    void proc_accept();

private:
    Ui::ColorPosterize *ui;

    Picture *m_pic;
};

#endif // COLORPOSTERIZE_H
