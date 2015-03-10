#include "ToolLayerManager.h"
#include "ui_ToolLayerManager.h"
#include <QStandardItemModel>
#include <QPixmap>
#include <QDebug>
#include "ToolLayerAttribute.h"
#include "Picture.h"
#include "Layer.h"
#include "Utils.h"
ToolLayerManager::ToolLayerManager(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ToolLayerManager)
{
    ui->setupUi(this);
    ui->layerView->setUpdatesEnabled(true);

    connect(ui->layerView,&QListView::clicked,this,&ToolLayerManager::select_changed);
    connect(ui->layerView,&QListView::doubleClicked,this,&ToolLayerManager::onDoubleClicked);
    connect(ui->pushButton_add,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);
    connect(ui->pushButton_remove,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);
    connect(ui->pushButton_copy,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);
    connect(ui->pushButton_up,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);
    connect(ui->pushButton_down,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);
    connect(ui->pushButton_merge,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);
    connect(ui->pushButton_attribute,&QPushButton::clicked,this,&ToolLayerManager::layer_proc);

}

ToolLayerManager::~ToolLayerManager()
{
    delete ui;
}

void ToolLayerManager::switchModel(Picture *pic)
{
    m_pic = pic;
    this->model = pic->Model();
    ui->layerView->setModel(model);
}

void ToolLayerManager::select_changed(const QModelIndex &index)
{
    m_pic->LayerProc(LAYER_SELECTED,index.row());
}

void ToolLayerManager::checkStateChanged(QStandardItem *item)
{
    if(item->checkState() == 0)
        m_pic->getIndexLayer(item->row())->setVisible(false);
    else
        m_pic->getIndexLayer(item->row())->setVisible(true);

    m_pic->MergeMethod();
}

void ToolLayerManager::onDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if(row == -1)
        return;

    ToolLayerAttribute *attribute = new ToolLayerAttribute(m_pic,model->item(row),this);
    attribute->exec();
}

void ToolLayerManager::layer_proc()
{
    int row = ui->layerView->currentIndex().row();
    if(row == -1)
        return;
    if(sender()==ui->pushButton_add)
    {
        m_pic->LayerProc(LAYER_ADD,row);
    }
    else if(sender()==ui->pushButton_remove)
    {
        if(model->rowCount()>1)
        {
            m_pic->LayerProc(LAYER_REMOVE,row);
        }
    }
    else if(sender()==ui->pushButton_copy)
    {
        m_pic->LayerProc(LAYER_COPY,row);
    }
    else if(sender()==ui->pushButton_up)
    {
        if(row >0)
        {
            m_pic->LayerProc(LAYER_UP,row);
        }
    }
    else if(sender()==ui->pushButton_down)
    {
        if(row < model->rowCount()-1)
        {
            m_pic->LayerProc(LAYER_DOWN,row);
        }
    }
    else if(sender()==ui->pushButton_merge)
    {
        if(row < model->rowCount()-1)
        {
            m_pic->LayerProc(LAYER_MERGE,row);
        }
    }
    else if(sender()==ui->pushButton_attribute)
    {
        ToolLayerAttribute *attribute = new ToolLayerAttribute(m_pic,model->item(row),this);
        attribute->exec();
    }
}

void ToolLayerManager::setCurrentIndex(int index)
{
    QModelIndex itemIndex = model->item(index)->index();
    ui->layerView->setCurrentIndex(itemIndex);
}
