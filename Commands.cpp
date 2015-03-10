#include "Commands.h"
#include "Layer.h"
#include "Picture.h"
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

/******************************************************************************************************************/
//图像内容操作命令
CommandImage::CommandImage(Picture *pic,const QString &name, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    this->setText(name);
    m_pic = pic;
    m_index = pic->getCurrentIndex();
    m_savedImg = pic->getCurrentLayer()->getCurrentImage();
    pic->pop_up_record(m_index);
}

void CommandImage::undo()
{
    m_pic->pop_up_record(m_index);
}

void CommandImage::redo()
{
    m_pic->push_back_record(m_index);
    m_pic->getIndexLayer(m_index)->setCurrentImage(m_savedImg);
}

/******************************************************************************************************************/
//图层操作命令
/**
 * @brief 添加图层
 */
CommandAddLayer::CommandAddLayer(Picture *pic, int index, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    this->setText("添加图层");
}

void CommandAddLayer::undo()
{
    m_pic->removeLayer(m_index);
}

void CommandAddLayer::redo()
{
    m_pic->addLayer(m_index);
}

/**
 * @brief 删除图层
 */
CommandRemoveLayer::CommandRemoveLayer(Picture *pic, int index, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    this->setText("删除图层");
}

void CommandRemoveLayer::undo()
{
    m_pic->insertLayer(m_savedLayer,m_index);
}

void CommandRemoveLayer::redo()
{
    m_savedLayer = new Layer(m_pic->getCurrentLayer());
    m_pic->removeLayer(m_index);
}

/**
 * @brief 复制图层
 */
CommandCopyLayer::CommandCopyLayer(Picture *pic, int index, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    m_savedLayer = new Layer(m_pic->getCurrentLayer());
    this->setText("复制图层");
}

void CommandCopyLayer::undo()
{
    m_pic->removeLayer(m_index);
}

void CommandCopyLayer::redo()
{
    Layer *layer = new Layer(m_savedLayer);
    m_pic->insertLayer(layer,m_index);
}

/**
 * @brief 向下合并图层
 */
CommandMergeLayer::CommandMergeLayer(Picture *pic, int index, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    m_savedImg = pic->getIndexLayer(index+1)->getCurrentImage().clone();
    this->setText("向下合并图层");
}

void CommandMergeLayer::undo()
{
    m_pic->getCurrentLayer()->setCurrentImage(m_savedImg);
    m_pic->insertLayer(m_savedLayer,m_index);
}

void CommandMergeLayer::redo()
{
    m_savedLayer = new Layer(m_pic->getCurrentLayer());
    m_pic->mergeLayer(m_index);
}


/**
 * @brief 向上移动图层
 */
CommandMoveUpLayer::CommandMoveUpLayer(Picture *pic, int index, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    this->setText("向上移动图层");
}

void CommandMoveUpLayer::undo()
{
    m_pic->moveDownLayer(m_index-1);
}

void CommandMoveUpLayer::redo()
{
    m_pic->moveUpLayer(m_index);
}

/**
 * @brief 向下移动图层
 */
CommandMoveDownLayer::CommandMoveDownLayer(Picture *pic, int index, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    this->setText("向下移动图层");
}

void CommandMoveDownLayer::undo()
{
    m_pic->moveUpLayer(m_index+1);
}

void CommandMoveDownLayer::redo()
{
    m_pic->moveDownLayer(m_index);
}

/**
 * @brief 图层属性操作
 */
CommandAttributeLayer::CommandAttributeLayer(Picture *pic, int index,const QString &name,int type,int transparent,bool checked, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_pic = pic;
    m_index = index;
    m_oldName = name;
    m_oldType = type;
    m_oldTransparent = transparent;
    m_oldCheckState = checked;

    Layer *layer = m_pic->getIndexLayer(m_index);
    m_name = layer->getItemName();
    m_type = layer->getMergeType();
    m_transparent = layer->getTransparent();
    m_checkState = layer->isVisible();
    this->setText("图层属性操作");
}

void CommandAttributeLayer::undo()
{
    Layer *layer = m_pic->getIndexLayer(m_index);
    layer->setItemName(m_oldName);
    layer->setMergeType(m_oldType);
    layer->setTransparent(m_oldTransparent);
    layer->setVisible(m_oldCheckState);
    m_pic->MergeMethod();
}

void CommandAttributeLayer::redo()
{
    Layer *layer = m_pic->getIndexLayer(m_index);
    layer->setItemName(m_name);
    layer->setMergeType(m_type);
    layer->setTransparent(m_transparent);
    layer->setVisible(m_checkState);
    m_pic->MergeMethod();
}





