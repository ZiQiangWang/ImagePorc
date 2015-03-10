#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QGraphicsPixmapItem>

namespace cv {
    class Mat;
}

enum
{
    NORMAL,
    COLOR_PICKER,
    PERSPECTIVE
};

class PixmapItem :public QObject,public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    PixmapItem();
    PixmapItem(const QPixmap &pixmap);
    ~PixmapItem();

    void setCursorFlag(int flag);

    void savePixmap(QPixmap pixmap);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
public slots:
    void perspective(const cv::Mat &t);

signals:
    void itemMoved(const QPointF &movedFromPosition);
    void getColor(int *rgb);
private:
    int cursorFlag;
    QPixmap savedPxmap;
};

#endif // PIXMAPITEM_H
