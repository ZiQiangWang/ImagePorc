#include "FilterUnsharpMask.h"
#include "ui_FilterUnsharpMask.h"
#include "Picture.h"
#include "Commands.h"
#include "Types.h"

FilterUnsharpMask::FilterUnsharpMask(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterUnsharpMask)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    m_sharp = new _UnsharpMask(2,0.5,0);

    connect(this,&FilterUnsharpMask::state_changed,pic,&Picture::UnsharpMask);
    connect(this,&FilterUnsharpMask::rejected,pic,&Picture::PopUpRecord);
    connect(this,&FilterUnsharpMask::accepted,this,&FilterUnsharpMask::proc_accept);
    pic->PushBackRecord();
    pic->UnsharpMask(m_sharp);
}

FilterUnsharpMask::~FilterUnsharpMask()
{
    delete ui;
    delete m_sharp;
}

void FilterUnsharpMask::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"滤镜-虚光蒙版");
    m_pic->getUndoStack()->push(command);
}

void FilterUnsharpMask::on_horizontalSlider_radius_valueChanged(int value)
{
    m_sharp->radius = value;
    emit state_changed(m_sharp);
}

void FilterUnsharpMask::on_horizontalSlider_threshold_valueChanged(int value)
{
    m_sharp->threshold = value;
    emit state_changed(m_sharp);
}

void FilterUnsharpMask::on_horizontalSlider_amount_valueChanged(int value)
{
    m_sharp->amount = (double)value/100;
    emit state_changed(m_sharp);
}

void FilterUnsharpMask::on_pushButton_reset_clicked()
{
    ui->horizontalSlider_radius->setValue(2);
    ui->horizontalSlider_amount->setValue(50);
    ui->horizontalSlider_threshold->setValue(0);
}
