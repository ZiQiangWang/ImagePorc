#include "ColorBrightnessContrast.h"
#include "ui_ColorBrightnessContrast.h"
#include "Picture.h"
#include "Commands.h"

ColorBrightnessContrast::ColorBrightnessContrast(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorBrightnessContrast)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    connect(this,&ColorBrightnessContrast::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorBrightnessContrast::accepted,this,&ColorBrightnessContrast::proc_accept);
    connect(this,&ColorBrightnessContrast::state_changed,pic,&Picture::BrightnessContrast);
    pic->PushBackRecord();
}

ColorBrightnessContrast::~ColorBrightnessContrast()
{
    delete ui;
}

void ColorBrightnessContrast::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-亮度-对比度");
    m_pic->getUndoStack()->push(command);
}

void ColorBrightnessContrast::on_horizontalSlider_brightness_valueChanged(int value)
{
    emit state_changed(value,ui->horizontalSlider_contrast->value());
}

void ColorBrightnessContrast::on_horizontalSlider_contrast_valueChanged(int value)
{
    emit state_changed(ui->horizontalSlider_brightness->value(),value);
}

void ColorBrightnessContrast::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_brightness->setValue(0);
    ui->horizontalSlider_contrast->setValue(0);
}

