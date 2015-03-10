#include "FilterRedEyeRemoval.h"
#include "ui_FilterRedEyeRemoval.h"
#include "Picture.h"
#include "Commands.h"

FilterRedEyeRemoval::FilterRedEyeRemoval(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterRedEyeRemoval)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    m_threshold = 50;

    connect(this,&FilterRedEyeRemoval::state_changed,pic,&Picture::RedEyeRemoval);
    connect(this,&FilterRedEyeRemoval::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterRedEyeRemoval::accepted,this,&FilterRedEyeRemoval::proc_accept);
    pic->PushBackRecord();
    pic->RedEyeRemoval(m_threshold);
}

FilterRedEyeRemoval::~FilterRedEyeRemoval()
{
    delete ui;
}

void FilterRedEyeRemoval::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-去除红眼");
    m_pic->getUndoStack()->push(command);
}

void FilterRedEyeRemoval::on_horizontalSlider_valueChanged(int value)
{
    m_threshold = value;
    emit state_changed(m_threshold);
}

void FilterRedEyeRemoval::on_pushButton_reset_clicked()
{
    ui->horizontalSlider->setValue(50);
}
