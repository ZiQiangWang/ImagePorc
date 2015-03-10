#include "FilterMotionBlur.h"
#include "ui_FilterMotionBlur.h"
#include "Picture.h"
#include "Commands.h"
#include "Types.h"

FilterMotionBlur::FilterMotionBlur(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterMotionBlur)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    blur = new _MotionBlur(25,10,true);

    connect(this,&FilterMotionBlur::state_changed,pic,&Picture::MotionBlur);
    connect(this,&FilterMotionBlur::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterMotionBlur::accepted,this,&FilterMotionBlur::proc_accept);
    pic->PushBackRecord();
    pic->MotionBlur(blur);
}

FilterMotionBlur::~FilterMotionBlur()
{
    delete ui;
    delete blur;
}

void FilterMotionBlur::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-运动模糊");
    m_pic->getUndoStack()->push(command);
}

void FilterMotionBlur::on_horizontalSlider_angle_valueChanged(int value)
{
    blur->angle = value;
    emit state_changed(blur);
}

void FilterMotionBlur::on_horizontalSlider_length_valueChanged(int value)
{
    blur->distance = value;
    emit state_changed(blur);
}

void FilterMotionBlur::on_checkBox_toggled(bool checked)
{
    blur->centered = checked;
    emit state_changed(blur);
}

void FilterMotionBlur::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_angle->setValue(25);
    ui->horizontalSlider_length->setValue(10);
    ui->checkBox->setChecked(true);
}


