#ifndef TOOLLAYERMANAGER_H
#define TOOLLAYERMANAGER_H

#include <QDockWidget>

class Picture;
class QStandardItemModel;
class QStandardItem;

namespace Ui {
class ToolLayerManager;
}

class ToolLayerManager : public QDockWidget
{
    Q_OBJECT

public:
    explicit ToolLayerManager(QWidget *parent = 0);

    ~ToolLayerManager();

    void switchModel(Picture *pic);

public slots:
    void checkStateChanged(QStandardItem *item);

    void setCurrentIndex(int index);

private slots:
    void layer_proc();

    void select_changed(const QModelIndex &index);

    void onDoubleClicked(const QModelIndex &index);

private:
    Ui::ToolLayerManager *ui;

    QStandardItemModel *model;

    Picture *m_pic;
public:
    enum
    {
        LAYER_SELECTED,
        LAYER_ADD,
        LAYER_REMOVE,
        LAYER_COPY,
        LAYER_MERGE,
        LAYER_UP,
        LAYER_DOWN,
    };
};

#endif // TOOLLAYERMANAGER_H
