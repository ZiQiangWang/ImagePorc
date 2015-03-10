#ifndef SPLINE_H
#define SPLINE_H

#include <QVector>
QT_BEGIN_NAMESPACE
//class QVector;
class QPointF;
QT_END_NAMESPACE
class CubicSpline
{
public:
    CubicSpline(const QVector<QPointF>& points);
    ~CubicSpline();

    void getValue(const QVector<double> &key,QVector<double> &value);
private:
    int m_knum;
    double *m_t;
    double *m_knot;
    double *m_ddp;
};

void r8vec_bracket ( int n, double x[], double xval, int *left,
                    int *right );
double *d3_np_fs ( int n, double a[], double b[] );

double *spline_cubic_set ( int n, double t[], double y[], int ibcbeg,
                          double ybcbeg, int ibcend, double ybcend );
double spline_cubic_val ( int n, double t[], double tval, double y[], double ypp[],
                         double *ypval, double *yppval );
void spline_quadratic_val ( int ndata, double tdata[], double ydata[],
                           double tval, double *yval, double *ypval );
void spline_linear_val ( int ndata, double tdata[], double ydata[],
                        double tval, double *yval, double *ypval );


#endif // SPLINE_H
