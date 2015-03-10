#include "StatusBar.h"
#include "ui_StatusBar.h"
#include <math.h>
StatusBar::StatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusBar)
{
    ui->setupUi(this);
}

StatusBar::~StatusBar()
{
    delete ui;
}

void StatusBar::resetStatusBar(bool isLoaded)
{
    ui->horizontalSlider_scale->setValue(200);
    if(isLoaded)
    {
        ui->horizontalSlider_scale->setEnabled(true);
        ui->pushButton_adjust->setEnabled(true);
        ui->pushButton_zoom_in->setEnabled(true);
        ui->pushButton_zoom_out->setEnabled(true);
        ui->label_size->setVisible(true);
        ui->label_position->clear();
        ui->label_position->setVisible(true);
        ui->label_scale->setVisible(true);
    }
    else
    {
        ui->horizontalSlider_scale->setEnabled(false);
        ui->pushButton_adjust->setEnabled(false);
        ui->pushButton_zoom_in->setEnabled(false);
        ui->pushButton_zoom_out->setEnabled(false);
        ui->label_size->setVisible(false);
        ui->label_position->clear();
        ui->label_position->setVisible(false);
        ui->label_scale->setVisible(false);
    }
}

void StatusBar::setPosition(QString pos)
{
    ui->label_position->setText(pos);
}

void StatusBar::setScale(int value)
{
    int scale = ui->horizontalSlider_scale->value();
    ui->horizontalSlider_scale->setValue(scale+value);
}

void StatusBar::setImageSize(QString size)
{
    ui->label_size->setText(size);
}

void StatusBar::on_pushButton_adjust_clicked()
{
    ui->horizontalSlider_scale->setValue(200);
}

void StatusBar::on_pushButton_zoom_out_clicked()
{
    int value = ui->horizontalSlider_scale->value();
    if(value<=100)
        value = 100;
    else
        value--;
    ui->horizontalSlider_scale->setValue(value);
}

void StatusBar::on_pushButton_zoom_in_clicked()
{
    int value = ui->horizontalSlider_scale->value();
    if(value>=300)
        value = 300;
    else
        value++;
    ui->horizontalSlider_scale->setValue(value);
}

void StatusBar::on_horizontalSlider_scale_valueChanged(int value)
{
    int scale = (int)pow(10,double(value)/100);
    ui->label_scale->setText(QString("%1%").arg(scale));

    emit sendScale((double)scale/100);
}

RichStatusBar::RichStatusBar(QWidget *parent) :
    QStatusBar(parent)
{
    statusBar = new StatusBar();
    this->addWidget(statusBar,1);
}

RichStatusBar::~RichStatusBar()
{
    delete statusBar;
}

StatusBar *RichStatusBar::widget() const
{
    return statusBar;
}


