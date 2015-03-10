#include "PixmapItem.h"
#include <QApplication>
#include <QGraphicsSceneWheelEvent>
#include <QPixmap>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
//#include "Utils.h"
#include <opencv2/core/core.hpp>
using namespace cv;

PixmapItem::PixmapItem()
{
    cursorFlag = NORMAL;
}

PixmapItem::PixmapItem(const QPixmap &pixmap)
{
    this->setPixmap(pixmap);
    cursorFlag = NORMAL;
}

PixmapItem::~PixmapItem()
{
}

void PixmapItem::setCursorFlag(int flag)
{
    cursorFlag = flag;
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (cursorFlag)
    {
    case COLOR_PICKER:
        if (!savedPxmap.isNull())
        {
            QImage image = savedPxmap.toImage();
            if (!image.isNull())
            {
                if (image.valid(event->pos().x(), event->pos().y()))
                {
                    QColor color = image.pixel(event->pos().x(), event->pos().y());
                    int rgb[3];
                    rgb[0]=color.red();
                    rgb[1]=color.green();
                    rgb[2]=color.blue();
                    emit getColor(rgb);
                }
            }
        }
        break;

    case PERSPECTIVE:
        qDebug() << event->pos();
        break;
    }

    QGraphicsPixmapItem::mousePressEvent(event);
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (cursorFlag)
    {

    }

    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void PixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch (cursorFlag)
    {
    case COLOR_PICKER:
        if (!savedPxmap.isNull())
        {
            QImage image = savedPxmap.toImage();
            if (!image.isNull())
            {
                if (image.valid(event->pos().x(), event->pos().y()))
                {
                    QColor color = image.pixel(event->pos().x(), event->pos().y());
                    int rgb[3];
                    rgb[0]=color.red();
                    rgb[1]=color.green();
                    rgb[2]=color.blue();
                    emit getColor(rgb);
                }
            }
        }
        break;
    }

    QGraphicsPixmapItem::mouseMoveEvent(event);
}

void PixmapItem::perspective(const Mat &t)
{
    this->setTransform(QTransform(t.at<double>(0,0),t.at<double>(1,0),t.at<double>(2,0),t.at<double>(0,1),t.at<double>(1,1),t.at<double>(2,1),t.at<double>(0,2),t.at<double>(1,2),t.at<double>(2,2)));
}

void PixmapItem::savePixmap(QPixmap pixmap)
{
    savedPxmap = pixmap;
}


