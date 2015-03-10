#include "GraphicsView.h"
#include <QWheelEvent>
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <iostream>
#include <QDebug>
#include "PixmapItem.h"
#include "Utils.h"
#include <QTime>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setBackgroundBrush(QBrush(Qt::gray));
    selector = new ShapeItem;
    m_scene = new QGraphicsScene(this);
    m_item = new PixmapItem;
    this->setScene(m_scene);
    m_scene->addItem(m_item);
    this->setRenderHint(QPainter::Antialiasing);
    this->setMouseTracking(true);
}

GraphicsView::~GraphicsView()
{
    SafeDelete(m_item);
    SafeDelete(m_scene);
    SafeDelete(selector);
}

void GraphicsView::setItem(PixmapItem *item)
{
    m_item = item;
}

void GraphicsView::setPixItem(const QPixmap &pixmap)
{
    m_item->setPixmap(pixmap);
}

PixmapItem *GraphicsView::PixItem() const
{
    return m_item;
}

void GraphicsView::setToolFlag(int type)
{
    selector->type = type;

    if(selector->type == SELECTOR_NONE)
    {
        selector->isExist = false;
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        emit getSelection(Mat());
    }
    else
    {
        selector->isExist = true;
        this->setDragMode(QGraphicsView::NoDrag);
    }
    if(selector->rect != 0)
    {
        m_scene->removeItem(selector->rect);
        selector->rect = 0;
    }
    if(selector->ellipse != 0)
    {
        m_scene->removeItem(selector->ellipse);
        selector->ellipse = 0;
    }
    if(selector->path != 0)
    {
        m_scene->removeItem(selector->path);
        selector->path = 0;
    }
}

int GraphicsView::getToolFlag()
{
    return selector->type;
}


void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if(QApplication::keyboardModifiers()==Qt::ControlModifier)
    {
        int numDegrees = event->delta()/ 8;
        if (numDegrees)
        {
            int numSteps = numDegrees / 15;
            emit getScale(numSteps*20);
        }
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    orig = event->pos();
    if(selector->isExist)
    {
        switch (selector->type)
        {
        case SELECTOR_NONE:
            break;
        case SELECTOR_RECT:
            if(selector->rect!=0)
            {
                m_scene->removeItem(selector->rect);
                selector->rect = 0;
            }
            selector->rect = m_scene->addRect(orig.x(),orig.y(),0,0);
            selector->rect->setPen(QPen(Qt::white));
            selector->rect->setBrush(QBrush(QColor(0,0,255,100)));
            break;
        case SELECTOR_ELLIPSE:
            if(selector->ellipse!=0)
            {
                m_scene->removeItem(selector->ellipse);
                selector->ellipse = 0;
            }
            selector->ellipse = m_scene->addEllipse(QRectF(orig.x(),orig.y(),0,0));
            selector->ellipse->setPen(QPen(Qt::white));
            selector->ellipse->setBrush(QBrush(QColor(0,0,255,100)));
            break;
        case SELECTOR_PATH:
            if(selector->path!=0)
            {
                m_scene->removeItem(selector->path);
                selector->path = 0;
                path = QPainterPath();
            }
            QPointF point = this->mapToScene(orig);
            path.moveTo(point.toPoint());
            selector->path = m_scene->addPath(path);
            selector->path->setPen(QPen(Qt::white));
            selector->path->setBrush(QBrush(QColor(0,0,255,100)));
            break;
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = this->mapToScene(event->x(),event->y());
    emit getPosition(QString("%1,%2").arg((int)pos.x()).arg((int)pos.y()));

    if(event->buttons() & Qt::LeftButton)
    {
        if(selector->isExist)
        {
            QPoint topleft,bottomright;
            int x = event->x();
            int y = event->y();

            int ox = orig.x();
            int oy = orig.y();
            topleft.setX(min(x,ox));
            topleft.setY(min(y,oy));

            bottomright.setX(max(x,ox));
            bottomright.setY(max(y,oy));
            QRectF rect(this->mapToScene(topleft),this->mapToScene(bottomright));

            switch (selector->type)
            {
            case SELECTOR_RECT:
                selector->rect->setRect(rect);
                break;
            case SELECTOR_ELLIPSE:
                selector->ellipse->setRect(rect);
                break;
            case SELECTOR_PATH:
                QPointF point = this->mapToScene(x,y);
                if((point - path.currentPosition()).manhattanLength()>10)
                {
                    path.lineTo(this->mapToScene(x,y));
                    selector->path->setPath(path);
                }
                break;
            }
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(selector->isExist)
    {
        int width = m_item->pixmap().width();
        int height = m_item->pixmap().height();
        Mat mask = Mat::zeros(height,width,CV_8UC1);
        switch (selector->type)
        {
        case SELECTOR_RECT:
            if(selector->rect->rect().width()>10 && selector->rect->rect().height()>10)
            {
                QPolygonF poly;
                QRectF rect;
                poly = selector->rect->mapToItem(m_item,selector->rect->rect());
                rect = QRectF(poly[0],poly[2]);

                for(int i=0;i<height;i++)
                {
                    uchar *pMsk = mask.ptr<uchar>(i);
                    for(int j=0;j<width;j++)
                    {
                        if(rect.contains(QPointF(j,i)))
                            pMsk[j] = 1;
                    }
                }
                selector->rect->setBrush(QBrush(QColor(255,255,255,25)));
                emit getSelection(mask);
            }
            else
            {
                m_scene->removeItem(selector->rect);
                selector->rect = 0;
                path = QPainterPath();
                emit getSelection(Mat());
            }
            break;
        case SELECTOR_ELLIPSE:
            if(selector->ellipse->rect().width()>10 && selector->ellipse->rect().height()>10)
            {
                QPainterPath path = selector->ellipse->mapToItem(m_item,selector->ellipse->shape());

                for(int i=0;i<height;i++)
                {
                    uchar *pMsk = mask.ptr<uchar>(i);
                    for(int j=0;j<width;j++)
                    {
                        if(path.contains(QPointF(j,i)))
                            pMsk[j] = 1;
                    }
                }
                selector->ellipse->setBrush(QBrush(QColor(255,255,255,25)));
                emit getSelection(mask);
            }
            else
            {
                m_scene->removeItem(selector->ellipse);
                selector->ellipse = 0;
                path = QPainterPath();
                emit getSelection(Mat());
            }
            break;
        case SELECTOR_PATH:
            path.closeSubpath();
            selector->path->setPath(path);

            if(selector->path->boundingRect().width()>10 && selector->path->boundingRect().height()>10)
            {
                QPainterPath shape = selector->path->mapToItem(m_item,selector->path->shape());

                vector<vector<Point> > contours;
                vector<Point> contours_item;
                for(int i=0;i<shape.elementCount();i++)
                {
                    contours_item.push_back(Point(shape.elementAt(i).x,shape.elementAt(i).y));
                }
                contours.push_back(contours_item);
                drawContours(mask,contours,-1,Scalar(1),-1);

                selector->path->setBrush(QBrush(QColor(255,255,255,25)));
                emit getSelection(mask);
            }
            else
            {
                m_scene->removeItem(selector->path);
                selector->path = 0;
                path = QPainterPath();
                emit getSelection(Mat());
            }
            break;
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}
