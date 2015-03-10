#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>

namespace Ui {
class StatusBar;
}

class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = 0);

    ~StatusBar();

    void resetStatusBar(bool isLoaded);

public slots:
    void setPosition(QString pos);

    void setScale(int value);

    void setImageSize(QString size);

private slots:
    void on_pushButton_adjust_clicked();

    void on_pushButton_zoom_out_clicked();

    void on_pushButton_zoom_in_clicked();

    void on_horizontalSlider_scale_valueChanged(int value);

signals:
    void sendScale(double scale);

private:
    Ui::StatusBar *ui;
};


#include <QStatusBar>

class RichStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit RichStatusBar(QWidget *parent = 0);

    ~RichStatusBar();

    StatusBar *widget() const;
signals:

public slots:

private:
    StatusBar *statusBar;
};

#endif // STATUSBAR_H
