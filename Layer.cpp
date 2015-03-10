#include "Layer.h"
#include "Utils.h"
#include <QStandardItem>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

Layer::Layer(const Mat &img, QObject *parent) :
    QObject(parent)
{
    m_workImg = img.clone();
    m_transparent = 255;
    m_mergeType = 0;
    m_isVisible = true;
    m_listItem = new QStandardItem(Mat2Pixmap(m_workImg),"temp");
    m_listItem->setFlags(m_listItem->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    m_listItem->setCheckState(Qt::Checked); // AND initialize check state
}

Layer::Layer(const Layer *layer)
{
    this->m_workImg = layer->m_workImg.clone();
    this->m_transparent = layer->m_transparent;
    this->m_mergeType = layer->m_mergeType;
    this->m_isVisible = layer->m_isVisible;
    this->m_listItem = new QStandardItem(layer->m_listItem->icon(),layer->m_listItem->text());
    this->m_listItem->setFlags(m_listItem->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    this->m_listItem->setCheckState(Qt::Checked); // AND initialize check state
}

Layer::~Layer()
{
    SafeDelete(m_listItem);
}

void Layer::setItemName(const QString &name)
{
    m_listItem->setText(name);
}

QString Layer::getItemName() const
{
    return m_listItem->text();
}

QStandardItem *Layer::getListItem() const
{
    return m_listItem;
}
/**
 * @brief 获取当前操作的图片
 */
Mat Layer::getCurrentImage()
{
    return m_workImg.clone();
}

void Layer::setCurrentImage(const Mat &img)
{
    m_workImg = img.clone();
    m_listItem->setIcon(QIcon(Mat2Pixmap(m_workImg)));
}

void Layer::setTransparent(int value)
{
    m_transparent = value;
}

int Layer::getTransparent() const
{
    return m_transparent;
}

void Layer::setMergeType(int type)
{
    m_mergeType = type;
}

int Layer::getMergeType() const
{
    return m_mergeType;
}

void Layer::setVisible(bool checked)
{
    m_isVisible = checked;
    if(checked)
        m_listItem->setCheckState(Qt::Checked);
    else
        m_listItem->setCheckState(Qt::Unchecked);
}

bool Layer::isVisible() const
{
    return m_isVisible;
}
