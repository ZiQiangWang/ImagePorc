#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "Types.h"
#include <QPointF>

#include <opencv2/core/core.hpp>

class Layer;
class Picture;
/******************************************************************************************************************/
//图像内容操作命令
class CommandImage: public QUndoCommand
{
public:
    explicit CommandImage(Picture *pic,const QString &name, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
    cv::Mat m_savedImg;
};

/******************************************************************************************************************/
//图层操作命令
class CommandAddLayer: public QUndoCommand
{
public:
    explicit CommandAddLayer(Picture *pic, int index, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
};

class CommandRemoveLayer: public QUndoCommand
{
public:
    explicit CommandRemoveLayer(Picture *pic, int index, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
    Layer * m_savedLayer;
};

class CommandCopyLayer: public QUndoCommand
{
public:
    explicit CommandCopyLayer(Picture *pic, int index, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
    Layer * m_savedLayer;
};

class CommandMergeLayer: public QUndoCommand
{
public:
    explicit CommandMergeLayer(Picture *pic, int index, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
    Layer * m_savedLayer;
    cv::Mat m_savedImg;
};

class CommandMoveUpLayer: public QUndoCommand
{
public:
    explicit CommandMoveUpLayer(Picture *pic, int index, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
};

class CommandMoveDownLayer: public QUndoCommand
{
public:
    explicit CommandMoveDownLayer(Picture *pic, int index, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
};

class CommandAttributeLayer: public QUndoCommand
{
public:
    explicit CommandAttributeLayer(Picture *pic, int index,const QString &name,int type,int transparent,bool checked, QUndoCommand *parent=0);

    void undo();
    void redo();

private:
    Picture *m_pic;
    int m_index;
    QString m_name;
    int m_type,m_transparent;
    bool m_checkState;

    QString m_oldName;
    int m_oldType,m_oldTransparent;
    bool m_oldCheckState;
};




#endif // COMMANDS_H


