#include "ColorBalance.h"
#include "ui_ColorBalance.h"
#include "Utils.h"
#include "Layer.h"
#include "Picture.h"
#include "Commands.h"
#include "Types.h"
#include <QDebug>

ColorBalance::ColorBalance(Picture *pic,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorBalance)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;
    m_ToneRgn=1;

    color_balance = new _ColorBalance;
    for(int i=0;i<3;i++)
    {
        color_balance->cyan_red[i] = 0;
        color_balance->magenta_green[i] = 0;
        color_balance->yellow_blue[i] = 0;
    }
    color_balance->preserve_luminosity = true;

    connect(this,&ColorBalance::state_changed,pic,&Picture::ColorBalance);

    connect(this,&ColorBalance::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorBalance::accepted,this,&ColorBalance::proc_accept);
    pic->PushBackRecord();
}

ColorBalance::~ColorBalance()
{
    delete ui;
    delete color_balance;
}

void ColorBalance::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-色彩平衡");
    m_pic->getUndoStack()->push(command);
}

void ColorBalance::on_checkBox_preserve_luminosity_toggled(bool checked)
{
    color_balance->preserve_luminosity = checked;
    emit state_changed(color_balance);
}

void ColorBalance::on_radioButton_Shadow_clicked()
{
    m_ToneRgn = COLOR_BALANCE_SHADOWS;

    ui->horizontalSlider_red->setValue(color_balance->cyan_red[m_ToneRgn]);
    ui->horizontalSlider_green->setValue(color_balance->magenta_green[m_ToneRgn]);
    ui->horizontalSlider_blue->setValue(color_balance->yellow_blue[m_ToneRgn]);
    emit state_changed(color_balance);
}

void ColorBalance::on_radioButton_Midtone_clicked()
{
    m_ToneRgn = COLOR_BALANCE_MIDTONES;
    ui->horizontalSlider_red->setValue(color_balance->cyan_red[m_ToneRgn]);
    ui->horizontalSlider_green->setValue(color_balance->magenta_green[m_ToneRgn]);
    ui->horizontalSlider_blue->setValue(color_balance->yellow_blue[m_ToneRgn]);
    emit state_changed(color_balance);
}

void ColorBalance::on_radioButton_HighLight_clicked()
{
    m_ToneRgn = COLOR_BALANCE_HIGHLIGHTS;
    ui->horizontalSlider_red->setValue(color_balance->cyan_red[m_ToneRgn]);
    ui->horizontalSlider_green->setValue(color_balance->magenta_green[m_ToneRgn]);
    ui->horizontalSlider_blue->setValue(color_balance->yellow_blue[m_ToneRgn]);
    emit state_changed(color_balance);
}

void ColorBalance::on_horizontalSlider_red_valueChanged(int value)
{
    color_balance->cyan_red[m_ToneRgn] = value;
    emit state_changed(color_balance);
}

void ColorBalance::on_horizontalSlider_green_valueChanged(int value)
{
    color_balance->magenta_green[m_ToneRgn] = value;
    emit state_changed(color_balance);
}

void ColorBalance::on_horizontalSlider_blue_valueChanged(int value)
{
    color_balance->yellow_blue[m_ToneRgn] = value;
    emit state_changed(color_balance);
}

void ColorBalance::on_pushButton_reset_range_clicked()
{
    ui->horizontalSlider_red->setValue(0);
    ui->horizontalSlider_green->setValue(0);
    ui->horizontalSlider_blue->setValue(0);
}

void ColorBalance::on_pushButton_reset_clicked()
{
    ui->checkBox_preserve_luminosity->setChecked(true);
    //ui->radioButton_Midtone->setChecked(true);
    ui->horizontalSlider_red->setValue(0);
    ui->horizontalSlider_green->setValue(0);
    ui->horizontalSlider_blue->setValue(0);
    for(int i=0;i<3;i++)
    {
        color_balance->cyan_red[i] = 0;
        color_balance->magenta_green[i] = 0;
        color_balance->yellow_blue[i] = 0;
    }
    emit state_changed(color_balance);
}


