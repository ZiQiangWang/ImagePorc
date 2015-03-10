#include "FilterSurfaceBlur.h"
#include "ui_FilterSurfaceBlur.h"
#include "Picture.h"
#include "Commands.h"

FilterSurfaceBlur::FilterSurfaceBlur(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterSurfaceBlur)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    m_radius = 6;
    m_threshold = 15;

    connect(this,&FilterSurfaceBlur::state_changed,pic,&Picture::SurfaceBlur);
    connect(this,&FilterSurfaceBlur::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterSurfaceBlur::accepted,this,&FilterSurfaceBlur::proc_accept);
    pic->PushBackRecord();
    pic->SurfaceBlur(m_radius,m_threshold);
}

FilterSurfaceBlur::~FilterSurfaceBlur()
{
    delete ui;
}

void FilterSurfaceBlur::proc_accept()
{

}

void FilterSurfaceBlur::on_horizontalSlider_radius_valueChanged(int value)
{
    m_radius = value;
    emit state_changed(m_radius,m_threshold);
}

void FilterSurfaceBlur::on_horizontalSlider_threshold_valueChanged(int value)
{
    m_threshold = value;
    emit state_changed(m_radius,m_threshold);
}

void FilterSurfaceBlur::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_radius->setValue(6);
    ui->horizontalSlider_threshold->setValue(15);
}


