#include "filter.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
Filter::Filter(void)
{
}

Filter::~Filter()
{

}

void Filter::FilerProcess(const Mat& src,Mat& dst,int type)
{
    if (!src.data && !dst.data)
        return;

    switch (type)
    {
    case FILTER_SKETCH:
        Sketch(src,dst);
        break;

    case FILTER_SCULPTURE:
        Sculpture(src,dst);
        break;

    case FILTER_INTAGLIO:
        Intaglio(src,dst);
        break;

    case FILTER_ECLOSION:
        Eclosion(src,dst);
        break;

    case FILTER_NOSTALGIA:
        Nostalgia(src,dst);
        break;

    case FILTER_CASTING:
        Casting(src,dst);
        break;

    case FILTER_FREEZING:
        Freezing(src,dst);
        break;

    case FILTER_INVERT_COLOR:
        InvertColor(src,dst);
        break;

    case FILTER_BINARY:
        Binary(src,dst);
        break;

    case FILTER_GRAY:
        Gray(src,dst);
        break;

    case FILTER_HARDLIGHT:
        HardLight(src,dst);
        break;

    case FILTER_SOFTLIGHT:
        SoftLight(src,dst);
        break;

    case FILTER_GLOWING_EDGES:
        GlowingEdges(src,dst);
        break;

    case FILTER_HIGH_PASS:
        HighPass(src,dst);
        break;

    case FILTER_SHARPEN:
        Sharpen(src,dst);
        break;

    case FILTER_POINTOLITE:
        //Pointolite(src,dst,Point(30,40));
        break;
    default:
        break;
    }
}

/*---------------------------------------------------------------------------------------*/
//把彩色图片打造成素描的效果仅仅需要几步操作：
//1、去色；
//2、复制去色图层，并且反色；反色为Y(i,j)=255-X(i,j)
//3、对反色图像进行高斯模糊；
//4、模糊后的图像叠加模式选择颜色减淡效果。
//减淡公式：C =MIN( A +（A×B）/（255-B）,255)，其中C为混合结果，A为去色后的像素点，B为高斯模糊后的像素点。
/*---------------------------------------------------------------------------------------*/
void Filter::Sketch(const Mat& src,Mat& dst)
{
    int width=src.cols;
    int height=src.rows;
    Mat gray0,gray1;
    if (src.channels()==3)
    {
        cvtColor(src,gray0,CV_BGR2GRAY);
    }
    else if (src.channels()==1)
    {
        gray0 = src;
    }
    addWeighted(gray0,-1,NULL,0,255,gray1);
    //高斯模糊,高斯核的Size与最后的效果有关
    GaussianBlur(gray1,gray1,Size(11,11),0);

    //融合：颜色减淡
    Mat img(gray1.size(),CV_8UC1);
    for (int y=0; y<height; y++)
    {

        uchar* P0  = gray0.ptr<uchar>(y);
        uchar* P1  = gray1.ptr<uchar>(y);
        uchar* P  = img.ptr<uchar>(y);
        for (int x=0; x<width; x++)
        {
            int tmp0=P0[x];
            int tmp1=P1[x];
            P[x] =(uchar) min((tmp0+(tmp0*tmp1)/(256-tmp1)),255);
        }
    }
    dst = img;
}

/*---------------------------------------------------------------------------------------*/
// 浮雕的算法是对图像的每一个点进行卷积处理，采用的矩阵如下:
// [1  0  0；
//  0  0  0；
//  0  0  -1 ]；
// 假设原图像为X，处理后的图像为Y ，也就是说，对于坐标为（i,j）点，
// 其浮雕效果图的算法为Y(i,j)=X(i+1,j+1)-X(i-1,j-1)+128。当然，X,Y的取值均在0~255之间。
// 同时也可以
// [1  0
//  0 -1]
// 假设原图像为X，处理后的图像为Y ，也就是说，对于坐标为（i,j）点，
// 其浮雕效果图的算法为Y(i,j)=X(i,j)-X(i-1,j-1)+128。当然，X,Y的取值均在0~255之间。
/*---------------------------------------------------------------------------------------*/
void Filter::Sculpture(const Mat& src,Mat& dst)
{
    int width = src.cols;
    int height = src.rows;
    int channel = src.channels();

    Mat tempDst(src.size(),src.type());

    for (int y = 1; y < height-1; y++)
    {
        const uchar *srcP0 = src.ptr<uchar>(y);
        const uchar *srcP1 = src.ptr<uchar>(y+1);

        uchar *dstP = tempDst.ptr<uchar>(y);

        for (int x = 1; x < width-1; x++)
        {
            for (int k = 0; k < channel; k++)
            {
                int tmp = srcP1[channel*(x+1)+k] - srcP0[channel*(x-1)+k] + 128;//浮雕
                tmp = (tmp>255 ? 255 : (tmp<0? 0 : tmp));
                dstP[channel*(x+1)+k]=tmp;
            }
        }
    }
    dst = tempDst;
}

void Filter::Intaglio(const Mat& src,Mat& dst)
{
    Sculpture(src,dst);
    InvertColor(dst,dst);
}
/*---------------------------------------------------------------------------------------*/
//在PHOTOSHOP里，羽化就是使你选定范围的图边缘达到朦胧的效果。
//
//羽化值越大，朦胧范围越宽，羽化值越小，朦胧范围越窄。可根据你想留下图的大小来调节。
//算法分析：
//1、通过对rgb值增加额外的V值实现朦胧效果
//2、通过控制V值的大小实现范围控制。
//3、V  = 255 * 当前点Point距中点距离的平方s1 / (顶点距中点的距离平方 *mSize)s2;
//4、s1 有根据 ratio 修正 dx dy值。
/*---------------------------------------------------------------------------------------*/
void Filter::Eclosion(const Mat& src,Mat& dst)
{
    const float MSIZE = 0.5;
    int width=src.cols;
    int height=src.rows;
    int channel = src.channels();

    int centerX=width>>1;
    int centerY=height>>1;

    int maxV=centerX*centerX+centerY*centerY;
    int minV=(int)(maxV*(1-MSIZE));
    int diff= maxV -minV;
    float ratio = width >height ? (float)height/(float)width : (float)width/(float)height;

    //Scalar avg=mean(src);
    Mat tempDst(src.size(),src.type());

    for (int y=0;y<height;y++)
    {
        const uchar* srcP=src.ptr<uchar>(y);
        uchar* dstP=tempDst.ptr<uchar>(y);
        for (int x=0;x<width;x++)
        {
            for (int k = 0; k < channel; k++)
            {
                int tmp = srcP[channel*x+k];
                float dx=static_cast<float>(centerX-x);
                float dy=static_cast<float>(centerY-y);

                if(width > height)
                    dx= (dx*ratio);
                else
                    dy = (dy*ratio);

                int dstSq =static_cast<int>(dx*dx + dy*dy);
                float v = ((float) dstSq / diff)*255;
                tmp = (int)(tmp +v);
                tmp = (tmp>255 ? 255 : (tmp<0? 0 : tmp));

                dstP[channel*x+k] = (uchar)tmp;
            }
            /*int b=srcP[3*x];
            int g=srcP[3*x+1];
            int r=srcP[3*x+2];

            float dx=static_cast<float>(centerX-x);
            float dy=static_cast<float>(centerY-y);

            if(width > height)
                dx= (dx*ratio);
            else
                dy = (dy*ratio);

            int dstSq =static_cast<int>(dx*dx + dy*dy);

            float v = ((float) dstSq / diff)*255;

            r = (int)(r +v);
            g = (int)(g +v);
            b = (int)(b +v);
            r = (r>255 ? 255 : (r<0? 0 : r));
            g = (g>255 ? 255 : (g<0? 0 : g));
            b = (b>255 ? 255 : (b<0? 0 : b));

            dstP[3*x] = (uchar)b;
            dstP[3*x+1] = (uchar)g;
            dstP[3*x+2] = (uchar)r;  */
        }
    }
    dst = tempDst;
}


/*---------------------------------------------------------------------------------------*/
//怀旧色调色原理
//R = 0.393r + 0.769g + 0.189b
//G = 0.349r + 0.686g + 0.168b
//B = 0.272r + 0.534g + 0.131b
/*---------------------------------------------------------------------------------------*/
void Filter::Nostalgia(const Mat& src,Mat& dst)
{
    if(src.channels()==1)
        return;
    int width=src.cols;
    int height=src.rows;
    Mat tempDst(src.size(),CV_8UC3);
    for (int y=0; y<height; y++)
    {
        const uchar* srcP  = src.ptr<uchar>(y);
        uchar* dstP  = tempDst.ptr<uchar>(y);
        for (int x=0; x<width; x++)
        {
            uchar B=srcP[3*x];
            uchar G=srcP[3*x+1];
            uchar R=srcP[3*x+2];
            float newB=static_cast<float>(0.272*R+0.534*G+0.131*B);
            float newG=static_cast<float>(0.349*R+0.686*G+0.168*B);
            float newR=static_cast<float>(0.393*R+0.769*G+0.189*B);
            if(newB<0)newB=0;
            if(newB>255)newB=255;
            if(newG<0)newG=0;
            if(newG>255)newG=255;
            if(newR<0)newR=0;
            if(newR>255)newR=255;
            dstP[3*x] = (uchar)newB;
            dstP[3*x+1] = (uchar)newG;
            dstP[3*x+2] = (uchar)newR;
        }
    }
    dst = tempDst;
}



/*---------------------------------------------------------------------------------------*/
//熔铸算法
//r = r*128/(g+b +1);
//g = g*128/(r+b +1);
//b = b*128/(g+r +1);
/*---------------------------------------------------------------------------------------*/
void Filter::Casting(const Mat& src,Mat& dst)
{
    if(src.channels()==1)
        return;
    int width=src.cols;
    int height=src.rows;
    Mat tempDst(src.size(),CV_8UC3);
    for (int y=0;y<height;y++)
    {
        const uchar* srcP=src.ptr<uchar>(y);
        uchar* dstP=tempDst.ptr<uchar>(y);
        for (int x=0;x<width;x++)
        {
            float b0=srcP[3*x];
            float g0=srcP[3*x+1];
            float r0=srcP[3*x+2];

            float b = b0*255/(g0+r0+1);
            float g = g0*255/(b0+r0+1);
            float r = r0*255/(g0+b0+1);

            r = (r>255 ? 255 : (r<0? 0 : r));
            g = (g>255 ? 255 : (g<0? 0 : g));
            b = (b>255 ? 255 : (b<0? 0 : b));

            dstP[3*x] = (uchar)b;
            dstP[3*x+1] = (uchar)g;
            dstP[3*x+2] = (uchar)r;
        }
    }
    dst = tempDst;
}


/*---------------------------------------------------------------------------------------*/
//冰冻算法
//r = (r-g-b)*3/2;
//g = (g-r-b)*3/2;
//b = (b-g-r)*3/2;
/*---------------------------------------------------------------------------------------*/
void Filter::Freezing(const Mat& src,Mat& dst)
{
    if(src.channels()==1)
        return;
    int width=src.cols;
    int height=src.rows;
    Mat tempDst(src.size(),src.type());
    for (int y=0;y<height;y++)
    {
        const uchar* srcP=src.ptr<uchar>(y);
        uchar* dstP=tempDst.ptr<uchar>(y);
        for (int x=0;x<width;x++)
        {
            float b0=srcP[3*x];
            float g0=srcP[3*x+1];
            float r0=srcP[3*x+2];

            float b = (b0-g0-r0)*3/2;
            float g = (g0-b0-r0)*3/2;
            float r = (r0-g0-b0)*3/2;

            r = (r>255 ? 255 : (r<0? -r : r));
            g = (g>255 ? 255 : (g<0? -g : g));
            b = (b>255 ? 255 : (b<0? -b : b));
            //          r = (r>255 ? 255 : (r<0? 0 : r));
            //          g = (g>255 ? 255 : (g<0? 0 : g));
            //          b = (b>255 ? 255 : (b<0? 0 : b));
            dstP[3*x] = (uchar)b;
            dstP[3*x+1] = (uchar)g;
            dstP[3*x+2] = (uchar)r;
        }
    }
    dst = tempDst;
}


/*---------------------------------------------------------------------------------------*/
//反色算法
//Y(i,j)=255-X(i,j)
/*---------------------------------------------------------------------------------------*/
void Filter::InvertColor(const Mat& src,Mat& dst)
{
    Mat lut(1,256,CV_8U);
    for(int i = 0; i < 256; i++)
    {
        lut.at<uchar>(i) = 255-i;
    }
    Mat tempDst(src.size(),src.type());
    LUT(src,lut,tempDst);
    dst = tempDst;

    //src.convertTo(dst,-1,-1,255);

    /*int width = src.cols;
    int height = src.rows;
    int channel = src.channels();

    Mat tempDst(src.size(),src.type());

    for (int y = 0; y < height; y++)
    {
        const uchar* srcP = src.ptr<uchar>(y);
        uchar* dstP = tempDst.ptr<uchar>(y);
        for (int x = 0; x < width; x++)
        {
            for (int k = 0; k < channel; k++)
            {
                dstP[channel*x+k] = 255 - srcP[channel*x+k];
            }
        }
    }
    dst = tempDst;*/
}

/*---------------------------------------------------------------------------------------*/
//黑白
/*---------------------------------------------------------------------------------------*/
void Filter::Binary(const Mat& src,Mat& dst)
{
    if (src.channels()==3)
    {
        cvtColor(src,dst,CV_BGR2GRAY);
    }

    threshold(dst,dst,70,255,CV_THRESH_BINARY);

}

/*---------------------------------------------------------------------------------------*/
//灰度图
/*---------------------------------------------------------------------------------------*/
void  Filter::Gray(const Mat& src,Mat& dst)
{
    if (src.channels()!=1)
    {
        cvtColor(src,dst,CV_BGR2GRAY);
    }
}
/*---------------------------------------------------------------------------------------*/
//高反差保留
//r  = (pix[x,y]-avg(R))
//dst[x,y] = 128+|r|*r/(2*R)
/*---------------------------------------------------------------------------------------*/
void Filter::HighPass(const Mat& src,Mat& dst)
{
    int R = 11;

    Mat avg;
    int width=src.cols;
    int height=src.rows;
    int channel = src.channels();
    //GaussianBlur(img,avg,Size(R,R),0.0);
    blur(src,avg,Size(R,R));
    Mat tempDst(src.size(),src.type());
    float tmp,r;
    for (int y=0;y<height;y++)
    {
        const uchar* srcP=src.ptr<uchar>(y);
        uchar* avgP=avg.ptr<uchar>(y);
        uchar* dstP=tempDst.ptr<uchar>(y);
        for (int x=0;x<width;x++)
        {
            for (int k = 0; k < channel; k++)
            {
                r = ((float)srcP[channel*x+k]-(float)avgP[channel*x+k]);
                tmp = 128+abs(r)*r/(2*R);
                tmp=tmp>255?255:tmp;
                tmp=tmp<0?0:tmp;
                dstP[channel*x+k]=(uchar)(tmp);
            }
        }
    }
    dst = tempDst;
}


/*---------------------------------------------------------------------------------------*/
//强光
//R(上)>127.5
//R=R(下)+(255-R(下))*(R(上)-127.5)/127.5;
//R(上)<127.5
//R=R(下)-R(下)*(127.5-R(上))/127.5=(R(上)*R(下))/127.5;
/*---------------------------------------------------------------------------------------*/
void Filter::HardLight(const Mat& src,Mat& dst)
{
    int width=src.cols;
    int height=src.rows;
    int channel = src.channels();
    Mat tempDst(src.size(),src.type());

    float r,tmp;
    for (int y=0;y<height;y++)
    {
        const uchar* srcP=src.ptr<uchar>(y);
        uchar* dstP=tempDst.ptr<uchar>(y);
        for (int x=0;x<width;x++)
        {
            for (int k = 0; k < channel; k++)
            {
                r = (float)srcP[channel*x+k];
                if(r>127.5)
                    tmp = static_cast<float>(r+(255-r)*(r-127.5)/127.5);
                else
                    tmp = static_cast<float>(r*r/127.5);
                tmp=tmp>255?255:tmp;
                tmp=tmp<0?0:tmp;
                dstP[channel*x+k]=(uchar)(tmp);
            }
        }
    }
    dst = tempDst;
}


/*---------------------------------------------------------------------------------------*/
//柔光
//R(上)>127.5
//R=R(下)+(255-R(下))*(R(上)-127.5)/127.5*(0.5-|R(下)-127.5|/255);
//R(上)<127.5
//R=R(下)-R(下)*(127.5-R(上))/127.5*(0.5-|R(下)-127.5|/255);
/*---------------------------------------------------------------------------------------*/
void Filter::SoftLight(const Mat& src,Mat& dst)
{
    int width=src.cols;
    int height=src.rows;
    int channel = src.channels();

    Mat tempDst(src.size(),src.type());

    float r,tmp;
    for (int y = 0; y < height; y++)
    {
        const uchar *srcP = src.ptr<uchar>(y);
        uchar *dstP = tempDst.ptr<uchar>(y);
        for (int x = 0; x < width; x++)
        {
            for (int k = 0; k < channel; k++)
            {


                r = (float)srcP[channel*x+k];
                if (r>127.5)
                    /*tmp = static_cast<float>(r+(255-r)*(r-127.5)/127.5*(0.5-abs(r-127.5)/255));*/
                    tmp = static_cast<float>(r*(255-r)/127.5+sqrt(r/255)*(2*r-255));
                else
                    /*tmp = static_cast<float>(r-r*(127.5-r)/127.5*(0.5-abs(r-127.5)/255));*/
                    tmp = static_cast<float>(pow(r/255,2)*(255-2*r)+pow(r,2)/127.5);
                tmp=tmp>255?255:tmp;
                tmp=tmp<0?0:tmp;
                dstP[channel*x+k]=(uchar)tmp;
            }
        }
    }
    dst = tempDst;
}


/*---------------------------------------------------------------------------------------*/
//照亮边缘
//对图像三通道求梯度，梯度结果重新和成图片
/*---------------------------------------------------------------------------------------*/
void Filter::GlowingEdges(const Mat& src,Mat& dst)
{
    if (src.channels()==3)
    {
        vector<Mat> rgb;
        split(src,rgb);

        GradientSobel(rgb[0]);
        GradientSobel(rgb[1]);
        GradientSobel(rgb[2]);

        Mat tempDst(src.size(),CV_8UC3);
        merge(rgb,tempDst);
        dst = tempDst;
    }
    else if(src.channels()==1)
    {
        Mat tempSrc = src.clone();
        GradientSobel(tempSrc);
        dst = tempSrc;
    }
}

/*---------------------------------------------------------------------------------------*/
//锐化
//对图像三通道求梯度，梯度结果重新和成图片
/*---------------------------------------------------------------------------------------*/
void Filter::Sharpen(const Mat& src,Mat& dst)
{
    //Mat blur,high_contrast;
    //GaussianBlur(src,blur,Size(5,5),0,0);
    //Mat mask = src-blur;
    //mask.convertTo(high_contrast,-1,2.2,0);
    //dst = high_contrast + src;

    //Mat kern = (Mat_<char>(3,3) <<  -1,-1, -1,
    //							    -1, 9,-1,
    //							    -1,-1, -1);
    Mat kern = (Mat_<char>(3,3) <<  0,-1, 0,
                                    -1,5,-1,
                                    0,-1, 0);
    Mat tempDst(src.size(),src.type());
    filter2D(src,tempDst,src.depth(),kern);
    dst = tempDst;
}

//void Filter::Pointolite(const Mat& src,Mat& dst,const Point &pos)
//{
//    int width = src.cols;
//    int height = src.rows;
//    int channel = src.channels();
//    int a = pos.x;
//    int b = pos.y;
//    Mat tempDst(src.size(),CV_8UC3);
//    for (int y = 0; y < height; y++)
//    {
//        const uchar* srcP = src.ptr<uchar>(y);
//        uchar* dstP = tempDst.ptr<uchar>(y);
//        for (int x = 0; x < width; x++)
//        {
//            for (int k = 0; k < channel; k++)
//            {
//                if (sqrt(pow(a-y,2)+pow(b-x,2)) < b)
//                {
//                    float tmp = static_cast<float>(srcP[3*x+k]+200*(1-(sqrt(pow(a-y,2)+pow(b-x,2))+1)/b));
//                    tmp=tmp>255?255:tmp;
//                    tmp=tmp<0?0:tmp;
//                    dstP[3*x+k] = (uchar)tmp;
//                }
//            }
//        }
//    }
//    dst = tempDst;
//}


void Filter::GradientSobel(Mat& src)
{
    int ddepth = CV_16S;
    GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    Sobel( src, grad_x, ddepth, 1, 0, 3, 1, 0, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    Sobel( src, grad_y, ddepth, 0, 1, 3, 1, 0, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, src );
}

