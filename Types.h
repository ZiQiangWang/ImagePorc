#ifndef TYPES_H
#define TYPES_H
/**
 * 旋转-偏移-缩放-透视参数
 */
struct _RotateZoom
{
    int angle;
    int per_angle,perspective;
    double x,y;
    double scale;

    _RotateZoom(int _angle,int _per_angle,int _perspective,double _x,double _y,double _scale)
        :angle(_angle),per_angle(_per_angle),perspective(_perspective),x(_x),y(_y),scale(_scale)
    {}
};

/**
 * 色彩平衡参数
 */
struct _ColorBalance
{
    bool preserve_luminosity;

    int cyan_red[3];
    int magenta_green[3];
    int yellow_blue[3];
};


/**
 * 色阶参数
 */
struct _Levels
{
  double gamma[5];

  int    low_input[5];
  int    high_input[5];

  int    low_output[5];
  int    high_output[5];
};

/**
 * 色调-亮度-饱和度
 */
struct _HueSaturation
{
    double hue[7];
    double lightness[7];
    double saturation[7];
    double overlap;
};


/**
 * 曲线参数
 */
struct _Curve
{
    double rgb[256];
    double r[256];
    double g[256];
    double b[256];
};

/**
 * 径向模糊参数
 */
struct _ZoomBlur
{
    int    FocalLength;
    double center_x;
    double center_y;

    _ZoomBlur(int l,double x,double y)
        :FocalLength(l),center_x(x),center_y(y){}
};

/**
 * 旋转模糊参数
 */
struct _RadialBlur
{
    int    angle;
    double center_x;
    double center_y;

    _RadialBlur(int a,double x,double y)
        :angle(a),center_x(x),center_y(y){}
};

/**
 * 运动模糊参数
 */
struct _MotionBlur
{
    int  angle;
    int  distance;
    bool centered;
    _MotionBlur(int a,int d,bool c)
        :angle(a),distance(d),centered(c){}
};

/**
 * 虚光蒙版参数
 */
struct _UnsharpMask
{
    double  radius;
    double  amount;
    int     threshold;
    _UnsharpMask(double r,double a,int t)
        :radius(r),amount(a),threshold(t){}
};

/**
 * 浮雕
 */
struct _Emboss
{
    int  azimuth;   //极角
    int  elevation; //仰角
    int  depth;     //深度
    int  embossp;   //有色
    _Emboss(int a,int e,int d,int em)
        :azimuth(a),elevation(e),depth(d),embossp(em){}
};

struct embossFilter
{
  double Lx;
  double Ly;
  double Lz;
  double Nz;
  double Nz2;
  double NzLz;
  double bg;
};

#endif // TYPES_H


