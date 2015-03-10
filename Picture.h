#ifndef PICTURE_H
#define PICTURE_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <QPixmap>
#include "Types.h"

class Layer;
class QUndoStack;
class PixmapItem;
class Effect;
class QStandardItemModel;

namespace cv {
    class Mat;
}
class Picture : public QObject
{
    Q_OBJECT
public:
    explicit Picture(PixmapItem *item,const QString &fileName, QObject *parent = 0);

    ~Picture();
    //初始化载入图片
    void loadImage(const QString &fileName);

//*************************
// 图层操作函数
//*************************
    //选中图层
    void selectLayer(int index);
    //添加图层
    void addLayer(int index);
    //插入一个现有图层
    void insertLayer(Layer *layer,int index);
    //删除图层
    void removeLayer(int index);
    //复制图层
    void copyLayer(int index);
    //上移当前图层
    void moveUpLayer(int index);
    //下移当前图层
    void moveDownLayer(int index);
    //向下合并图层
    void mergeLayer(int index);
    //获取当前图层
    Layer *getCurrentLayer();
    //获取当前图层
    Layer *getIndexLayer(int index);
    //对图层的操作的响应
    void LayerProc(int type,int index);

    QStandardItemModel *Model() const;

public:
    void push_back_record(int index);

    void pop_up_record(int index);

    int getCurrentIndex() const;

    int getlayerCount() const;

    QUndoStack *getUndoStack() const;

    int getWidth() const;

    int getHeight() const;

public slots:
    void PushBackRecord();

    void PopUpRecord();
signals:

    void sendMat(const cv::Mat &result);

    void setCurrentItem(int index);

public slots:
    //设置选择区域掩码
    void setMask(const cv::Mat &mask);
    //响应选择图层的变化
    void onIndexChanged(int index);
    //图层混合
    void MergeMethod();

//*****************************
//对图像的操作
//*****************************
public slots:
    //水平翻转
    void FlipHorizontal();
    //垂直翻转
    void FlipVertical();
    //顺时针旋转90度
    void ClockwiseRotate90();
    //逆时针旋转90度
    void AntiClockwiseRotate90();
    //旋转180度
    void Rotate180();
    //旋转-缩放-平移
    void RotateZoom(_RotateZoom *rotate);
    //透视变换
    void perspective(cv::Mat m);
    //去色
    void GrayScale();
    //反相
    void InvertColor();
    //阈值
    void Threshold(int min, int max);
    //色彩平衡
    void ColorBalance(_ColorBalance *color_balance);
    //亮度-对比度
    void BrightnessContrast(int levelB,int levelC);
    //单色化
    void Colorize(int H, int S, int L);
    //色调分离
    void Posterize(int level);
    //色调-亮度-对比度
    void HueSaturation(_HueSaturation *hs);
    //色调均化
    void Equalize();
    //白平衡
    void WhiteBalance();
    //颜色增强
    void ColorEnhance();
    //色阶
    void Levels(_Levels *levels);
    //曲线
    void Curve(_Curve *curve);
    //径向模糊
    void ZoomBlur(_ZoomBlur *blur);
    //旋转模糊
    void RadialBlur(_RadialBlur *blur);
    //高斯模糊
    void GaussianBlur2(int x,int y);
    //运动模糊
    void MotionBlur(_MotionBlur *blur);
    //表面模糊
    void SurfaceBlur(int radius,int threshold);
    //马赛克
    void Mosaic(int x,int y);
    //Sobel边缘检测
    void SobelEdge();
    //Laplacian边缘检测
    void LaplacianEdge();
    //Canny边缘检测
    void CannyEdge();
    //添加画布纹理
    void Canvas();
    //浮雕
    void Emboss(_Emboss *emboss);
    //去红眼
    void RedEyeRemoval(int threshold);
    //锐化
    void Sharpen();
    //虚光蒙版
    void UnsharpMask(_UnsharpMask *sharp);

private:
    QList<Layer*> m_layerList;      //存放所有图层类
    int m_currIndex;                //当前选中的图层索引
    QUndoStack *m_undoStack;
    PixmapItem *m_item;             //显示图像的控件
    cv::Mat m_showImg;              //图层融合结果
    cv::Mat m_mask;
    cv::Mat m_bk;
    std::vector<cv::Mat> m_recordImg;
    Effect *m_effect;
    QStandardItemModel *m_model;
};

#endif // PICTURE_H
