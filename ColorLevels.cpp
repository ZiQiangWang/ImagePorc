#include "ColorLevels.h"
#include "ui_ColorLevels.h"
#include "Picture.h"
#include "Layer.h"
#include "Commands.h"
#include "PixmapItem.h"
#include "Types.h"
#include "Utils.h"
#include "Effect.h"
#include <QDebug>
#include <QMainWindow>

ColorLevels::ColorLevels(Picture *pic, PixmapItem *item,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorLevels)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->move(20,100);

    m_pic = pic;
    m_item = item;
    m_item->setFlag(QGraphicsItem::ItemIsSelectable);
    m_item->savePixmap(Mat2Pixmap(pic->getCurrentLayer()->getCurrentImage()));

    m_hist = new Histogram(pic->getCurrentLayer()->getCurrentImage(),histImage);
    ui->label_hist->setPixmap(Mat2Pixmap(histImage[0]));

    levels = new _Levels;
    for(int channel=0;channel<5;channel++)
    {
        levels->gamma[channel]       = 1.0;
        levels->low_input[channel]   = 0;
        levels->high_input[channel]  = 255;
        levels->low_output[channel]  = 0;
        levels->high_output[channel] = 255;
    }
    m_channel=0;

    connect(ui->horizontalSlider_high_input,&QSlider::valueChanged,this,&ColorLevels::set_min_intput_high);
    connect(ui->horizontalSlider_low_input,&QSlider::valueChanged,this,&ColorLevels::set_max_intput_low);

    connect(m_item,&PixmapItem::getColor,this,&ColorLevels::onColorPicker);

    connect(this,&ColorLevels::state_changed,pic,&Picture::Levels);

    connect(this,&ColorLevels::rejected,pic,&Picture::PopUpRecord);
    connect(this,&ColorLevels::accepted,this,&ColorLevels::proc_accept);
    connect(this,&ColorLevels::finished,this,&ColorLevels::onClose);
    pic->PushBackRecord();
}

ColorLevels::~ColorLevels()
{
    delete ui;
    delete levels;
}


void ColorLevels::proc_accept()
{
    QUndoCommand *command = new CommandImage(m_pic,"颜色变换-色阶");
    m_pic->getUndoStack()->push(command);
}

void ColorLevels::onColorPicker(int *rgb)
{
    if(ui->pushButton_black->isChecked())
    {
        if(m_channel==0)
        {
            levels->low_input[0] = 0.299*rgb[0] + 0.587*rgb[1] + 0.114*rgb[2];

        }
        else
        {
            levels->low_input[m_channel] = rgb[m_channel-1];
        }
        emit state_changed(levels);
        ui->horizontalSlider_low_input->setValue(levels->low_input[m_channel]);
    }
    else if(ui->pushButton_white->isChecked())
    {
        if(m_channel==0)
        {
            levels->high_input[0] = 0.299*rgb[0] + 0.587*rgb[1] + 0.114*rgb[2];
        }
        else
        {
            levels->high_input[m_channel] = rgb[m_channel-1];
        }
        emit state_changed(levels);
        ui->horizontalSlider_high_input->setValue(levels->high_input[m_channel]);
    }
    else if(ui->pushButton_black_2->isChecked())
    {
        levels->low_input[1]=rgb[0];
        levels->low_input[2]=rgb[1];
        levels->low_input[3]=rgb[2];
        emit state_changed(levels);
        ui->horizontalSlider_low_input->setValue(levels->low_input[m_channel]);
    }
    else if(ui->pushButton_gray->isChecked())
    {
        double avg = (double)(rgb[0]+rgb[1]+rgb[2])/(3*255);
        double r = (double)rgb[0]/255;
        double g = (double)rgb[1]/255;
        double b = (double)rgb[2]/255;


        levels->gamma[1]=log(r)/log(avg);
        levels->gamma[2]=log(g)/log(avg);
        levels->gamma[3]=log(b)/log(avg);

        emit state_changed(levels);
        ui->horizontalSlider_gamma->setValue((int)(levels->gamma[m_channel]*100));
    }
    else if(ui->pushButton_white_2->isChecked())
    {
        levels->high_input[1]=rgb[0];
        levels->high_input[2]=rgb[1];
        levels->high_input[3]=rgb[2];
        emit state_changed(levels);
        ui->horizontalSlider_high_input->setValue(levels->high_input[m_channel]);
    }
}

void ColorLevels::set_min_intput_high()
{
    if(ui->horizontalSlider_high_input->value()<=ui->horizontalSlider_low_input->value())
        ui->horizontalSlider_high_input->setValue(ui->horizontalSlider_low_input->value());
}

void ColorLevels::set_max_intput_low()
{
    if(ui->horizontalSlider_low_input->value()>=ui->horizontalSlider_high_input->value())
        ui->horizontalSlider_low_input->setValue(ui->horizontalSlider_high_input->value());
}

void ColorLevels::on_comboBox_channels_currentIndexChanged(int index)
{
    ui->label_hist->setPixmap(Mat2Pixmap(histImage[index]));
    m_channel = index;
    ui->horizontalSlider_low_input->setValue(levels->low_input[m_channel]);
    ui->horizontalSlider_gamma->setValue(levels->gamma[m_channel]*100);
    ui->horizontalSlider_high_input->setValue(levels->high_input[m_channel]);
    ui->horizontalSlider_low_output->setValue(levels->low_output[m_channel]);
    ui->horizontalSlider_high_output->setValue(levels->high_output[m_channel]);
    emit state_changed(levels);
}

void ColorLevels::on_horizontalSlider_low_input_valueChanged(int value)
{
    levels->low_input[m_channel]=value;
    emit state_changed(levels);
}

void ColorLevels::on_horizontalSlider_gamma_valueChanged(int value)
{
    levels->gamma[m_channel] = (double)value/100.0;
    emit state_changed(levels);
    ui->doubleSpinBox_gamma->setValue((double)value/100.0);
}

void ColorLevels::on_doubleSpinBox_gamma_valueChanged(const QString &value)
{
    QString temp = value;
    int num = temp.remove(QChar('.'),Qt::CaseInsensitive).toInt();
    ui->horizontalSlider_gamma->setValue(num);
}

void ColorLevels::on_horizontalSlider_high_input_valueChanged(int value)
{
    levels->high_input[m_channel]=value;
    emit state_changed(levels);
}

void ColorLevels::on_horizontalSlider_low_output_valueChanged(int value)
{
    levels->low_output[m_channel]=value;
    emit state_changed(levels);
}

void ColorLevels::on_horizontalSlider_high_output_valueChanged(int value)
{
    levels->high_output[m_channel]=value;
    emit state_changed(levels);
}

void ColorLevels::on_pushButton_reset_channel_clicked()
{
    ui->horizontalSlider_low_input->setValue(0);
    ui->horizontalSlider_gamma->setValue(100);
    ui->horizontalSlider_high_input->setValue(255);
    ui->horizontalSlider_low_output->setValue(0);
    ui->horizontalSlider_high_output->setValue(255);
}

void ColorLevels::on_pushButton_reset_clicked()
{
    for(int channel=0;channel<5;channel++)
    {
        levels->gamma[channel]       = 1.0;
        levels->low_input[channel]   = 0;
        levels->high_input[channel]  = 255;
        levels->low_output[channel]  = 0;
        levels->high_output[channel] = 255;
    }

    ui->horizontalSlider_low_input->setValue(0);
    ui->horizontalSlider_gamma->setValue(100);
    ui->horizontalSlider_high_input->setValue(255);
    ui->horizontalSlider_low_output->setValue(0);
    ui->horizontalSlider_high_output->setValue(255);

    emit state_changed(levels);
}

void ColorLevels::on_pushButton_black_clicked(bool checked)
{
    if(checked)
    {
        resetButton();
        ui->pushButton_black->setChecked(true);
        m_item->setCursor(QCursor(QPixmap(":/icon/color/Image/stock-color-picker-black.png"),0,16));
    }
    else
    {
        m_item->setCursorFlag(NORMAL);
        m_item->unsetCursor();
    }
}

void ColorLevels::on_pushButton_white_clicked(bool checked)
{

    if(checked)
    {
        resetButton();
        ui->pushButton_white->setChecked(true);
        m_item->setCursor(QCursor(QPixmap(":/icon/color/Image/stock-color-picker-white.png"),0,16));
    }
    else
    {
        m_item->setCursorFlag(NORMAL);
        m_item->unsetCursor();
    }
}

void ColorLevels::on_pushButton_black_2_clicked(bool checked)
{
    if(checked)
    {
        resetButton();
        ui->pushButton_black_2->setChecked(true);
        m_item->setCursor(QCursor(QPixmap(":/icon/color/Image/stock-color-picker-black.png"),0,16));
    }
    else
    {
        m_item->setCursorFlag(NORMAL);
        m_item->unsetCursor();
    }
}

void ColorLevels::on_pushButton_gray_clicked(bool checked)
{
    if(checked)
    {
        resetButton();
        ui->pushButton_gray->setChecked(true);
        m_item->setCursor(QCursor(QPixmap(":/icon/color/Image/stock-color-picker-gray.png"),0,16));
    }
    else
    {
        m_item->setCursorFlag(NORMAL);
        m_item->unsetCursor();
    }
}

void ColorLevels::on_pushButton_white_2_clicked(bool checked)
{
    if(checked)
    {
        resetButton();
        ui->pushButton_white_2->setChecked(true);
        m_item->setCursor(QCursor(QPixmap(":/icon/color/Image/stock-color-picker-white.png"),0,16));
    }
    else
    {
        m_item->setCursorFlag(NORMAL);
        m_item->unsetCursor();
    }
}

void ColorLevels::on_pushButton_auto_clicked()
{

    for(int channel=0;channel<5;channel++)
    {
        levels->gamma[channel]       = 1.0;
        levels->low_input[channel]   = 0;
        levels->high_input[channel]  = 255;
        levels->low_output[channel]  = 0;
        levels->high_output[channel] = 255;
    }

    Histogram * hist = new Histogram(m_pic->getCurrentLayer()->getCurrentImage());

    for(int channel = 1;channel <= 3;channel++)
    {
        hist->calculate(0,255,channel);
        const int nCount = hist->getTotal();
        if (nCount == 0)
        {
            levels->low_input[channel] = levels->high_input[channel] = 0 ;
        }
        else
        {
            // Set the low input
            for (int i=0 ; i < 255 ; i++)
            {
                hist->calculate(0,i,channel);
                double percentage = hist->getPercent();
                if(percentage>=1)
                {
                    levels->low_input[channel] = i;
                    break;
                }
            }
            // Set the high input
            for (int i=255 ; i > 0 ; i--)
            {
                hist->calculate(i,255,channel);
                double percentage = hist->getPercent();
                if(percentage>=1)
                {
                    levels->high_input[channel] = i;
                    break;
                }
            }
        }
    }

    ui->horizontalSlider_low_input->setValue(levels->low_input[m_channel]);
    ui->horizontalSlider_high_input->setValue(levels->high_input[m_channel]);

    emit state_changed(levels);
}

void ColorLevels::resetButton()
{
    m_item->setCursorFlag(COLOR_PICKER);
    ui->pushButton_black->setChecked(false);
    ui->pushButton_white->setChecked(false);
    ui->pushButton_black_2->setChecked(false);
    ui->pushButton_white_2->setChecked(false);
    ui->pushButton_gray->setChecked(false);
}

void ColorLevels::onClose()
{
    m_item->setCursorFlag(NORMAL);
    m_item->unsetCursor();
    m_item->setFlag(QGraphicsItem::ItemIsSelectable,false);
}

