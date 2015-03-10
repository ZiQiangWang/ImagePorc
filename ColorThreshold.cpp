#include "ColorThreshold.h"
#include "ui_ColorThreshold.h"
#include "Commands.h"
#include "Layer.h"
#include "Picture.h"
#include <QSlider>

ColorThreshold::ColorThreshold(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorThreshold)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    m_min = 127;
    m_max = 255;

    connect(ui->horizontalSlider_low,&QSlider::valueChanged,this,&ColorThreshold::set_slider_max);
    connect(ui->horizontalSlider_up,&QSlider::valueChanged,this,&ColorThreshold::set_slider_min);

    connect(this,&ColorThreshold::state_changed,pic,&Picture::Threshold);
    connect(this,&ColorThreshold::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorThreshold::accepted,this,&ColorThreshold::proc_accept);
    pic->PushBackRecord();
    pic->Threshold(m_min,m_max);
}

ColorThreshold::~ColorThreshold()
{
    delete ui;
}

void ColorThreshold::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-阈值");
    m_pic->getUndoStack()->push(command);
}

void ColorThreshold::on_horizontalSlider_low_valueChanged(int value)
{
    m_min = value;
    emit state_changed(m_min,m_max);
}

void ColorThreshold::on_horizontalSlider_up_valueChanged(int value)
{
    m_max = value;
    emit state_changed(m_min,m_max);
}

void ColorThreshold::set_slider_min()
{
    if(ui->horizontalSlider_up->value()<=m_min)
        ui->horizontalSlider_up->setValue(m_min);
}

void ColorThreshold::set_slider_max()
{
    if(ui->horizontalSlider_low->value()>=m_max)
        ui->horizontalSlider_low->setValue(m_max);
}

void ColorThreshold::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_low->setValue(127);
    ui->horizontalSlider_up->setValue(255);
}


