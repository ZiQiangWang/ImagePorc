#include "ColorHueSaturation.h"
#include "ui_ColorHueSaturation.h"
#include "Picture.h"
#include "Commands.h"
#include "Types.h"

ColorHueSaturation::ColorHueSaturation(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorHueSaturation)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;
    hs = new _HueSaturation;

    for(int i=0;i<7;i++)
    {
        hs->hue[i]=0;
        hs->lightness[i]=0;
        hs->saturation[i]=0;
    }
    hs->overlap = 0;
    color = 0;

    connect(this,&ColorHueSaturation::state_changed,pic,&Picture::HueSaturation);

    connect(this,&ColorHueSaturation::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorHueSaturation::accepted,this,&ColorHueSaturation::proc_accept);

    pic->PushBackRecord();
}

ColorHueSaturation::~ColorHueSaturation()
{
    delete ui;
    delete hs;
}

void ColorHueSaturation::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-色调-亮度-饱和度");
    m_pic->getUndoStack()->push(command);
}

void ColorHueSaturation::on_comboBox_currentIndexChanged(int index)
{
    color = index;
    ui->horizontalSlider_hue->setValue(hs->hue[color]);
    ui->horizontalSlider_lightness->setValue(hs->lightness[color]);
    ui->horizontalSlider_saturation->setValue(hs->saturation[color]);
}

void ColorHueSaturation::on_horizontalSlider_overlap_valueChanged(int value)
{
    hs->overlap = value;
    emit state_changed(hs);
}
void ColorHueSaturation::on_horizontalSlider_hue_valueChanged(int value)
{
    hs->hue[color]=value;
    emit state_changed(hs);
}

void ColorHueSaturation::on_horizontalSlider_lightness_valueChanged(int value)
{
    hs->lightness[color]=value;
    emit state_changed(hs);
}

void ColorHueSaturation::on_horizontalSlider_saturation_valueChanged(int value)
{
    hs->saturation[color] = value;
    emit state_changed(hs);
}

void ColorHueSaturation::on_pushButton_reset_color_clicked()
{
    ui->horizontalSlider_hue->setValue(0);
    ui->horizontalSlider_lightness->setValue(0);
    ui->horizontalSlider_saturation->setValue(0);
}

void ColorHueSaturation::on_pushButton_reset_clicked()
{
    for(int i=0;i<7;i++)
    {
        hs->hue[i]=0;
        hs->lightness[i]=0;
        hs->saturation[i]=0;
    }
    hs->overlap = 0;
    ui->horizontalSlider_overlap->setValue(0);
    ui->horizontalSlider_hue->setValue(0);
    ui->horizontalSlider_lightness->setValue(0);
    ui->horizontalSlider_saturation->setValue(0);
    emit state_changed(hs);
}
