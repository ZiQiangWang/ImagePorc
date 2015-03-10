#include "ColorColorize.h"
#include "ui_ColorColorize.h"
#include "Picture.h"
#include "Commands.h"

ColorColorize::ColorColorize(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorColorize)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;
    m_h = 180;
    m_s = 50;
    m_l = 0;

    connect(this,&ColorColorize::state_changed,pic,&Picture::Colorize);

    connect(this,&ColorColorize::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorColorize::accepted,this,&ColorColorize::proc_accept);

    pic->PushBackRecord();
    pic->Colorize(m_h,m_s,m_l);
}

ColorColorize::~ColorColorize()
{
    delete ui;
}

void ColorColorize::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-单色化");
    m_pic->getUndoStack()->push(command);
}

void ColorColorize::on_horizontalSlider_H_valueChanged(int value)
{
    m_h = value;
    emit state_changed(m_h,m_s,m_l);
}

void ColorColorize::on_horizontalSlider_S_valueChanged(int value)
{
    m_s = value;
    emit state_changed(m_h,m_s,m_l);
}

void ColorColorize::on_horizontalSlider_L_valueChanged(int value)
{
    m_l = value;
    emit state_changed(m_h,m_s,m_l);
}

void ColorColorize::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_H->setValue(180);
    ui->horizontalSlider_S->setValue(50);
    ui->horizontalSlider_L->setValue(0);
}
