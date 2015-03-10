#include "FilterGaussianBlur.h"
#include "ui_FilterGaussianBlur.h"
#include "Picture.h"
#include "Commands.h"

FilterGaussianBlur::FilterGaussianBlur(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterGaussianBlur)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    m_x = 2;
    m_y = 2;

    connect(ui->horizontalSlider_x,&QSlider::valueChanged,ui->horizontalSlider_y,&QSlider::setValue);
    connect(ui->horizontalSlider_y,&QSlider::valueChanged,ui->horizontalSlider_x,&QSlider::setValue);
    connect(this,&FilterGaussianBlur::state_changed,pic,&Picture::GaussianBlur2);
    connect(this,&FilterGaussianBlur::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterGaussianBlur::accepted,this,&FilterGaussianBlur::proc_accept);
    pic->PushBackRecord();
    pic->GaussianBlur2(m_x,m_y);
}

FilterGaussianBlur::~FilterGaussianBlur()
{
    delete ui;
}

void FilterGaussianBlur::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-高斯模糊");
    m_pic->getUndoStack()->push(command);
}

void FilterGaussianBlur::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        connect(ui->horizontalSlider_x,&QSlider::valueChanged,ui->horizontalSlider_y,&QSlider::setValue);
        connect(ui->horizontalSlider_y,&QSlider::valueChanged,ui->horizontalSlider_x,&QSlider::setValue);
    }
    else
    {
        disconnect(ui->horizontalSlider_x,&QSlider::valueChanged,ui->horizontalSlider_y,&QSlider::setValue);
        disconnect(ui->horizontalSlider_y,&QSlider::valueChanged,ui->horizontalSlider_x,&QSlider::setValue);
    }
}

void FilterGaussianBlur::on_horizontalSlider_x_valueChanged(int value)
{
    m_x = value;
    emit state_changed(m_x,m_y);
}

void FilterGaussianBlur::on_horizontalSlider_y_valueChanged(int value)
{
    m_y = value;
    emit state_changed(m_x,m_y);
}

void FilterGaussianBlur::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_x->setValue(2);
    ui->horizontalSlider_y->setValue(2);
}
