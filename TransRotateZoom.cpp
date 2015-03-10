#include "TransRotateZoom.h"
#include "ui_TransRotateZoom.h"
#include "Types.h"
#include "Picture.h"
#include "Commands.h"
#include <QDebug>
TransRotateZoom::TransRotateZoom(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransRotateZoom)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    rotate = new _RotateZoom(0,0,0,0,0,1.0);

    connect(this,&TransRotateZoom::state_changed,pic,&Picture::RotateZoom);
    connect(this,&TransRotateZoom::rejected,pic,&Picture::PopUpRecord);
    connect(this,&TransRotateZoom::accepted,this,&TransRotateZoom::proc_accept);
    pic->PushBackRecord();
}

TransRotateZoom::~TransRotateZoom()
{
    delete ui;
}

void TransRotateZoom::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"变换-旋转-缩放-平移");
    m_pic->getUndoStack()->push(command);
}

void TransRotateZoom::Transform()
{

}

void TransRotateZoom::on_horizontalSlider_rotate_valueChanged(int value)
{
    rotate->angle = 0-value;
    emit state_changed(rotate);
}

void TransRotateZoom::on_horizontalSlider_per_rotate_valueChanged(int value)
{
    rotate->per_angle = value;
    emit state_changed(rotate);
}

void TransRotateZoom::on_horizontalSlider_perspective_valueChanged(int value)
{
    rotate->perspective = value;
    emit state_changed(rotate);
}

void TransRotateZoom::on_horizontalSlider_offset_x_valueChanged(int value)
{
    ui->doubleSpinBox_offset_x->setValue((double)value/100);
    rotate->x = (double)value/100;
    emit state_changed(rotate);
}

void TransRotateZoom::on_doubleSpinBox_offset_x_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_offset_x->setValue(num);
}

void TransRotateZoom::on_horizontalSlider_offset_y_valueChanged(int value)
{
    ui->doubleSpinBox_offset_y->setValue((double)value/100);
    rotate->y = (double)value/100;
    emit state_changed(rotate);
}

void TransRotateZoom::on_doubleSpinBox_offset_y_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_offset_y->setValue(num);
}

void TransRotateZoom::on_horizontalSlider_zoom_valueChanged(int value)
{
    ui->doubleSpinBox_zoom->setValue((double)value/100);
    rotate->scale = (double)value/100;
    emit state_changed(rotate);
}

void TransRotateZoom::on_doubleSpinBox_zoom_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_zoom->setValue(num);
}

void TransRotateZoom::on_pushButton_reset_rotate_clicked()
{
    ui->horizontalSlider_rotate->setValue(0);
    ui->horizontalSlider_per_rotate->setValue(0);
    ui->horizontalSlider_perspective->setValue(0);
}

void TransRotateZoom::on_pushButton_reset_offset_clicked()
{
    ui->horizontalSlider_offset_x->setValue(0);
    ui->horizontalSlider_offset_y->setValue(0);
}

void TransRotateZoom::on_pushButton_reset_zoom_clicked()
{
    ui->horizontalSlider_zoom->setValue(100);
}

void TransRotateZoom::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_rotate->setValue(0);
    ui->horizontalSlider_per_rotate->setValue(0);
    ui->horizontalSlider_perspective->setValue(0);
    ui->horizontalSlider_offset_x->setValue(0);
    ui->horizontalSlider_offset_y->setValue(0);
    ui->horizontalSlider_zoom->setValue(100);
}



