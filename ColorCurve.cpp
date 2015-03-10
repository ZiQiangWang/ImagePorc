#include "ColorCurve.h"
#include "ui_ColorCurve.h"
#include "Picture.h"
#include "Layer.h"
#include "Commands.h"
#include "Utils.h"
#include "Effect.h"
#include "Types.h"
#include <QDebug>

ColorCurve::ColorCurve(Picture *pic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorCurve)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->move(20,100);

    m_pic = pic;
    m_curve = new _Curve;
    m_channel = 0;

    for(int i=0;i<256;i++)
    {
        m_curve->rgb[i]=i;
        m_curve->r[i]=i;
        m_curve->g[i]=i;
        m_curve->b[i]=i;
    }

    hist = new Histogram(pic->getCurrentLayer()->getCurrentImage(),histImage);
    ui->curve->axisRect()->setBackground(Mat2Pixmap(histImage[0]));

    connect(ui->curve,&CurveEditor::valueChanged,this,&ColorCurve::onValueChanged);
    connect(ui->curve,&CurveEditor::getPosition,this,&ColorCurve::show_position);
    connect(this,&ColorCurve::state_changed,pic,&Picture::Curve);
    connect(this,&ColorCurve::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorCurve::accepted,this,&ColorCurve::proc_accept);
    pic->PushBackRecord();
}

ColorCurve::~ColorCurve()
{
    delete ui;
    delete hist;
    delete m_curve;
}

void ColorCurve::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-曲线");
    m_pic->getUndoStack()->push(command);
}

void ColorCurve::onValueChanged(QVector<double> value)
{

    switch(m_channel)
    {
    case 0:
        for(int i=0;i<value.size();i++)
        {
            m_curve->rgb[i]=value[i];
        }
        break;
    case 1:
        for(int i=0;i<value.size();i++)
        {
            m_curve->r[i]=value[i];
        }
        break;
    case 2:
        for(int i=0;i<value.size();i++)
        {
            m_curve->g[i]=value[i];
        }
        break;
    case 3:
        for(int i=0;i<value.size();i++)
        {
            m_curve->b[i]=value[i];
        }
        break;
    }
    emit state_changed(m_curve);
}

void ColorCurve::show_position(QPoint pos)
{
    if(pos.x()==-1 && pos.y()==-1)
    {
        ui->label_position->clear();
        ui->label_position->setText(QString(""));
    }
    else
    {
        int key = Clamp0255(pos.x());
        int value = Clamp0255(pos.y());
        ui->label_position->setText(QString("(%1，%2)").arg(key).arg(value));
    }
}

void ColorCurve::on_comboBox_currentIndexChanged(int index)
{
    m_channel = index;
    ui->curve->setChannel(index);
    ui->curve->channelChanged();

    ui->curve->axisRect()->setBackground(Mat2Pixmap(histImage[index]));
    ui->curve->replot();
}

void ColorCurve::on_pushButton_reset_channel_clicked()
{
    ui->curve->resetChannel(m_channel);
    switch(m_channel)
    {
    case 0:
        for(int i=0;i<256;i++)
        {
            m_curve->rgb[i]=i;
        }
        break;
    case 1:
        for(int i=0;i<256;i++)
        {
            m_curve->r[i]=i;
        }
        break;
    case 2:
        for(int i=0;i<256;i++)
        {
            m_curve->g[i]=i;
        }
        break;
    case 3:
        for(int i=0;i<256;i++)
        {
            m_curve->b[i]=i;
        }
        break;
    }
}

void ColorCurve::on_pushButton_reset_clicked()
{
    for(int i=0;i<4;i++)
    {
        ui->curve->resetChannel(i);
    }
    for(int i=0;i<256;i++)
    {
        m_curve->rgb[i]=i;
        m_curve->r[i]=i;
        m_curve->g[i]=i;
        m_curve->b[i]=i;
    }
    emit state_changed(m_curve);
}
