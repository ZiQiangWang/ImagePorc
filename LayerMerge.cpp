#include "LayerMerge.h"
#include "Utils.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
#include <QDebug>
LayerMerge::LayerMerge(Size size)
{
    Image_up = Mat(size,CV_32FC3);
    Image_down = Mat(size,CV_32FC3);
}

funcP LayerMerge::getFuncPointer(int type)
{
    switch(type)
    {
    case DARKEN:         //变暗
        return Darken;
    case MULTIPLY:       //正片叠底
        return Multiply;
    case COLOR_BURN:     //颜色加深
        return Color_Burn;
    case LINEAR_BURN:    //线性加深
        return Linear_Burn;

    case LIGHTEN:        //变亮
        return Lighten;
    case SCREEN:         //滤色
        return Screen;
    case COLOR_DODGE:    //颜色减淡
        return Color_Dodge;
    case LINEAR_DODGE:    //线性减淡
        return Linear_Dodge;

    case OVERLAY:      //叠加
        return Overlay;
    case SOFT_LIGHTEN:   //柔光
        return Soft_Lighten;
    case STRONG_LIGHTEN: //强光
        return Strong_Lighten;
    case VIVID_LIGHTEN:  //亮光
        return Vivid_Lighten;
    case LINEAR_LIGHTEN: //线性光
        return Linear_Lighten;
    case PIN_LIGHTEN:    //点光
        return Pin_Lighten;
    case HARD_MIX:       //实色混合
        return Hard_Mix;

    case DIFFERENCE:     //差值
        return Difference;
    case EXCLUSION:       //排除
        return Exclusion;
    case SUBTRACT:
        return Subtract;
    case DIVIDE:
        return Divide;
    }
}

void LayerMerge::MergeMethod(const Mat &ImgUp, const Mat &ImgDown, Mat &mix, int type, int alpha)
{
    if(type == NORMAL)
    {
        mix = ImgUp.clone();
        if(alpha <255)
            Transparent(ImgUp,ImgDown,mix,alpha);
    }
    else if(type == DISSOLVE)
    {
        mix = ImgUp.clone();
        Mat Rand_mat(ImgUp.size(), CV_8UC1);
        cv::randu(Rand_mat,Scalar(0),Scalar(255));
        for(int row=0; row<ImgUp.rows; row++)
        {
            const uchar *pRand = Rand_mat.ptr<uchar>(row);
            const uchar *pDown = ImgDown.ptr<uchar>(row);
            uchar *pMix = mix.ptr<uchar>(row);
            for(int col=0; col<ImgUp.cols; col++)
            {
                if(pRand[col]>=alpha)
                {
                    for(int c=0; c<3; c++)
                    {
                        pMix[3*col+c] = pDown[3*col+c];
                    }
                }
            }
        }
    }
    else if(type >= HUE)
    {
        Mat up_hsv,down_hsv,mix_hsv;
        cvtColor(ImgUp,up_hsv,COLOR_BGR2HSV);
        cvtColor(ImgDown,down_hsv,COLOR_BGR2HSV);
        cv::vector<Mat> up_hsv_c,down_hsv_c,mix_hsv_c;
        split(up_hsv,up_hsv_c);
        split(down_hsv,down_hsv_c);

        switch(type)
        {
        case HUE:
            down_hsv_c[0]=up_hsv_c[0];
            break;
        case SATURATION:
            down_hsv_c[1] =up_hsv_c[1];
            break;
        case COLOR:
            down_hsv_c[0] = up_hsv_c[0];
            down_hsv_c[1] = up_hsv_c[1];
            break;
        case LUMINOSITY:
            down_hsv_c[2]=up_hsv_c[2];
            break;
        }

        merge(down_hsv_c,mix_hsv);
        cvtColor(mix_hsv,mix,COLOR_HSV2BGR);


    }
    else if(type > DISSOLVE && type < HUE )
    {
        ImgUp.convertTo(Image_up,CV_32FC3);
        ImgDown.convertTo(Image_down,CV_32FC3);
        Image_up=Image_up/255;
        Image_down=Image_down/255;
        Mat Image_mix = Image_up.clone();
        float a=0;
        float b=0;

        funcP ptr = getFuncPointer(type);
        for(int row=0; row<ImgUp.rows; row++)
        {
            const float *pUp = Image_up.ptr<float>(row);
            const float *pDown = Image_down.ptr<float>(row);
            float *pMix = Image_mix.ptr<float>(row);
            for(int col=0; col<ImgUp.cols; col++)
            {
                for(int c=0; c<3; c++)
                {
                    a=pUp[3*col+c];
                    b=pDown[3*col+c];
                    pMix[3*col+c] = ptr(a,b);
                }
            }
        }

        if(alpha <255)
            Transparent(Image_mix,Image_down,Image_mix,alpha);
//        if(alphaDown <255)
//            Transparent(Image_mix,Image_up,Image_mix,alphaDown);
        Image_mix = Image_mix*255;
        mix = Mat(Image_mix.size(),CV_8UC3);
        Image_mix.convertTo(mix,CV_8UC3);
    }
}

// Transparent 不透明度
void LayerMerge::Transparent(const Mat& src1, const Mat& src2, Mat& dst, int alpha)
{
    float k = (float)alpha/255;
    dst=k*src1+(1-k)*src2;
}

float LayerMerge::Multiply(float a, float b)
{
    return a*b;
}

float LayerMerge::Color_Burn(float a, float b)
{
    return a==0 ? a : 1-(1-b)/a;
}

float LayerMerge::Color_Dodge(float a, float b)
{
    return a==1 ? a : Min(1.0f,b/(1-a));
}

float LayerMerge::Linear_Burn(float a, float b)
{
    return Max(a+b-1,0.0f);;
}

float LayerMerge::Linear_Dodge(float a, float b)
{
    return Min(a+b,1.0f);
}

float LayerMerge::Lighten(float a, float b)
{
    return Max(a,b);
}

float LayerMerge::Darken(float a, float b)
{
    return Min(a,b);
}

float LayerMerge::Screen(float a, float b)
{
    return 1-(1-a)*(1-b);
}

float LayerMerge::Overlay(float a, float b)
{
    return b<=0.5 ? 2*a*b : 1-2*(1-a)*(1-b);
}

float LayerMerge::Soft_Lighten(float a, float b)
{
    return a<=0.5 ? (2*a-1)*(b-b*b)+b : (2*a-1)*(sqrt(b)-b)+b;
}

float LayerMerge::Strong_Lighten(float a, float b)
{
    return a<=0.5 ? 2*a*b : 1-2*(1-a)*(1-b);
}

float LayerMerge::Vivid_Lighten(float a, float b)
{
    float result = 0.0f;
    if(a == 0.0 || a == 1.0)
        result = a;
    else
        result = a<=0.5 ? 1-(1-b)/(2*a) : b/(2*(1-a));
    return result;
}

float LayerMerge::Pin_Lighten(float a, float b)
{
    float result = 0.0f;
    if(b<=2*a-1)
         result=2*a-1;
    else if(b<=2*a)
         result=b;
    else
        result=2*a;
    return result;
}

float LayerMerge::Linear_Lighten(float a, float b)
{
    return b+2*a-1;
}

float LayerMerge::Hard_Mix(float a, float b)
{
    return a<1-b ? 0.0f : 1.0f;
}
float LayerMerge::Difference(float a, float b)
{
    return fabs(a-b);
}

float LayerMerge::Exclusion(float a, float b)
{
    return a+b-2*a*b;
}

float LayerMerge::Subtract(float a, float b)
{
    return std::max(b-a,0.0f);
}

float LayerMerge::Divide(float a, float b)
{
    return a==0 ? 1.0f : b/a;
}




