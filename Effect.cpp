#include "Effect.h"
#include "Utils.h"
#include <QDebug>
#include <QColor>
#include <QString>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

Effect::Effect(void)
{
}

Effect::~Effect()
{
}

void Effect::setData(const Mat &src,Mat mask)
{
    m_src = src.clone();
    if(m_src.channels()==1)
        cvtColor(m_src,m_src,COLOR_GRAY2BGR);
    else if(m_src.channels()==2)
        cvtColor(m_src,m_src,COLOR_GRAY2BGRA);

    m_mask = mask;
    if(!m_mask.empty())
    {
        if(m_src.size() != m_mask.size())
        {
            qDebug() << "掩码和原图的尺寸应该相同";
            return;
        }
    }
}

void Effect::reset()
{
    m_src = Mat();
    m_mask = Mat();
}

void Effect::replaceMask(const Mat &src, Mat &dst, Mat mask)
{
    int width = src.cols;
    int height = src.rows;
    int channel = src.channels();
    int k_num = channel;
    if(channel == 2 || channel ==4)
        k_num--;

    for(int row=0;row < height;row++)
    {
        const uchar* srcP = src.ptr<uchar>(row);
        uchar* dstP = dst.ptr<uchar>(row);
        uchar* mskP = mask.ptr<uchar>(row);

        for(int col=0;col < width;col++)
        {
            for(int k = 0;k < k_num;k++)
            {
                if(mskP[col]==1)
                {
                    dstP[channel*col+k] = srcP[channel*col+k];
                }
            }
        }
    }
}

void Effect::look_up_table(const Mat &src, const Mat &lut, Mat &dst, Mat mask)
{
    if(mask.empty())
        mask = Mat::ones(src.size(),CV_8UC1);

    int width = src.cols;
    int height = src.rows;
    int channel = src.channels();
    int k_num = channel;
    if(channel == 2 || channel ==4)
        k_num--;

    dst = src.clone();

    Mat m_lut;
    if(lut.channels()==1)
        cvtColor(lut,m_lut,COLOR_GRAY2BGR);
    else
        m_lut = lut;
    const uchar* lutP = m_lut.data;
    for(int row=0;row < height;row++)
    {
        const uchar* srcP = src.ptr<uchar>(row);
        uchar* dstP = dst.ptr<uchar>(row);
        uchar* mskP = mask.ptr<uchar>(row);

        for(int col=0;col < width;col++)
        {
            for(int k = 0;k < k_num;k++)
            {
                if(mskP[col]==1)
                {
                    //dstP[channel*col+k] = lut.at<uchar>(srcP[channel*col+k]);

                    dstP[channel*col+k] = lutP[3*(srcP[channel*col+k])+k];
                }
            }
        }
    }
}

/**
 * @brief 旋转缩放
 */
void Effect::RotateZoom(_RotateZoom *rotate,Mat &dst)
{
    int width=m_src.cols, height=m_src.rows;
    //旋转后的新图尺寸
    Mat M = Mat( 2, 3, CV_32FC1 );
    M = getRotationMatrix2D(Point(width/2,height/2),rotate->angle,rotate->scale);
    M.at<double>(0,2) += rotate->x*width;
    M.at<double>(1,2) += rotate->y*height;
    warpAffine(m_src,dst,M,Size(width,height),INTER_LINEAR,BORDER_CONSTANT,Scalar(255,255,255));
}


/**
*  @brief 去色，也就是灰度图
*/
void Effect::GrayScale(Mat &dst)
{
    if(!m_src.data)
        return;

    dst = m_src.clone();

    int channel = m_src.channels();
    if(channel == 3)
    {
        cvtColor(m_src,m_src,CV_BGR2GRAY);
        cvtColor(m_src,m_src,COLOR_GRAY2BGR);
    }
    else if(channel == 4)
    {
        cvtColor(m_src,m_src,COLOR_BGRA2GRAY);
        cvtColor(m_src,m_src,COLOR_GRAY2BGRA);
    }
    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }

}

/**
*  @brief 反色算法
*  Y(i,j)=255-X(i,j)
*/
void Effect::InvertColor(cv::Mat &dst)
{
    if(!m_src.data)
        return;

    Mat lut(1,256,CV_8U);
    uchar *p = lut.data;
    for(int i = 0; i < 256; i++)
    {
        p[i] = 255-i;
    }

    if(m_mask.empty())
    {
        LUT(m_src,lut,dst);
    }
    else
    {
        look_up_table(m_src,lut,dst,m_mask);
    }
}

/**
*  @brief 阈值
*/
void Effect::Threshold(int value, int max, cv::Mat &dst)
{
    if(!m_src.data)
        return;
    dst = m_src.clone();
    if(m_src.channels()==3)
        cvtColor(m_src,m_src,COLOR_BGR2GRAY);

    Mat srcTemp;
    inRange(m_src,Scalar(value),Scalar(max),srcTemp);
    cvtColor(srcTemp,srcTemp,COLOR_GRAY2BGR);

    if(m_mask.empty())
        dst = srcTemp;
    else
    {
        replaceMask(srcTemp,dst,m_mask);
    }
}

/**
 *  @brief 颜色平衡
 */
void Effect::ColorBalance(_ColorBalance *color_balance,cv::Mat &dst)
{
    if(!m_src.data)
        return;
    double  highlights[256] = { 0 };
    double  midtones[256]   = { 0 };
    double  shadows[256]    = { 0 };

    const double a = 64, b = 85, scale = 1.785;
    for (int i = 0; i < 256; i++)
    {
        double low = Clamp ((i - b) / -a + 0.5, 0.0, 1.0) * scale;
        double mid = Clamp ((i - b) /  a + 0.5, 0.0, 1.0) * Clamp ((i + b - 255) / -a + 0.5, 0.0, 1.0) * scale;

        shadows[i]          = low;
        midtones[i]         = mid;
        highlights[255 - i] = low;
    }

    Mat m_lut = Mat(1,256,CV_8UC3);
    uchar *p = m_lut.data;

    for (int i = 0; i < 256; i++)
    {
        int r_n = i, g_n = i, b_n = i ;

        r_n += color_balance->cyan_red[0] * shadows[i];
        r_n += color_balance->cyan_red[1] * midtones[i];
        r_n += color_balance->cyan_red[2] * highlights[i];
        r_n = Clamp0255(r_n);

        g_n += color_balance->magenta_green[0] * shadows[i];
        g_n += color_balance->magenta_green[1] * midtones[i];
        g_n += color_balance->magenta_green[2] * highlights[i];
        g_n = Clamp0255(g_n);

        b_n += color_balance->yellow_blue[0] * shadows[i];
        b_n += color_balance->yellow_blue[1] * midtones[i];
        b_n += color_balance->yellow_blue[2] * highlights[i];
        b_n = Clamp0255(b_n);

        p[3*i]=b_n;
        p[3*i+1]=g_n;
        p[3*i+2]=r_n;
    }

    if(m_mask.empty())
        LUT(m_src,m_lut,dst);
    else
    {
        look_up_table(m_src,m_lut,dst,m_mask);
    }

    if(color_balance->preserve_luminosity)
    {
        Mat hls;
        std::vector<Mat> src_hls;
        std::vector<Mat> dst_hls;

        cvtColor(m_src,hls,COLOR_BGR2HLS);
        split(hls,src_hls);
        cvtColor(dst,hls,COLOR_BGR2HLS);
        split(hls,dst_hls);

        dst_hls[1]=src_hls[1];
        merge(dst_hls,dst);
        cvtColor(dst,dst,COLOR_HLS2BGR);
    }
}

/**
 *  @brief 亮度-对比度
 */
void Effect::BrightnessContrast(int levelB, int levelC, Mat &dst)
{
    if(!m_src.data)
        return;
    Mat m_lut1 = Mat(1,256,CV_8U);
    uchar *p1 = m_lut1.data;
    for(int i=0;i<256;i++)
    {
        if(levelB<=0)
        {
            p1[i]=Clamp0255(i+i*(double)levelB/254.0);
        }
        else
        {
            p1[i]=Clamp0255(i+(255-i)*(double)levelB/254.0);
        }

    }
    Mat m_lut2 = Mat(1,256,CV_8U);
    uchar *p2 = m_lut2.data;
    for(int i=0;i<256;i++)
    {
        double contrast = (double)levelC / 127.0;
        double value = (double)i / 255.0;
        double n =(value>0.5)?(1.0-value):value;

        if (n < 0)
            n = 0 ;

        if (contrast<0)
        {
            n=0.5*pow(2.0*n,1.0+contrast);
        }
        else
        {
            double power = 1.0/(1.0-contrast);
            n = 0.5*pow(2.0*n,power);
        }
        value = (value>0.5)?(1.0-n):n;
        p2[i]=Clamp0255((int)(value*255));
    }

    if(m_mask.empty())
    {
        LUT(m_src,m_lut1,dst);
        LUT(dst,m_lut2,dst);
    }
    else
    {
        look_up_table(m_src,m_lut1,dst,m_mask);
        look_up_table(dst,m_lut2,dst,m_mask);
    }
}

/**
 *  @brief 单色化
 */
void Effect::Colorize(int H, int S, int L, Mat &dst)
{
    if(!m_src.data)
        return;

    int    lum_red_lookup[256];
    int    lum_green_lookup[256];
    int    lum_blue_lookup[256];

    int    final_red_lookup[256];
    int    final_green_lookup[256];
    int    final_blue_lookup[256];
    for(int i=0;i<256;i++)
    {
        lum_red_lookup[i]   = i*0.2126;
        lum_green_lookup[i] = i*0.7152;
        lum_blue_lookup[i]  = i*0.0722;
    }
    double h,s,l;
    h = (double)H/360.0;
    s = (double)S/100.0;

    for(int i=0;i<256;i++)
    {
        l = (double)i/255.0;
        QColor rgb = QColor::fromHslF(h,s,l).toRgb();

        final_red_lookup[i]=255*rgb.redF();
        final_green_lookup[i]=255*rgb.greenF();
        final_blue_lookup[i]=255*rgb.blueF();
    }

    Mat temp = Mat(m_src.size(),m_src.type());
    for(int row=0;row < m_src.rows;row++)
    {
        uchar* srcP = m_src.ptr<uchar>(row);
        uchar* tmpP = temp.ptr<uchar>(row);
        for(int col=0;col < m_src.cols;col++)
        {
            int lum = lum_blue_lookup[srcP[3*col]]+lum_green_lookup[srcP[3*col+1]]+lum_red_lookup[srcP[3*col+2]];

            if(L>0)
            {
                lum = (double) lum * (100.0 - L) / 100.0;
                lum += 255 - (100.0 - L) * 255.0 / 100.0;
            }
            else if(L<0)
            {
                lum = (double) lum * (L + 100.0) / 100.0;
            }
            tmpP[3*col] = final_blue_lookup[lum];
            tmpP[3*col+1] = final_green_lookup[lum];
            tmpP[3*col+2] = final_red_lookup[lum];
        }
    }

    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }
}

/**
 *  @brief 色调分离（减少颜色数量）
 */
void Effect::Posterize(int level, Mat &dst)
{
    if(!m_src.data)
        return;
    Mat m_lut = Mat(1,256,CV_8U);
    uchar *p = m_lut.data;

    for(int i=0;i<256;i++)
    {
        double d = 255.0/(level-1.0);

        p[i]=Clamp0255(Round(d*Round(i/d)));
    }

    if(m_mask.empty())
    {
        LUT(m_src,m_lut,dst);
    }
    else
    {
        if(m_mask.size()!=m_src.size())
        {
            qDebug() << "掩码尺寸应与原图像相同";
            return;
        }
        else
        {
            look_up_table(m_src,m_lut,dst,m_mask);
        }
    }
}

/**
 *  @brief 色调-亮度-饱和度
 */
void Effect::HueSaturation(_HueSaturation *hs, Mat &dst)
{
    if(!m_src.data)
        return;

    int hue_transfer[6][256]={0};
    int lightness_transfer[6][256]={0};
    int saturation_transfer[6][256]={0};

    int value;
    /*  Calculate transfers  */
    for (int hue_type=0; hue_type<6; hue_type++)
    {
        for (int i=0; i<256; i++)
        {
            /*  Hue  */
            value = (hs->hue[0]+ hs->hue[hue_type + 1]) * 255.0 / 360.0;
            if ((i + value) < 0)
                hue_transfer[hue_type][i] = 255 + (i + value);
            else if ((i + value) > 255)
                hue_transfer[hue_type][i] = i + value - 255;
            else
                hue_transfer[hue_type][i] = i + value;
            /*  Lightness  */
            value = (hs->lightness[0] + hs->lightness[hue_type + 1]) * 127.0 / 100.0;
            value = Clamp(value, -255, 255);

            if (value < 0)
                lightness_transfer[hue_type][i] = (uchar) ((i * (255 + value)) / 255);
            else
                lightness_transfer[hue_type][i] = (uchar) (i + ((255 - i) * value) / 255);

            /*  Saturation  */
            value = (hs->saturation[0]+ hs->saturation[hue_type + 1]) * 255.0 / 100.0;
            value = Clamp(value, -255, 255);

            saturation_transfer[hue_type][i] = Clamp0255((i * (255 + value)) / 255);
        }
    }

    Mat m_lut = Mat(1,256,CV_8UC3);
    uchar *p = m_lut.data;
    for(int i=0;i<256;i++)
    {
        int hue=0;
        if (i < 21)
            hue = 0 ;
        else if (i < 64)
            hue = 1 ;
        else if (i < 106)
            hue = 2 ;
        else if (i < 149)
            hue = 3 ;
        else if (i < 192)
            hue = 4 ;
        else if (i < 234)
            hue = 5 ;
        else
            hue = 0 ;
        p[3*i] = Clamp0255(hue_transfer[hue][i]);
        p[3*i+1] = Clamp0255(lightness_transfer[hue][i]);
        p[3*i+2] = Clamp0255(saturation_transfer[hue][i]);
    }

    Mat src_hls;
    if(m_src.channels()==3)
        cvtColor(m_src,src_hls,COLOR_BGR2HLS_FULL);

    if(m_mask.empty())
    {
        LUT(src_hls,m_lut,dst);
    }
    else
    {
        look_up_table(src_hls,m_lut,dst,m_mask);
    }

    cvtColor(dst,dst,COLOR_HLS2BGR_FULL);
}

/**
 *  @brief 色调均化
 */
void Effect::Equalize(Mat &dst)
{
    if(!m_src.data)
        return;

    dst = m_src.clone();

    std::vector<Mat> rgb;
    split(m_src,rgb);
    equalizeHist(rgb[0],rgb[0]);
    equalizeHist(rgb[1],rgb[1]);
    equalizeHist(rgb[2],rgb[2]);
    merge(rgb,m_src);

    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }
}

/**
 *  @brief 白平衡
 */
void Effect::WhiteBalance(Mat &dst)
{
    if(!m_src.data)
        return;

    _Levels *levels = new _Levels;
    for(int channel=0;channel<5;channel++)
    {
        levels->gamma[channel]       = 1.0;
        levels->low_input[channel]   = 0;
        levels->high_input[channel]  = 255;
        levels->low_output[channel]  = 0;
        levels->high_output[channel] = 255;
    }

    Histogram * hist = new Histogram(m_src);

    for(int channel = 1;channel <= 3;channel++)
    {
        hist->calculate(0,255,channel);
        const int nCount = hist->getTotal();
        if (nCount == 0)
        {
            levels->low_input[channel] = levels->high_input[channel] = 0 ;
        }
        else
        {
            // Set the low input
            for (int i=0 ; i < 255 ; i++)
            {
                hist->calculate(0,i,channel);
                double percentage = hist->getPercent();
                if(percentage>=1)
                {
                    levels->low_input[channel] = i;
                    break;
                }
            }
            // Set the high input
            for (int i=255 ; i > 0 ; i--)
            {
                hist->calculate(i,255,channel);
                double percentage = hist->getPercent();
                if(percentage>=1)
                {
                    levels->high_input[channel] = i;
                    break;
                }
            }
        }
    }
    SafeDelete(hist);
    this->Levels(levels,dst);
    SafeDelete(levels);
}

/**
 *  @brief 颜色增强
 */
void Effect::ColorEnhance(Mat &dst)
{
    if(!m_src.data)
        return;

    //InvertColor(m_src,src_temp);

    std::vector<Mat> mask_bgr(3);
    Mat mask_temp;
    std::vector<Mat> src_bgr;
    split(m_src,src_bgr);
    min(src_bgr[0],src_bgr[1],mask_bgr[0]);
    min(src_bgr[2],mask_bgr[0],mask_bgr[0]);
    mask_bgr[1]=mask_bgr[0];
    mask_bgr[2]=mask_bgr[0];
    merge(mask_bgr,mask_temp);
    m_src = m_src - mask_temp;
    //    for(int row=0;row < m_src.rows;row++)
    //    {
    //        Vec3b *p = src_temp.ptr<Vec3b>(row);
    //        Vec3b *p1 = mask_temp.ptr<Vec3b>(row);
    //        for(int col=0;col<m_src.cols;col++)
    //        {
    //            int k=p[col].val[0];
    //            if(k>p[col].val[1]) k=p[col].val[1];
    //            if(k>p[col].val[2]) k=p[col].val[2];
    //            p1[col].val[0] = k;
    //            p1[col].val[1] = k;
    //            p1[col].val[2] = k;

    //            p[col].val[0] = p[col].val[0]-k;
    //            p[col].val[1] = p[col].val[1]-k;
    //            p[col].val[2] = p[col].val[2]-k;
    //        }
    //    }

    Mat src_hsv;
    cvtColor(m_src,src_hsv,COLOR_BGR2HSV);
    std::vector<Mat> hsv;
    split(src_hsv,hsv);
    double min,max;
    minMaxIdx(hsv[2],&min,&max);

    Mat m_lut = Mat(1,256,CV_8U);
    uchar *p = m_lut.data;
    for(int i=0;i<256;i++)
    {
        if(i>=min)
            p[i]= 255*(i-min)/(max-min);
    }

    if(m_mask.empty())
    {
        LUT(hsv[2],m_lut,hsv[2]);
    }
    else
    {
        look_up_table(hsv[2],m_lut,hsv[2],m_mask);
    }

    merge(hsv,dst);
    cvtColor(dst,dst,COLOR_HSV2BGR);
    dst = dst + mask_temp;
    //InvertColor(dst,dst);
}

/**
 *  @brief 色阶
 */
void Effect::Levels(_Levels *levels, Mat &dst)
{
    if(!m_src.data)
        return;
    Mat m_lut = Mat(1,256,CV_8U);
    Mat m_lutR = Mat(1,256,CV_8U);
    Mat m_lutG = Mat(1,256,CV_8U);
    Mat m_lutB = Mat(1,256,CV_8U);
    uchar *p = m_lut.data;
    uchar *pR = m_lutR.data;
    uchar *pG = m_lutG.data;
    uchar *pB = m_lutB.data;

    for(int i=0;i<256;i++)
    {
        for (int j=0; j < 4; j++)
        {
            double inten = (double)i/255 ;
            /*  determine input intensity  */
            if(levels->low_input[j]!=levels->high_input[j])
                inten = (double)(i-levels->low_input[j])/
                        (double)(levels->high_input[j]-levels->low_input[j]);
            else
                inten = i-levels->low_input[j];

            /* clamp to new black and white points */
            inten = Clamp(inten,0.0,1.0);
            if(levels->gamma[j]!=0.0)
                inten =  pow(inten,(1.0/levels->gamma[j]));

            /*  determine the output intensity  */
            if (levels->high_output[j] >= levels->low_output[j])
                inten = (double) (inten * (levels->high_output[j] -
                                           levels->low_output[j]) +
                                  levels->low_output[j]);
            else if (levels->high_output[j] < levels->low_output[j])
                inten = (double) (levels->low_output[j] - inten *
                                  (levels->low_output[j] - levels->high_output[j]));

            switch(j)
            {
            case 0:
                p[i]=Clamp0255(Round(inten));
                break;
            case 1:
                pR[i]=Clamp0255(Round(inten));
                break;
            case 2:
                pG[i]=Clamp0255(Round(inten));
                break;
            case 3:
                pB[i]=Clamp0255(Round(inten));
                break;
            }

        }
    }
    if(m_mask.empty())
    {
        LUT(m_src,m_lut,dst);

        std::vector<Mat> bgr;
        split(dst,bgr);
        LUT(bgr[2],m_lutR,bgr[2]);
        LUT(bgr[1],m_lutG,bgr[1]);
        LUT(bgr[0],m_lutB,bgr[0]);
        merge(bgr,dst);
    }
    else
    {
        look_up_table(m_src,m_lut,dst,m_mask);

        std::vector<Mat> bgr;
        split(dst,bgr);
        look_up_table(bgr[2],m_lutR,bgr[2],m_mask);
        look_up_table(bgr[1],m_lutG,bgr[1],m_mask);
        look_up_table(bgr[0],m_lutB,bgr[0],m_mask);
        merge(bgr,dst);
    }
}

/**
 *  @brief 曲线
 */
void Effect::Curve(_Curve *curve, Mat &dst)
{
    if(!m_src.data)
        return;
    Mat m_lut = Mat(1,256,CV_8U);
    Mat m_lutR = Mat(1,256,CV_8UC1);
    Mat m_lutG = Mat(1,256,CV_8UC1);
    Mat m_lutB = Mat(1,256,CV_8UC1);
    uchar *p = m_lut.data;
    uchar *pR = m_lutR.data;
    uchar *pG = m_lutG.data;
    uchar *pB = m_lutB.data;

    for(int i=0;i<256;i++)
    {
        for(int j=0;j<4;j++)
        {
            switch(j)
            {
            case 0:
                p[i]=Clamp0255(curve->rgb[i]);
                break;
            case 1:
                pR[i]=Clamp0255(curve->r[i]);
                break;
            case 2:
                pG[i]=Clamp0255(curve->g[i]);
                break;
            case 3:
                pB[i]=Clamp0255(curve->b[i]);
                break;
            }
        }
    }

    if(m_mask.empty())
    {
        LUT(m_src,m_lut,dst);

        std::vector<Mat> bgr;
        split(dst,bgr);
        LUT(bgr[2],m_lutR,bgr[2]);
        LUT(bgr[1],m_lutG,bgr[1]);
        LUT(bgr[0],m_lutB,bgr[0]);
        merge(bgr,dst);
    }
    else
    {
        look_up_table(m_src,m_lut,dst,m_mask);

        std::vector<Mat> bgr;
        split(dst,bgr);
        look_up_table(bgr[2],m_lutR,bgr[2],m_mask);
        look_up_table(bgr[1],m_lutG,bgr[1],m_mask);
        look_up_table(bgr[0],m_lutB,bgr[0],m_mask);
        merge(bgr,dst);
    }
}

/**
 * @brief 径向模糊
 */
void Effect::ZoomBlur(_ZoomBlur *blur, Mat &dst)
{
    if(!m_src.data)
        return;

    int width=m_src.cols;
    int height=m_src.rows;

    long fox = (long)(width * blur->center_x * 32768.0);
    long foy = (long)(height* blur->center_y * 32768.0);
    long fcx = fox + (width << 15);
    long fcy = foy + (height << 15);
    long fz = blur->FocalLength;

    const int n = 64;
    Mat temp = Mat(m_src.size(),m_src.type());
    for (int row=0; row<height; row++)
    {
        uchar *srcP  = m_src.ptr<uchar>(row);
        uchar *tmpP  = temp.ptr<uchar>(row);
        for (int col=0; col<width; col++)
        {
            long fx = (col << 16) - fcx;
            long fy = (row << 16) - fcy;
            int sr = 0;
            int sg = 0;
            int sb = 0;

            sb += srcP[3*col];
            sg += srcP[3*col+1];
            sr += srcP[3*col+2];

            for (int i = 0; i < n; ++i)
            {
                fx -= ((fx >> 4) * fz) >> 10;
                fy -= ((fy >> 4) * fz) >> 10;

                int u = (int)((fx + fcx + 32768) >> 16);
                int v = (int)((fy + fcy + 32768) >> 16);
                sb += m_src.at<Vec3b>(v,u)[0];
                sg += m_src.at<Vec3b>(v,u)[1];
                sr += m_src.at<Vec3b>(v,u)[2];
            }
            tmpP[3*col] = sb/(n+1);
            tmpP[3*col+1] = sg/(n+1);
            tmpP[3*col+2] = sr/(n+1);
        }
    }
    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }

}

/**
 * @brief 旋转模糊
 */
void Effect::RadialBlur(_RadialBlur *blur, Mat &dst)
{
    if(!m_src.data)
        return;

    int width=m_src.cols;
    int height=m_src.rows;
    int nCenX = (blur->center_x+1)*width/2,
            nCenY = (blur->center_y+1)*height/2,
            R = (int) hypot(Max(nCenX,width-nCenX), Max(nCenY,height-nCenY)),
            n = (int)(4 * AngleToRadian(blur->angle) * sqrt((double)R) + 2) ;

    double ct[n], st[n] ;
    double   theta = (double)AngleToRadian(blur->angle) / ((double)(n - 1)),
            offset = double(theta * (n - 1) / 2.0) ;
    for (int i=0 ; i < n ; i++)
    {
        ct[i] = cos (theta * i - offset) ;
        st[i] = sin (theta * i - offset) ;
    }

    Mat temp = Mat(m_src.size(),m_src.type());
    for (int row=0; row<height; row++)
    {
        uchar *srcP  = m_src.ptr<uchar>(row);
        uchar *tmpP  = temp.ptr<uchar>(row);
        for (int col=0; col<width; col++)
        {
            int   xr = col - nCenX, yr = row - nCenY,
                    r = (int)sqrt ((double)FSquare(xr) + (double)FSquare(yr)),
                    nStep ;
            if (r == 0)
                nStep = 1 ;
            else
            {
                nStep = R/r ;
                if (nStep == 0)
                    nStep = 1 ;
                else
                    if (nStep > n-1)
                        nStep = n-1 ;
            }
            int nSumB=0,nSumG=0,nSumR=0,nCount = 0;

            nSumB += srcP[3*col];
            nSumG += srcP[3*col+1];
            nSumR += srcP[3*col+2];
            for (int i=0 ; i < n ; i += nStep)
            {
                int xx = (int)(nCenX + xr * ct[i] - yr * st[i]),
                        yy = (int)(nCenY + xr * st[i] + yr * ct[i]) ;
                if (xx > 0 && yy > 0 && xx < width && yy < height)
                {
                    nSumB += m_src.at<Vec3b>(yy,xx)[0];
                    nSumG += m_src.at<Vec3b>(yy,xx)[1];
                    nSumR += m_src.at<Vec3b>(yy,xx)[2];
                    nCount++ ;
                }

            }
            tmpP[3*col] = nSumB/(nCount+1);
            tmpP[3*col+1] = nSumG/(nCount+1);
            tmpP[3*col+2] = nSumR/(nCount+1);

        }

    }

    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }
}


/**
 * @brief 高斯模糊
 */
void Effect::GaussianBlur2(int x, int y, Mat &dst)
{
    if(!m_src.data)
        return;

    dst = m_src.clone();
    GaussianBlur(m_src,m_src,Size(2*x-1,2*y-1),0,0);
    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }
}

/**
 * @brief 运动模糊
 */
void Effect::MotionBlur(_MotionBlur *blur, Mat &dst)
{
    if(!m_src.data)
        return;

    double theta = (double)(blur->angle+180)/180*CV_PI;

    double x = blur->distance*cos(theta);
    double y = blur->distance*sin(theta);
    QPointF start(0.0,0.0);
    QPointF end(x,-y);
    if(blur->centered)
    {
        start = -end/2;
        end /=2;
    }

    double k = (end.y()-start.y())/(end.x()-start.x());
    double b = start.y() - k*start.x();
    double min=0;
    double max=0;
    if(start.x()<end.x())
    {
        min = start.x();
        max = end.x();
    }
    else
    {
        min = end.x();
        max = start.x();
    }
    QVector<QPointF> points;
    points << start;

    for(int i=floor(min+1);i<max;i++)
    {
        points << QPointF(i,k*i+b);
    }

    points << end;

    //    int width=m_src.cols, height=m_src.rows;

    //    Mat temp;
    //    Mat dstTemp = Mat(m_src.size(),m_src.type());
    //    dstTemp += m_src/(points.length()+1);
    //    //旋转后的新图尺寸
    //    Mat M = Mat( 2, 3, CV_32FC1 );
    //    for(int i=0;i<points.length();i++)
    //    {
    //        M = getRotationMatrix2D(Point(width/2,height/2),0,1);
    //        M.at<double>(0,2) += points[i].x();
    //        M.at<double>(1,2) += points[i].y();
    //        warpAffine(m_src,temp,M,Size(width,height),INTER_LINEAR,BORDER_REPLICATE);
    //        dstTemp += temp/(points.length()+1);
    //    }
    //    dst = dstTemp;

    int width = m_src.cols;
    int height = m_src.rows;
    Mat temp = Mat(m_src.size(),m_src.type());
    for (int row=0; row<height; row++)
    {
        uchar *srcP  = m_src.ptr<uchar>(row);
        uchar *tmpP  = temp.ptr<uchar>(row);
        for (int col=0; col<width; col++)
        {
            int sr = 0;
            int sg = 0;
            int sb = 0;
            int sc = 0;
            sb += srcP[3*col];
            sg += srcP[3*col+1];
            sr += srcP[3*col+2];

            for(int i=0;i<points.length();i++)
            {
                QPointF pt = QPointF(points[i].x() + (double)col, points[i].y() + (double)row);
                if (pt.x() >= 0 && pt.y() >= 0 && pt.x() <= (width - 1) && pt.y() <= (height - 1))
                {
                    sb += m_src.at<Vec3b>(pt.y(),pt.x())[0];
                    sg += m_src.at<Vec3b>(pt.y(),pt.x())[1];
                    sr += m_src.at<Vec3b>(pt.y(),pt.x())[2];
                    sc++;
                }
            }
            tmpP[3*col] = sb/(sc+1);
            tmpP[3*col+1] = sg/(sc+1);
            tmpP[3*col+2] = sr/(sc+1);
        }
    }
    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }
}

void Effect::SurfaceBlur(int radius, int threshold, Mat &dst)
{
    //    if(!m_src.data)
    //        return;

    //    int factors[256];
    //    double fv = threshold * 2.5;
    //    factors[0]=1;
    //    for(int i=1;i<256;i++)
    //       factors[i] = 1 - i/fv;

    //    copyMakeBorder(m_src,m_src,radius,radius,radius,radius,BORDER_REPLICATE);
    //    int width = m_src.cols;
    //    int height = m_src.rows;
    //    int size = radius << 1 + 1;

    //    for (int row=radius; row<height-radius; row++)
    //    {
    //        uchar *srcP  = m_src.ptr<uchar>(row);
    //        for (int col=radius; col<width-radius; col++)
    //        {
    //            int sb=0,sg=0,sr=0;
    //            for(int i=-radius;i<=radius;i++)
    //            {
    //                for(int j=-radius;j<=radius;j++)
    //                {
    //                   sb =factors[abs(m_src.at<Vec3b>(row+i,col+j)[0]-srcP[3*col])];
    //                   sg =factors[abs(m_src.at<Vec3b>(row+i,col+j)[1]-srcP[3*col+1])];
    //                   sr =factors[abs(m_src.at<Vec3b>(row+i,col+j)[2]-srcP[3*col+2])];

    //                }
    //            }

    //        }
    //    }

    //    double slope = (double)96 / threshold;

    //    for (int i = 0; i < 256; i++)
    //    {
    //        int factor = Round(255 - (i * slope));

    //        if (factor < 0)
    //        {
    //            factor = 0;
    //        }

    //        factors[i] = factor;
    //    }

}

void Effect::Mosaic(int x, int y, Mat &dst)
{
    if(!m_src.data)
        return;

    int width = m_src.cols;
    int height = m_src.rows;
    dst = m_src.clone();
    Mat roi,roi_avg;
    for (int row=0; row<height; row += y)
    {
        for (int col=0; col<width; col += x)
        {
            Rect rect(col,row,Min(x,width - col),Min(y,height-row));
            roi = m_src(rect);
            roi_avg = Mat(roi.size(),roi.type(),mean(roi));
            addWeighted(roi,0,roi_avg,1,0,roi);
        }
    }
    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }
}

void Effect::SobelEdge(Mat &dst)
{
    if(!m_src.data)
        return;

    dst = m_src.clone();
    cv::vector<Mat> bgr;
    split(m_src,bgr);

    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    for(int channel=0;channel<3;channel++)
    {
        GaussianBlur( bgr[channel], bgr[channel], Size(3,3), 0, 0, BORDER_DEFAULT );
        /// 求 X方向梯度
        Sobel( bgr[channel], grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_x, abs_grad_x );

        /// 求Y方向梯度
        Sobel( bgr[channel], grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
        convertScaleAbs( grad_y, abs_grad_y );

        /// 合并梯度(近似)
        addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, bgr[channel] );
    }
    merge(bgr,m_src);

    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }
}

void Effect::LaplacianEdge(Mat &dst)
{
    if(!m_src.data)
        return;

    dst = m_src.clone();
    cv::vector<Mat> bgr;
    split(m_src,bgr);

    for(int channel=0;channel<3;channel++)
    {
        GaussianBlur( bgr[channel], bgr[channel], Size(3,3), 0, 0, BORDER_DEFAULT );

        Laplacian( bgr[channel], bgr[channel], CV_16S, 3, 1, 0, BORDER_DEFAULT );

        convertScaleAbs( bgr[channel], bgr[channel] );
    }
    merge(bgr,m_src);

    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }

}

void Effect::CannyEdge(Mat &dst)
{
    if(!m_src.data)
        return;
    dst = m_src.clone();

    cv::vector<Mat> bgr;
    split(m_src,bgr);

    for(int channel=0;channel<3;channel++)
    {
        GaussianBlur( bgr[channel], bgr[channel], Size(3,3), 0, 0, BORDER_DEFAULT );

        Canny(bgr[channel], bgr[channel], 50, 200, 3);
    }
    merge(bgr,m_src);

    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }

}

void Effect::FillPattern(const Mat &tex, int alpha, Mat &dst)
{
    int width = m_src.cols;
    int height = m_src.rows;
    int tex_width = tex.cols;
    int tex_height = tex.rows;
    dst = m_src.clone();
    for (int row=0; row<height; row++)
    {
        uchar *pSrc = m_src.ptr<uchar>(row);
        for (int col=0; col<width; col++)
        {
            Vec3b pTex = tex.at<Vec3b>(row%tex_height,col%tex_width);
            //            qDebug() << pTex[0]+pTex[1]+pTex[2];
            int n = (pTex[0]+pTex[1]+pTex[2]-384)*alpha/765;
            pSrc[3*col] = Clamp0255(pSrc[3*col]-n);
            pSrc[3*col+1] = Clamp0255(pSrc[3*col+1]-n);
            pSrc[3*col+2] = Clamp0255(pSrc[3*col+2]-n);
        }
    }
    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }
}

void Effect::Emboss(_Emboss *emboss, Mat &dst)
{
    embossFilter *filter = new embossFilter;
    const double pixelScale = 255.9;
    double azimuth = emboss->azimuth *CV_PI/180.0;
    double elevation = emboss->elevation *CV_PI/180.0;
    int width45 = emboss->depth;

    filter->Lx = cos (azimuth) * cos (elevation) * pixelScale;
    filter->Ly = sin (azimuth) * cos (elevation) * pixelScale;
    filter->Lz = sin (elevation) * pixelScale;
    filter->Nz = (6 * 255) / width45;
    filter->Nz2 = filter->Nz * filter->Nz;
    filter->NzLz = filter->Nz * filter->Lz;

    filter->bg = filter->Lz;

    double M[3][3];
    int width = m_src.cols;
    int height = m_src.rows;
    int channel = m_src.channels();
    int step = m_src.step;
    uchar *s[3];
    s[0] = m_src.data;
    s[1] = s[0]+step;
    s[2] = s[1]+step;

    Mat temp = Mat(m_src.size(),m_src.type());
    uchar *d = temp.data;

    Mat src_clone = m_src.clone();
    uchar *tex = src_clone.data;
    if(emboss->embossp==0)
        tex += (width + 1) * channel;

    for(int y=0;y<height-1;y++)
    {
        for (int x = 0; x < width-1; x++)
        {
            long   Nx, Ny, NdotL;
            int    shade;

            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    M[i][j] = 0.0;

            for (int c = 0; c < channel; c++)
            {
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                    {
                        M[i][j] +=  s[i][j * channel + c];
                    }
            }

            Nx = M[0][0] + M[1][0] + M[2][0] - M[0][2] - M[1][2] - M[2][2];
            Ny = M[2][0] + M[2][1] + M[2][2] - M[0][0] - M[0][1] - M[0][2];

            /* shade with distant light source */
            if ( Nx == 0 && Ny == 0 )
                shade = filter->bg;
            else if ( (NdotL = Nx * filter->Lx + Ny * filter->Ly + filter->NzLz) < 0 )
                shade = 0;
            else
                shade = NdotL / sqrt(Nx*Nx + Ny*Ny + filter->Nz2);

            if (emboss->embossp==0)
            {
                for (int c = 0; c < channel; c++)
                    *d++ = (*tex++ * shade) >> 8;
            }
            else
            {
                for (int c = 0; c < channel; c++)
                    *d++ = shade;
            }

            for (int i = 0; i < 3; i++)
                s[i] += channel;
        }
    }
    SafeDelete(filter);

    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }
}

void Effect::RedEyeRemoval(int threshold,cv::Mat &dst)
{
    int width = m_src.cols;
    int height = m_src.rows;
    Mat temp = Mat(m_src.size(),m_src.type());
    for (int row=0; row<height; row++)
    {
        uchar *pSrc = m_src.ptr<uchar>(row);
        uchar *pTmp = temp.ptr<uchar>(row);

        for (int col=0; col<width; col++)
        {
            int adjusted_red       = pSrc[3*col+2] * 0.5133333;
            int adjusted_green     = pSrc[3*col+1] * 1;
            int adjusted_blue      = pSrc[3*col] * 0.1933333;
            int adjusted_threshold = (threshold - 50) * 2;

            if (adjusted_red >= adjusted_green - adjusted_threshold &&
                    adjusted_red >= adjusted_blue - adjusted_threshold)
            {
                pTmp[3*col+2] = Clamp (((double) (adjusted_green + adjusted_blue)
                                        / (2.0  * 0.5133333)), 0.0, 255.0);
            }
            else
            {
                pTmp[3*col+2] = pSrc[3*col+2];
            }

            pTmp[3*col+1] = pSrc[3*col+1];
            pTmp[3*col] = pSrc[3*col];
        }
    }
    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }
}

void Effect::Sharpen(Mat &dst)
{
    dst = m_src.clone();
    Mat kern = (Mat_<char>(3,3) <<  0,-1, 0,
                -1,5,-1,
                0,-1, 0);
    filter2D(m_src,m_src,m_src.depth(),kern);
    if(m_mask.empty())
    {
        dst = m_src;
    }
    else
    {
        replaceMask(m_src,dst,m_mask);
    }
}

void Effect::UnsharpMask(_UnsharpMask *sharp, Mat &dst)
{
    int size = 2*sharp->radius+1;
    //高斯模糊
    Mat src_blur;
    GaussianBlur(m_src,src_blur,Size(size,size),0, 0, BORDER_DEFAULT);
    //原图与高斯模糊结果之差
    Mat diff = cv::abs(m_src - src_blur) < sharp->threshold;

    Mat temp;
    temp = m_src*(1+sharp->amount)+src_blur*(-sharp->amount);
    m_src.copyTo(temp,diff);
    if(m_mask.empty())
    {
        dst = temp;
    }
    else
    {
        dst = m_src.clone();
        replaceMask(temp,dst,m_mask);
    }

}

/**
 *  直方图
 */

Histogram::Histogram(const Mat &src)
{
    if(!src.data)
        return;

    total = src.cols*src.rows;

    Mat src_temp,src_gray;
    if(src.channels()==3)
    {
        src_temp = src;
        cvtColor(src,src_gray,COLOR_BGR2GRAY);
    }
    else
    {
        cvtColor(src,src_temp,COLOR_GRAY2BGR);
        src_gray = src;
    }

    // 分割成3个单通道图像 ( R, G 和 B )
    std::vector<Mat> rgb_planes;
    split( src_temp, rgb_planes );

    // 设定bin数目
    int histSize = 256;

    // 设定取值范围 ( R,G,B) )
    float range[] = { -0.5, 255.5 } ;
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;


    // 计算直方图:
    calcHist( &src_gray,1, 0, Mat(), hist[0], 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &rgb_planes[2], 1, 0, Mat(), hist[1], 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &rgb_planes[1], 1, 0, Mat(), hist[2], 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &rgb_planes[0], 1, 0, Mat(), hist[3], 1, &histSize, &histRange, uniform, accumulate );
}

Histogram::Histogram(const Mat &src, Mat dst[])

{
    if(!src.data)
        return;

    new(this) Histogram(src);
    // 创建直方图画布
    int histSize = 256;
    int hist_w = 256; int hist_h = 200;
    int bin_w = cvRound( (double) hist_w/histSize );
    dst[0] = Mat( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );
    dst[1] = Mat( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );
    dst[2] = Mat( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );
    dst[3] = Mat( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );


    Mat gray_hist_N,r_hist_N,g_hist_N,b_hist_N;
    // 将直方图归一化到范围 [ 0, histImage.rows ]
    normalize(hist[0], gray_hist_N, 0, dst[0].rows, NORM_MINMAX, -1, Mat() );
    normalize(hist[1], r_hist_N, 0, dst[1].rows, NORM_MINMAX, -1, Mat() );
    normalize(hist[2], g_hist_N, 0, dst[2].rows, NORM_MINMAX, -1, Mat() );
    normalize(hist[3], b_hist_N, 0, dst[3].rows, NORM_MINMAX, -1, Mat() );

    for( int i = 0; i < histSize+1; i++ )
    {
        line( dst[0], Point( bin_w*i, hist_h-cvRound(gray_hist_N.at<float>(i)) ) ,
                Point( bin_w*i, hist_h),
                Scalar( 200, 200, 200));
        line( dst[1], Point( bin_w*i, hist_h-cvRound(r_hist_N.at<float>(i)) ) ,
                Point( bin_w*i, hist_h),
                Scalar( 160, 160, 255));
        line( dst[2], Point( bin_w*i, hist_h-cvRound(g_hist_N.at<float>(i)) ) ,
                Point( bin_w*i, hist_h),
                Scalar( 160, 255, 160));
        line( dst[3], Point( bin_w*i, hist_h-cvRound(b_hist_N.at<float>(i)) ) ,
                Point( bin_w*i, hist_h),
                Scalar( 255, 160, 160));
    }
}

Histogram::~Histogram()
{

}

void Histogram::calculate(int min, int max,int channel)
{
    x=0.0;
    s=0.0;
    median = 0;
    count = 0;
    percent = 0.0;
    //计算平均值
    for(int i=min;i<max+1;i++)
    {
        int n = hist[channel].at<float>(i);
        count += n;
        x += n*i;
    }

    x =  count ? (x/count) : 0.0 ;
    //计算中值
    double sum=0;
    for(int i=min;i<max+1;i++)
    {
        sum += hist[channel].at<float>(i);
        if(sum*2 > count)
        {
            median = i;
            break;
        }
    }
    //计算标准差
    for(int i=min;i<max+1;i++)
    {
        s += hist[channel].at<float>(i)*pow(i-x,2);
    }
    s = sqrt(s/count);
    //计算百分比
    percent = (double)count*100/total;
}

int Histogram::getTotal()
{
    return total;
}

int Histogram::getCount()
{
    return count;
}

int Histogram::getMedian()
{
    return median;
}

double Histogram::getMean()
{
    return x;
}

double Histogram::getDev()
{
    return s;
}

double Histogram::getPercent()
{
    return percent;
}

