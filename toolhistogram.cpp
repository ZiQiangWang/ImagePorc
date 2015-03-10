#include "ToolHistogram.h"
#include "ui_ToolHistogram.h"
#include "Layer.h"
#include "Utils.h"
#include "Effect.h"
#include <QDebug>
ToolHistogram::ToolHistogram(Layer *layer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolHistogram)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->move(20,100);

    hist = new Histogram(layer->getCurrentImage(),histImage);

    ui->label->setPixmap(Mat2Pixmap(histImage[0]));

    hist->calculate(0,255);
    ui->label_mean->setText(QString("%1").arg(hist->getMean()));
    ui->label_dev->setText(QString("%1").arg(hist->getDev()));
    ui->label_median->setText(QString("%1").arg(hist->getMedian()));
    ui->label_total->setText(QString("%1").arg(hist->getTotal()));
    ui->label_count->setText(QString("%1").arg(hist->getCount()));
    ui->label_percent->setText(QString("%1").arg(hist->getPercent()));

    connect(ui->horizontalSlider_min,&QSlider::valueChanged,this,&ToolHistogram::set_slider_max);
    connect(ui->horizontalSlider_max,&QSlider::valueChanged,this,&ToolHistogram::set_slider_min);

    connect(ui->horizontalSlider_min,&QSlider::valueChanged,this,&ToolHistogram::calculate);
    connect(ui->horizontalSlider_max,&QSlider::valueChanged,this,&ToolHistogram::calculate);

}

ToolHistogram::~ToolHistogram()
{
    delete ui;
    delete hist;
}

void ToolHistogram::on_comboBox_currentIndexChanged(int index)
{
    calculate();

    // 在直方图画布上画出直方图
    switch (index) {
    case 0:
        ui->label->setPixmap(Mat2Pixmap(histImage[0]));
        break;
    case 1:
        ui->label->setPixmap(Mat2Pixmap(histImage[1]));
        break;
    case 2:
        ui->label->setPixmap(Mat2Pixmap(histImage[2]));
        break;
    case 3:
        ui->label->setPixmap(Mat2Pixmap(histImage[3]));
        break;
    }
}

void ToolHistogram::calculate()
{
    hist->calculate(ui->horizontalSlider_min->value(),ui->horizontalSlider_max->value(),ui->comboBox->currentIndex());
    ui->label_mean->setText(QString("%1").arg(hist->getMean()));
    ui->label_dev->setText(QString("%1").arg(hist->getDev()));
    ui->label_median->setText(QString("%1").arg(hist->getMedian()));
    ui->label_total->setText(QString("%1").arg(hist->getTotal()));
    ui->label_count->setText(QString("%1").arg(hist->getCount()));
    ui->label_percent->setText(QString("%1").arg(hist->getPercent()));
}

void ToolHistogram::set_slider_min()
{
    if(ui->horizontalSlider_max->value()<=ui->horizontalSlider_min->value())
        ui->horizontalSlider_max->setValue(ui->horizontalSlider_min->value());
}

void ToolHistogram::set_slider_max()
{
    if(ui->horizontalSlider_min->value()>=ui->horizontalSlider_max->value())
        ui->horizontalSlider_min->setValue(ui->horizontalSlider_max->value());
}
