#ifndef FILTER_H
#define FILTER_H

namespace cv {
    class Mat;
}

class Filter
{
public:
    enum
    {
        FILTER_SKETCH		,	// 素描
        FILTER_SCULPTURE	,	// 浮雕
        FILTER_INTAGLIO		,	// 凹雕
        FILTER_ECLOSION		,	// 羽化
        FILTER_NOSTALGIA	,  // 怀旧
        FILTER_CASTING		,	// 熔铸
        FILTER_FREEZING		,	// 冰冻
        FILTER_INVERT_COLOR ,	// 反色
        FILTER_BINARY		,	// 黑白色
        FILTER_HARDLIGHT	,	// 强光
        FILTER_SOFTLIGHT	, // 柔光
        FILTER_GLOWING_EDGES, // 照亮边缘
        FILTER_HIGH_PASS	, // 高反差保留
        FILTER_SHARPEN		, // 锐化
        FILTER_GRAY			,	// 灰色
        FILTER_POINTOLITE   , // 点光源
    };
public:
    Filter(void);
    ~Filter(void);
    void FilerProcess(const cv::Mat& src,cv::Mat& dst,int type);

private:
    void Sketch(const cv::Mat& src,cv::Mat& dst);				// 素描
    void Sculpture(const cv::Mat& src,cv::Mat& dst);            // 浮雕
    void Intaglio(const cv::Mat& src,cv::Mat& dst);				// 凹雕
    void Eclosion(const cv::Mat& src,cv::Mat& dst);				// 羽化
    void Nostalgia(const cv::Mat& src,cv::Mat& dst);			// 怀旧
    void Casting(const cv::Mat& src,cv::Mat& dst);				// 熔铸
    void Freezing(const cv::Mat& src,cv::Mat& dst);				// 冰冻
    void InvertColor(const cv::Mat& src,cv::Mat& dst);			// 反色
    void Binary(const cv::Mat& src,cv::Mat& dst);				// 黑白色
    void Gray(const cv::Mat& src,cv::Mat& dst);					// 灰色
    void HighPass(const cv::Mat& src,cv::Mat& dst);				// 高反差保留
    void Colorful(const cv::Mat& src,cv::Mat& dst);				// 颜色变换
    void HardLight(const cv::Mat& src,cv::Mat& dst);			// 强光
    void SoftLight(const cv::Mat& src,cv::Mat& dst);			// 柔光
    void Sharpen(const cv::Mat& src,cv::Mat& dst);				// 锐化
    void GlowingEdges(const cv::Mat& src,cv::Mat& dst);			// 照亮边缘
//    void Pointolite(const cv::Mat& src,cv::Mat& dst,const cv::Point &pos);			// 点光源

private:
    void GradientSobel(cv::Mat& src);
};

#endif // FILTER_H
