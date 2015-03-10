#include "FilterRadialBlur.h"
#include "ui_FilterRadialBlur.h"
#include "Types.h"
#include "Picture.h"
#include "Commands.h"

FilterRadialBlur::FilterRadialBlur(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterRadialBlur)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    blur = new _RadialBlur(2,0.0,0.0);

    connect(this,&FilterRadialBlur::state_changed,pic,&Picture::RadialBlur);
    connect(this,&FilterRadialBlur::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterRadialBlur::accepted,this,&FilterRadialBlur::proc_accept);
    pic->PushBackRecord();
    pic->RadialBlur(blur);
}

FilterRadialBlur::~FilterRadialBlur()
{
    delete ui;
    delete blur;
}

void FilterRadialBlur::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-旋转模糊");
    m_pic->getUndoStack()->push(command);
}

void FilterRadialBlur::on_horizontalSlider_angle_valueChanged(int value)
{
    blur->angle = value;
    emit state_changed(blur);
}

void FilterRadialBlur::on_horizontalSlider_center_x_valueChanged(int value)
{
    blur->center_x = (double)value/100;
    ui->doubleSpinBox_center_x->setValue((double)value/100);
    emit state_changed(blur);
}

void FilterRadialBlur::on_doubleSpinBox_center_x_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_center_x->setValue(num);
}

void FilterRadialBlur::on_horizontalSlider_center_y_valueChanged(int value)
{
    blur->center_y = (double)value/100;
    ui->doubleSpinBox_center_y->setValue((double)value/100);
    emit state_changed(blur);
}

void FilterRadialBlur::on_doubleSpinBox_center_y_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_center_y->setValue(num);
}

void FilterRadialBlur::on_pushButton_reset_center_clicked()
{
    ui->horizontalSlider_center_x->setValue(0);
    ui->horizontalSlider_center_y->setValue(0);
}

void FilterRadialBlur::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_angle->setValue(2);
    ui->horizontalSlider_center_x->setValue(0);
    ui->horizontalSlider_center_y->setValue(0);
}


