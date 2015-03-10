#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <QPixmap>

namespace cv {
    class Mat;
}
std::string ws2s(const std::wstring& ws);
QPixmap Mat2Pixmap(const cv::Mat &image);

template<class T> inline const T& Max (const T& _X, const T& _Y) {return (_X <= _Y ? _Y : _X);}
template<class T> inline const T& Min (const T& _X, const T& _Y) {return (_Y <= _X ? _Y : _X);}

template<class T> inline T Clamp (const T& t, const T& tLow, const T& tHigh) {return Max (tLow, Min (tHigh, t)) ;}
inline int Clamp0255 (int n) {return Clamp (n, 0, 0xFF) ;}
inline double Clamp0255d (double n) {return Clamp (n, 0.0, 255.0) ;}

template<class T> inline T FSquare (const T& t) {return t*t ;}

// round double to int
inline int Round (const double& x)
{
    if (x > 0.0)
        return (int)(x + 0.5) ;
    else
        return (int)(x - 0.5) ;
}

template<class T>
void SafeDelete(T *&ptr)
{
    if(ptr)
    {
        delete ptr;
        ptr = 0;
    }
}

void Rotate(int &fx, int &fy, int fr);
double AngleToRadian (int nAngle);
#endif // UTILS_H
