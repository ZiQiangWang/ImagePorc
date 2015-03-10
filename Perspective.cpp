#include "Perspective.h"
#include "ui_Perspective.h"
#include "Commands.h"
#include "Picture.h"
#include "Layer.h"
#include <QSlider>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

Perspective::Perspective(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Perspective)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;
    m_width = pic->getCurrentLayer()->getCurrentImage().cols;
    m_height = pic->getCurrentLayer()->getCurrentImage().rows;

    src[0].x = 0;
    src[0].y = 0;
    src[1].x = 0;
    src[1].y = m_height-1;
    src[2].x = m_width-1;
    src[2].y = 0;
    src[3].x = m_width-1;
    src[3].y = m_height-1;

    dst[0].x = 0;
    dst[0].y = 0;
    dst[1].x = 0;
    dst[1].y = m_height-1;
    dst[2].x = m_width-1;
    dst[2].y = 0;
    dst[3].x = m_width-1;
    dst[3].y = m_height-1;

    connect(ui->horizontalSlider_TL_x,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_TL_y,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_BL_x,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_BL_y,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_TR_x,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_TR_y,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_BR_x,&QSlider::valueChanged,this,&Perspective::slider_changed);
    connect(ui->horizontalSlider_BR_y,&QSlider::valueChanged,this,&Perspective::slider_changed);

    connect(this,&Perspective::state_changed,pic,&Picture::perspective);
    connect(this,&Perspective::accepted,this,&Perspective::proc_accept);
    connect(this,&Perspective::rejected,pic,&Picture::PopUpRecord);

    pic->PushBackRecord();
}

Perspective::~Perspective()
{
    delete ui;
}

void Perspective::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_TL_x->setValue(0);
    ui->horizontalSlider_TL_y->setValue(0);
    ui->horizontalSlider_BL_x->setValue(0);
    ui->horizontalSlider_BL_y->setValue(100);
    ui->horizontalSlider_TR_x->setValue(100);
    ui->horizontalSlider_TR_y->setValue(0);
    ui->horizontalSlider_BR_x->setValue(100);
    ui->horizontalSlider_BR_y->setValue(100);
}

void Perspective::slider_changed(int value)
{
    double delta = static_cast<double>(value)/100;

    if(sender()==ui->horizontalSlider_TL_x)
        dst[0].x = m_width*delta;
    else if(sender()==ui->horizontalSlider_TL_y)
        dst[0].y = m_height*delta;
    else if(sender()==ui->horizontalSlider_BL_x)
        dst[1].x = m_width*delta;
    else if(sender()==ui->horizontalSlider_BL_y)
        dst[1].y = m_height*delta-1;
    else if(sender()==ui->horizontalSlider_TR_x)
        dst[2].x = m_width*delta-1;
    else if(sender()==ui->horizontalSlider_TR_y)
        dst[2].y = m_height*delta;
    else if(sender()==ui->horizontalSlider_BR_x)
        dst[3].x = m_width*delta-1;
    else if(sender()==ui->horizontalSlider_BR_y)
        dst[3].y = m_height*delta-1;
    t = getPerspectiveTransform(src,dst);

    emit state_changed(t);
}

void Perspective::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"变换-透视变换");
    m_pic->getUndoStack()->push(command);
}
