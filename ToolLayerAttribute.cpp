#include "ToolLayerAttribute.h"
#include "ui_ToolLayerAttribute.h"
#include <QStandardItem>
#include "Picture.h"
#include "Layer.h"
#include "Commands.h"
#include <QStyledItemDelegate>
#include <QDebug>
ToolLayerAttribute::ToolLayerAttribute(Picture *pic,QStandardItem *item, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolLayerAttribute)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    m_item = item;
    m_pic = pic;
    m_layer = pic->getCurrentLayer();
    m_name = item->text();
    m_type = m_layer->getMergeType();
    m_transparent = m_layer->getTransparent();
    m_checkState = m_layer->isVisible();

    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
    ui->comboBox_mode->setItemDelegate(itemDelegate);
    initComboBox();
    ui->lineEdit_name->setText(item->text());
    ui->comboBox_mode->setCurrentIndex(m_type);
    ui->horizontalSlider_Transparent->setValue(m_transparent);
    ui->checkBox_visible->setChecked(m_checkState);

    connect(this,&ToolLayerAttribute::state_changed,pic,&Picture::MergeMethod);
    connect(this,&ToolLayerAttribute::accepted,this,&ToolLayerAttribute::proc_accept);
    connect(this,&ToolLayerAttribute::rejected,this,&ToolLayerAttribute::proc_reject);

}

ToolLayerAttribute::~ToolLayerAttribute()
{
    delete ui;
}

void ToolLayerAttribute::initComboBox()
{
    for(int i=0;i<2;i++)
        ui->comboBox_mode->setItemData(i,QColor(191,239,255),Qt::BackgroundRole);
    for(int i=2;i<6;i++)
        ui->comboBox_mode->setItemData(i,QColor(188,238,104),Qt::BackgroundRole);
    for(int i=6;i<10;i++)
        ui->comboBox_mode->setItemData(i,QColor(191,239,255),Qt::BackgroundRole);
    for(int i=10;i<17;i++)
        ui->comboBox_mode->setItemData(i,QColor(188,238,104),Qt::BackgroundRole);
    for(int i=17;i<21;i++)
        ui->comboBox_mode->setItemData(i,QColor(191,239,255),Qt::BackgroundRole);
    for(int i=21;i<25;i++)
        ui->comboBox_mode->setItemData(i,QColor(188,238,104),Qt::BackgroundRole);

}

void ToolLayerAttribute::proc_accept()
{
    m_item->setText(ui->lineEdit_name->text());
    QUndoCommand *command = new CommandAttributeLayer(m_pic,m_pic->getCurrentIndex(),m_name,m_type,m_transparent,m_checkState);
    m_pic->getUndoStack()->push(command);
}

void ToolLayerAttribute::proc_reject()
{
    m_layer->setTransparent(m_transparent);
    m_layer->setMergeType(m_type);
    m_layer->setVisible(m_checkState);
    emit state_changed();
}

void ToolLayerAttribute::on_horizontalSlider_Transparent_valueChanged(int value)
{
    m_layer->setTransparent(value);
    emit state_changed();
}

void ToolLayerAttribute::on_comboBox_mode_activated(int index)
{
    m_layer->setMergeType(index);
    emit state_changed();
}

void ToolLayerAttribute::on_checkBox_visible_toggled(bool checked)
{
    m_layer->setVisible(checked);
    emit state_changed();
}
