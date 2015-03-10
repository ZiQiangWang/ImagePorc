#include "FilterMosaic.h"
#include "ui_FilterMosaic.h"
#include "Picture.h"
#include "Commands.h"
#include "Types.h"
#include <QUndoStack>

FilterMosaic::FilterMosaic(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterMosaic)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    m_x = 4;
    m_y = 4;

    connect(ui->horizontalSlider_x,&QSlider::valueChanged,ui->horizontalSlider_y,&QSlider::setValue);
    connect(ui->horizontalSlider_y,&QSlider::valueChanged,ui->horizontalSlider_x,&QSlider::setValue);
    connect(this,&FilterMosaic::state_changed,pic,&Picture::Mosaic);
    connect(this,&FilterMosaic::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterMosaic::accepted,this,&FilterMosaic::proc_accept);
    pic->PushBackRecord();
    pic->Mosaic(m_x,m_y);
}

FilterMosaic::~FilterMosaic()
{
    delete ui;
}

void FilterMosaic::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-马赛克");
    m_pic->getUndoStack()->push(command);
}

void FilterMosaic::on_horizontalSlider_x_valueChanged(int value)
{
    m_x = value;
    emit state_changed(m_x,m_y);
}

void FilterMosaic::on_horizontalSlider_y_valueChanged(int value)
{
    m_y = value;
    emit state_changed(m_x,m_y);
}

void FilterMosaic::on_checkBox_toggled(bool checked)
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

void FilterMosaic::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_x->setValue(4);
    ui->horizontalSlider_y->setValue(4);
    ui->checkBox->setChecked(true);
}
