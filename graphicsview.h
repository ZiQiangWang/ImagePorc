#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
namespace cv {
    class Mat;
}
class ShapeItem
{

public:
    bool isExist;
    //bool isMovable;
    int type;
    QGraphicsRectItem *rect;
    QGraphicsEllipseItem *ellipse;
    QGraphicsPathItem *path;
    ShapeItem():isExist(false),type(0),rect(0),ellipse(0),path(0){}
};

class PixmapItem;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:

    explicit GraphicsView(QWidget *parent = 0);

    ~GraphicsView();

    void setToolFlag(int type);

    int getToolFlag();

    void setItem(PixmapItem *item);

    void setPixItem(const QPixmap &pixmap);

    PixmapItem *PixItem() const;

protected:
    void wheelEvent(QWheelEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);
signals:

    void getSelection(const cv::Mat &mask);

    void getPosition(QString pos);

    void getScale(int value);

public slots:

private:
    QPoint orig;
    ShapeItem *selector;
    PixmapItem *m_item;
    QGraphicsScene *m_scene;
    QPainterPath path;
public:
    enum
    {
        SELECTOR_NONE,
        SELECTOR_RECT,
        SELECTOR_ELLIPSE,
        SELECTOR_PATH,
    };
};

#endif // GRAPHICSVIEW_H
