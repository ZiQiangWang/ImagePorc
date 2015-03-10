#include "FilterEmboss.h"
#include "ui_FilterEmboss.h"
#include "Picture.h"
#include "Commands.h"
#include "Types.h"

FilterEmboss::FilterEmboss(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterEmboss)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;
    m_emboss = new _Emboss(30,45,20,1);

    connect(this,&FilterEmboss::state_changed,pic,&Picture::Emboss);
    connect(this,&FilterEmboss::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterEmboss::accepted,this,&FilterEmboss::proc_accept);

    pic->PushBackRecord();
    pic->Emboss(m_emboss);
}

FilterEmboss::~FilterEmboss()
{
    delete ui;
}

void FilterEmboss::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-浮雕");
    m_pic->getUndoStack()->push(command);
}

void FilterEmboss::on_radioButton_bumpmap_toggled(bool checked)
{
    if(checked)
        m_emboss->embossp = 0;
    emit state_changed(m_emboss);
}

void FilterEmboss::on_radioButton_emboss_toggled(bool checked)
{
    if(checked)
        m_emboss->embossp = 1;
    emit state_changed(m_emboss);
}

void FilterEmboss::on_horizontalSlider_azimuth_valueChanged(int value)
{
    m_emboss->azimuth = value;
    emit state_changed(m_emboss);
}

void FilterEmboss::on_horizontalSlider_elevation_valueChanged(int value)
{
    m_emboss->elevation = value;
    emit state_changed(m_emboss);
}

void FilterEmboss::on_horizontalSlider_depth_valueChanged(int value)
{
    m_emboss->depth = value;
    emit state_changed(m_emboss);
}

void FilterEmboss::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_azimuth->setValue(30);
    ui->horizontalSlider_elevation->setValue(45);
    ui->horizontalSlider_depth->setValue(20);
}

