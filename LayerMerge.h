#ifndef LAYERMERGE_H
#define LAYERMERGE_H

#include <opencv2/core/core.hpp>
using namespace cv;
class LayerMerge;
typedef float(*funcP)(float,float);

class LayerMerge
{
public:
    LayerMerge(cv::Size size);

    void MergeMethod(const cv::Mat &ImgUp, const cv::Mat &ImgDown, cv::Mat &mix, int type, int alpha = 255);

    void Transparent(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst, int alpha);

private:
    funcP getFuncPointer(int type);

    static float Multiply(float a, float b);

    static float Color_Burn(float a, float b);

    static float Color_Dodge(float a, float b);

    static float Linear_Burn(float a, float b);

    static float Linear_Dodge(float a, float b);

    static float Lighten(float a, float b);

    static float Darken(float a, float b);

    static float Screen(float a, float b);

    static float Overlay(float a, float b);

    static float Soft_Lighten(float a, float b);

    static float Strong_Lighten(float a, float b);

    static float Vivid_Lighten(float a, float b);

    static float Pin_Lighten(float a, float b);

    static float Linear_Lighten(float a, float b);

    static float Hard_Mix(float a, float b);

    static float Difference(float a, float b);

    static float Exclusion(float a, float b);

    static float Subtract(float a, float b);

    static float Divide(float a, float b);

private:
     cv::Mat Image_up,Image_down;

public:
     enum
     {
         NORMAL,         //正常
         DISSOLVE,       //溶解

         DARKEN,         //变暗
         MULTIPLY,       //正片叠底
         COLOR_BURN,     //颜色加深
         LINEAR_BURN,    //线性加深

         LIGHTEN,        //变亮
         SCREEN,         //滤色
         COLOR_DODGE,    //颜色减淡
         LINEAR_DODGE,   //线性减淡

         OVERLAY,      //叠加
         SOFT_LIGHTEN,   //柔光
         STRONG_LIGHTEN, //强光
         VIVID_LIGHTEN,  //亮光
         LINEAR_LIGHTEN, //线性光
         PIN_LIGHTEN,    //点光
         HARD_MIX,       //实色混合

         DIFFERENCE,     //差值
         EXCLUSION,      //排除
         SUBTRACT,       //减去
         DIVIDE,         //划分

         HUE,            //色相
         SATURATION,	 //饱和度
         COLOR,          //颜色
         LUMINOSITY,	 //亮度
     };
};

#endif // LAYERMERGE_H
