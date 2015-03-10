#include "FilterZoomBlur.h"
#include "ui_FilterZoomBlur.h"
#include "Types.h"
#include "Picture.h"
#include "Commands.h"
FilterZoomBlur::FilterZoomBlur(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterZoomBlur)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    blur = new _ZoomBlur(10,0.0,0.0);

    connect(this,&FilterZoomBlur::state_changed,pic,&Picture::ZoomBlur);
    connect(this,&FilterZoomBlur::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterZoomBlur::accepted,this,&FilterZoomBlur::proc_accept);
    pic->PushBackRecord();
    pic->ZoomBlur(blur);
}

FilterZoomBlur::~FilterZoomBlur()
{
    delete ui;
    delete blur;
}

void FilterZoomBlur::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-径向模糊");
    m_pic->getUndoStack()->push(command);
}

void FilterZoomBlur::on_horizontalSlider_focus_valueChanged(int value)
{
    blur->FocalLength = value;
    emit state_changed(blur);
}

void FilterZoomBlur::on_horizontalSlider_center_x_valueChanged(int value)
{
    blur->center_x = (double)value/100;
    ui->doubleSpinBox_center_x->setValue((double)value/100);
    emit state_changed(blur);
}

void FilterZoomBlur::on_doubleSpinBox_center_x_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_center_x->setValue(num);
}

void FilterZoomBlur::on_horizontalSlider_center_y_valueChanged(int value)
{
    blur->center_y = (double)value/100;
    ui->doubleSpinBox_center_y->setValue((double)value/100);
    emit state_changed(blur);
}

void FilterZoomBlur::on_doubleSpinBox_center_y_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_center_y->setValue(num);
}

void FilterZoomBlur::on_pushButton_reset_center_clicked()
{
    ui->horizontalSlider_center_x->setValue(0);
    ui->horizontalSlider_center_y->setValue(0);
}

void FilterZoomBlur::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_focus->setValue(10);
    ui->horizontalSlider_center_x->setValue(0);
    ui->horizontalSlider_center_y->setValue(0);
}


