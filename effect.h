#ifndef EFFECT_H
#define EFFECT_H

#include "Types.h"

#include <opencv2/core/core.hpp>

class QString;

class Effect
{
public:
    Effect(void);
    ~Effect(void);

public:
    //旋转-平移-缩放
    void RotateZoom(_RotateZoom *rotate,cv::Mat &dst);
    //去色
    void GrayScale(cv::Mat &dst);
    //反相
    void InvertColor(cv::Mat &dst);
    //阈值
    void Threshold(int value, int max,cv::Mat &dst);
    //色彩平衡
    void ColorBalance(_ColorBalance *color_balance,cv::Mat &dst);
    //亮度-对比度
    void BrightnessContrast(int levelB, int levelC,cv::Mat &dst);
    //单色化
    void Colorize(int H,int S,int L,cv::Mat &dst);
    //色调分离
    void Posterize(int level,cv::Mat &dst);
    //色调-亮度-饱和度
    void HueSaturation(_HueSaturation *hs,cv::Mat &dst);
    //颜色增强
    void ColorEnhance(cv::Mat &dst);
    //白平衡
    void WhiteBalance(cv::Mat &dst);
    //色调均化
    void Equalize(cv::Mat &dst);
    //色阶
    void Levels(_Levels *levels,cv::Mat &dst);
    //曲线
    void Curve(_Curve *curve,cv::Mat &dst);
    //径向模糊
    void ZoomBlur(_ZoomBlur *blur,cv::Mat &dst);
    //旋转模糊
    void RadialBlur(_RadialBlur *blur,cv::Mat &dst);
    //高斯模糊
    void GaussianBlur2(int x,int y,cv::Mat &dst);
    //运动模糊
    void MotionBlur(_MotionBlur *blur, cv::Mat &dst);
    //表面模糊
    void SurfaceBlur(int radius,int threshold,cv::Mat &dst);
    //马赛克
    void Mosaic(int x,int y,cv::Mat &dst);
    //Sobel边缘检测
    void SobelEdge(cv::Mat &dst);
    //Laplacian边缘检测
    void LaplacianEdge(cv::Mat &dst);
    //Laplacian边缘检测
    void CannyEdge(cv::Mat &dst);
    //添加纹理
    void FillPattern(const cv::Mat &tex, int alpha, cv::Mat &dst);
    //浮雕
    void Emboss(_Emboss *emboss,cv::Mat &dst);
    //去除红眼
    void RedEyeRemoval(int threshold, cv::Mat &dst);
    //锐化
    void Sharpen(cv::Mat &dst);
    //虚光蒙版
    void UnsharpMask(_UnsharpMask *sharp, cv::Mat &dst);

    void setData(const cv::Mat &src,cv::Mat mask = cv::Mat());

    void reset();

    void replaceMask(const cv::Mat &src, cv::Mat &dst, cv::Mat mask = cv::Mat());
private:

    void look_up_table(const cv::Mat &src, const cv::Mat &lut, cv::Mat &dst, cv::Mat mask = cv::Mat());

private:
    cv::Mat m_src;
    cv::Mat m_mask;
};

class Histogram
{
public:
    Histogram(const cv::Mat &src);

    Histogram(const cv::Mat &src, cv::Mat dst[]);

    ~Histogram();

    void calculate(int min, int max, int channel=0);

    int getCount();

    int getTotal();

    double getMean();

    double getDev();

    int getMedian();

    double getPercent();
private:
    cv::Mat hist[4];// gray_hist,r_hist, g_hist, b_hist;
    double x,s,percent;
    int median,count,total;
};



#endif // EFFECT_H
