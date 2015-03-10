#include "Picture.h"
#include "Layer.h"
#include "Utils.h"
#include "PixmapItem.h"
#include "Effect.h"
#include "Commands.h"
#include "ToolLayerManager.h"
#include "LayerMerge.h"
#include <QStandardItemModel>
#include <QDebug>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

Picture::Picture(PixmapItem *item, const QString &fileName, QObject *parent) :
    QObject(parent)
{
    m_item = item;
    loadImage(fileName);
    m_currIndex = 0;
    m_effect = new Effect();
    m_mask = Mat();

    m_model = new QStandardItemModel();

    Layer *layer= new Layer(m_showImg);
    layer->setItemName("背景");
    m_layerList.append(layer);
    m_model->insertRow(0,layer->getListItem());

    m_undoStack = new QUndoStack(this);
}

Picture::~Picture()
{
    for(int i=0;i<m_layerList.count();i++)
        SafeDelete(m_layerList[i]);
    SafeDelete(m_effect);
    SafeDelete(m_model);
    SafeDelete(m_undoStack);
}

/**
 * @brief 初始化，载入图片
 */
void Picture::loadImage(const QString &fileName)
{
    m_showImg = imread(ws2s(fileName.toStdWString()));

    double alpha = 1.0;
    int width = m_showImg.cols;
    int height = m_showImg.rows;
    if(width>800 || height>600)
    {
        alpha = Max((double)width/800.0,(double)height/600.0);
        width /= alpha;
        height /= alpha;
    }

    if(alpha!=1)
        resize(m_showImg,m_showImg,Size(width,height));

    //绘制背景
    m_bk = Mat(m_showImg.size(),m_showImg.type(),Scalar(255,255,255));
    Mat roi,roi_gray;
    for (int row=0; row<m_bk.rows; row += 8)
    {
        int start = (row/8)%2 ? 0 : 8;
        for (int col=start; col<m_bk.cols; col += 16)
        {
            Rect rect(col,row,Min(8,width - col),Min(8,height-row));
            roi = m_bk(rect);
            roi_gray = Mat(roi.size(),roi.type(),Scalar(150,150,150));
            addWeighted(roi,0,roi_gray,1,0,roi);
        }
    }
    m_item->setPixmap(Mat2Pixmap(m_showImg));
}

QStandardItemModel *Picture::Model() const
{
    return m_model;
}


int Picture::getWidth() const
{
    return m_showImg.cols;
}

int Picture::getHeight() const
{
    return m_showImg.rows;
}

/**
 * @brief 响应图层选择变化
 */
void Picture::onIndexChanged(int index)
{
    m_currIndex = index;
}

/**
 * @brief 设置选择区域
 */
void Picture::setMask(const cv::Mat &mask)
{
    m_mask = mask;
}

/**
 * @brief 获得当前图层
 */
Layer *Picture::getCurrentLayer()
{
    return m_layerList[m_currIndex];
}

/**
 * @brief 获得当前图层
 */
Layer *Picture::getIndexLayer(int index)
{
    if(index <0 || index >= m_layerList.count())
        return new Layer(Mat());
    return m_layerList[index];
}

void Picture::push_back_record(int index)
{
    Mat workImg = m_layerList[index]->getCurrentImage();
    if(workImg.data)
        m_recordImg.push_back(workImg.clone());
}

void Picture::pop_up_record(int index)
{
    m_layerList[index]->setCurrentImage(m_recordImg.back());
    m_recordImg.pop_back();
}

void Picture::PushBackRecord()
{
    this->push_back_record(m_currIndex);
}

void Picture::PopUpRecord()
{
    this->pop_up_record(m_currIndex);
}

QUndoStack *Picture::getUndoStack() const
{
    return m_undoStack;
}
int Picture::getCurrentIndex() const
{
    return m_currIndex;
}

int Picture::getlayerCount() const
{
    return m_layerList.count();
}
/**
 * @brief 图层混合
 */
void Picture::MergeMethod()
{
    QVector<Layer*> visibleLayer;
    for(int i=m_layerList.count()-1;i>=0;i--)
    {
        if(m_layerList[i]->isVisible())
        {
            visibleLayer.append(m_layerList[i]);
        }
    }
    if(visibleLayer.count()==0)
    {
        m_showImg = m_bk.clone();
    }
    else if(visibleLayer.count() == 1)
    {
        LayerMerge *merg = new LayerMerge(m_showImg.size());
        merg->Transparent(visibleLayer[0]->getCurrentImage(),m_bk,m_showImg,visibleLayer[0]->getTransparent());
        delete merg;
    }
    else
    {
        LayerMerge *merg = new LayerMerge(m_showImg.size());
        m_showImg = visibleLayer[0]->getCurrentImage().clone();
        for(int i=1;i<visibleLayer.count();i++)
        {
            merg->MergeMethod(visibleLayer[i]->getCurrentImage(),m_showImg.clone(),m_showImg,visibleLayer[i]->getMergeType(),visibleLayer[i]->getTransparent());
        }
        merg->Transparent(m_showImg,m_bk,m_showImg,visibleLayer[0]->getTransparent());
        delete merg;
    }
    m_item->setPixmap(Mat2Pixmap(m_showImg));
}

/**
 * @brief 对图层的操作的响应
 * @param type
 * @param index
 */
void Picture::LayerProc(int type, int index)
{
    QUndoCommand *command;
    switch(type)
    {
    case ToolLayerManager::LAYER_SELECTED:
        selectLayer(index);
        break;
    case ToolLayerManager::LAYER_ADD:
        command = new CommandAddLayer(this,index);
        m_undoStack->push(command);
        break;
    case ToolLayerManager::LAYER_REMOVE:
        command = new CommandRemoveLayer(this,index);
        m_undoStack->push(command);
        break;
    case ToolLayerManager::LAYER_COPY:
        command = new CommandCopyLayer(this,index);
        m_undoStack->push(command);
        break;
    case ToolLayerManager::LAYER_UP:
        command = new CommandMoveUpLayer(this,index);
        m_undoStack->push(command);
        break;
    case ToolLayerManager::LAYER_DOWN:
        command = new CommandMoveDownLayer(this,index);
        m_undoStack->push(command);
        break;
    case ToolLayerManager::LAYER_MERGE:
        command = new CommandMergeLayer(this,index);
        m_undoStack->push(command);
        break;
    }
}

/**
 * @brief 选择图层
 */
void Picture::selectLayer(int index)
{
    m_currIndex = index;
}

/**
 * @brief 添加图层
 */
void Picture::addLayer(int index)
{
    m_currIndex = index;
    Layer *layer = new Layer(m_bk);
    layer->setVisible(false);
    layer->setItemName(QString("图层%1").arg(m_layerList.count()));
    m_layerList.insert(m_currIndex,layer);
    m_model->insertRow(index,layer->getListItem());
    this->MergeMethod();
    emit setCurrentItem(m_currIndex);
}

void Picture::insertLayer(Layer *layer, int index)
{
    m_currIndex = index;
    m_layerList.insert(m_currIndex,layer);
    m_model->insertRow(index,layer->getListItem());
    this->MergeMethod();
    emit setCurrentItem(m_currIndex);
}

/**
 * @brief 删除当前图层
 */
void Picture::removeLayer(int index)
{
    int count = m_layerList.count();
    if(count > 1)
    {
        if(index == count-1)
            m_currIndex = index-1;
        else
            m_currIndex = index;
        m_model->removeRow(index);
        SafeDelete(m_layerList[index]);
        m_layerList.removeAt(index);
    }
    this->MergeMethod();
    emit setCurrentItem(m_currIndex);
}

/**
 * @brief 复制当前图层
 */
void Picture::copyLayer(int index)
{
    m_currIndex = index;
    Layer *layer = new Layer(m_layerList[index]);
    m_layerList.insert(m_currIndex,layer);

    m_model->insertRow(index,layer->getListItem());
    this->MergeMethod();
    emit setCurrentItem(m_currIndex);
}

/**
 * @brief 向下合并图层
 */
void Picture::mergeLayer(int index)
{
    if(index <m_layerList.count()-1)
    {
        m_currIndex = index;
        Mat workImg;
        LayerMerge *merg = new LayerMerge(m_showImg.size());
        merg->MergeMethod(m_layerList[index]->getCurrentImage(),m_layerList[index+1]->getCurrentImage(),workImg,m_layerList[index]->getMergeType(),m_layerList[index]->getTransparent());
        delete merg;

        m_model->removeRow(index);
        SafeDelete(m_layerList[index]);
        m_layerList.removeAt(index);

        m_layerList[index]->setCurrentImage(workImg);
        emit setCurrentItem(m_currIndex);
    }
}

/**
 * @brief 上移图层
 */
void Picture::moveUpLayer(int index)
{
    if(index >0)
    {
        m_currIndex = index-1;
        QStandardItem *item = m_model->takeItem(index);
        m_model->removeRow(index);
        m_model->insertRow(index-1,item);
        m_layerList.move(index,index-1);
        this->MergeMethod();
        emit setCurrentItem(m_currIndex);
    }
}

/**
 * @brief 下移图层
 */
void Picture::moveDownLayer(int index)
{
    if(index <m_layerList.count()-1)
    {
        m_currIndex = index+1;
        QStandardItem *item = m_model->takeItem(index);
        m_model->removeRow(index);
        m_model->insertRow(index+1,item);

        m_layerList.move(index,index+1);
        this->MergeMethod();
        emit setCurrentItem(m_currIndex);
    }
}



//---------------------------------------------------------//
// 变换菜单
//---------------------------------------------------------//

/**
 * @brief 水平翻转
 */
void Picture::FlipHorizontal()
{
    Mat workImg = m_layerList[m_currIndex]->getCurrentImage();

    this->PushBackRecord();
    flip(workImg,workImg,1);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-水平翻转");
    m_undoStack->push(command);
}

/**
 * @brief 垂直翻转
 */
void Picture::FlipVertical()
{
    Mat workImg = m_layerList[m_currIndex]->getCurrentImage();

    this->PushBackRecord();
    flip(workImg,workImg,0);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-垂直翻转");
    m_undoStack->push(command);
}

/**
 * @brief 顺时针旋转90度
 */
void Picture::ClockwiseRotate90()
{
    Mat workImg = m_layerList[m_currIndex]->getCurrentImage();

    this->PushBackRecord();
    transpose(workImg,workImg);
    flip(workImg,workImg,1);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-顺时针旋转90度");
    m_undoStack->push(command);
}

/**
 * @brief 逆时针旋转90度
 */
void Picture::AntiClockwiseRotate90()
{
    Mat workImg = m_layerList[m_currIndex]->getCurrentImage();

    this->PushBackRecord();
    flip(workImg,workImg,0);
    flip(workImg,workImg,1);
    transpose(workImg,workImg);
    flip(workImg,workImg,1);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-逆时针旋转90度");
    m_undoStack->push(command);
}

/**
 * @brief 旋转180度
 */
void Picture::Rotate180()
{
    Mat workImg = m_layerList[m_currIndex]->getCurrentImage();

    this->PushBackRecord();
    flip(workImg,workImg,0);
    flip(workImg,workImg,1);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-旋转180度");
    m_undoStack->push(command);
}

/**
 * @brief 旋转-偏移-缩放
 */
void Picture::RotateZoom(_RotateZoom *rotate)
{
    m_effect->setData(m_recordImg.back());
    Mat workImg;
    m_effect->RotateZoom(rotate,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 透视变换
 */
void Picture::perspective(Mat m)
{
    Mat back = m_recordImg.back();
    Mat workImg;
    warpPerspective(back,workImg,m,back.size(),INTER_LINEAR,BORDER_CONSTANT,Scalar(255,255,255));
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}



////void Picture::trans(_RotateZoom *rotate,int x, int y, double &sx, double &sy, double &sz)
////{
////    double delta = CV_PI/180;
////    double rb = rotate->angle*delta;
////    double ra = rotate->per_angle*delta;
////    double r = 0 - rotate->perspective*delta;
////    double crb = cos(rb);
////    double cr = cos(r);
////    double cra = cos(ra);
////    double srb = sin(rb);
////    double sr = sin(r);
////    double sra = sin(ra);

////    double ox = x, oy = y, oz = 0;

////    sx = (ox * crb + oy * srb) / cr;
////    sy = -ox * srb + oy * crb;

////    sz = sx * sr;
////    sx = sx / cr;
////    ox = sx; oy = sy; oz = sz;

////    sx =  ox * cra + oy * sra;
////    sy = -ox * sra + oy * cra;
////}

//---------------------------------------------------------//
// 颜色菜单
//---------------------------------------------------------//
/**
 * @brief 去色
 */
void Picture::GrayScale()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->GrayScale(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
    QUndoCommand *command = new CommandImage(this,"变换-去色");
    m_undoStack->push(command);
}

/**
 * @brief 反色
 */
void Picture::InvertColor()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->InvertColor(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-反相");
    m_undoStack->push(command);
}

/**
 * @brief 阈值
 */
void Picture::Threshold(int min, int max)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Threshold(min,max,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 色彩平衡
 */
void Picture::ColorBalance(_ColorBalance *color_balance)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->ColorBalance(color_balance,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 亮度-对比度
 */
void Picture::BrightnessContrast(int levelB, int levelC)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->BrightnessContrast(levelB,levelC,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 单色化
 */
void Picture::Colorize(int H, int S, int L)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Colorize(H,S,L,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 色调分离
 */
void Picture::Posterize(int level)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Posterize(level,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 色调-饱和度-亮度
 */
void Picture::HueSaturation(_HueSaturation *hs)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->HueSaturation(hs,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 色调均化
 */
void Picture::Equalize()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Equalize(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-色调均化");
    m_undoStack->push(command);
}

/**
 * @brief 白平衡
 */
void Picture::WhiteBalance()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->WhiteBalance(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-白平衡");
    m_undoStack->push(command);
}

/**
 * @brief 颜色增强
 */
void Picture::ColorEnhance()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->ColorEnhance(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"变换-颜色增强");
    m_undoStack->push(command);
}

/**
 * @brief 色阶
 */
void Picture::Levels(_Levels *levels)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Levels(levels,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 曲线
 */
void Picture::Curve(_Curve *curve)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Curve(curve,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

//---------------------------------------------------------//
// 滤镜菜单
//---------------------------------------------------------//
/**
 * @brief 径向模糊
 */
void Picture::ZoomBlur(_ZoomBlur *blur)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->ZoomBlur(blur,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

void Picture::RadialBlur(_RadialBlur *blur)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->RadialBlur(blur,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 高斯模糊
 */
void Picture::GaussianBlur2(int x, int y)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->GaussianBlur2(x,y,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 运动模糊
 */
void Picture::MotionBlur( _MotionBlur *blur)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->MotionBlur(blur,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 表面模糊
 */
void Picture::SurfaceBlur(int radius, int threshold)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->SurfaceBlur(radius,threshold,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 马赛克
 */
void Picture::Mosaic(int x, int y)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Mosaic(x,y,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief Sobel边缘检测
 */
void Picture::SobelEdge()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->SobelEdge(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"滤镜-Sobel边缘检测");
    m_undoStack->push(command);
}

/**
 * @brief Laplacian边缘检测
 */
void Picture::LaplacianEdge()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->LaplacianEdge(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"滤镜-Laplacian边缘检测");
    m_undoStack->push(command);
}

/**
 * @brief Canny边缘检测
 */
void Picture::CannyEdge()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->CannyEdge(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"滤镜-Canny边缘检测");
    m_undoStack->push(command);
}

/**
 * @brief 添加画布纹理
 */
void Picture::Canvas()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    Mat tex = imread("Texture/tex_canvas.png");
    m_effect->FillPattern(tex,255,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"滤镜-画布");
    m_undoStack->push(command);
}

/**
 * @brief 浮雕滤镜
 */
void Picture::Emboss(_Emboss *emboss)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Emboss(emboss,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 去除红眼
 */
void Picture::RedEyeRemoval(int threshold)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->RedEyeRemoval(threshold,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}

/**
 * @brief 锐化
 */
void Picture::Sharpen()
{
    this->PushBackRecord();
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->Sharpen(workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();

    QUndoCommand *command = new CommandImage(this,"滤镜-锐化");
    m_undoStack->push(command);
}

/**
 * @brief 虚光蒙版
 */
void Picture::UnsharpMask(_UnsharpMask *sharp)
{
    m_effect->setData(m_recordImg.back(),m_mask);
    Mat workImg;
    m_effect->UnsharpMask(sharp,workImg);
    m_layerList[m_currIndex]->setCurrentImage(workImg);
    this->MergeMethod();
}


