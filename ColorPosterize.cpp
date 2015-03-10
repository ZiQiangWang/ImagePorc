#include "ColorPosterize.h"
#include "ui_ColorPosterize.h"
#include "Picture.h"
#include "Commands.h"

ColorPosterize::ColorPosterize(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorPosterize)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;

    connect(ui->horizontalSlider,&QSlider::valueChanged,pic,&Picture::Posterize);

    connect(this,&ColorPosterize::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorPosterize::accepted,this,&ColorPosterize::proc_accept);
    pic->PushBackRecord();
    pic->Posterize(ui->horizontalSlider->value());
}

ColorPosterize::~ColorPosterize()
{
    delete ui;
}

void ColorPosterize::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-色调分离");
    m_pic->getUndoStack()->push(command);
}

void ColorPosterize::on_pushButton_reset_clicked()
{
    ui->horizontalSlider->setValue(3);
}

