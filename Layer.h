#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include "Types.h"

#include <opencv2/core/core.hpp>

class QStandardItem;
class Layer : public QObject
{
    Q_OBJECT
public:

    explicit Layer(const cv::Mat &img,QObject *parent = 0);

    explicit Layer(const Layer *layer);

    ~Layer();

    void setItemName(const QString &name);

    QString getItemName() const;

    QStandardItem *getListItem() const;

    cv::Mat getCurrentImage();

    void setCurrentImage(const cv::Mat &img);

    void setTransparent(int value);

    int getTransparent() const;

    void setMergeType(int type);

    int getMergeType() const;

    void setVisible(bool checked);

    bool isVisible() const;


private:
    cv::Mat m_workImg;
    int m_transparent;
    int m_mergeType;
    bool m_isVisible;
    QStandardItem *m_listItem;
};

#endif // LAYER_H
