#include "Spline.h"
#include "Utils.h"
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>
#include <string.h>
using namespace std;

#include <QPointF>

CubicSpline::CubicSpline(const QVector<QPointF>& points)
    : m_knum(points.size()), m_t(0), m_knot(0), m_ddp(0)
{
    if(m_knum<=0)
        return;

    m_t = new double[m_knum];
    m_knot = new double[m_knum];

    for(int i=0; i<m_knum; ++i)
    {
        m_t[i]  = points[i].x();
        m_knot[i] = points[i].y();
    }
    if(m_knum > 3)
        m_ddp = spline_cubic_set(m_knum, m_t, m_knot, 2, 0, 2, 0);
}

CubicSpline::~CubicSpline()
{
    if(m_t)
        delete []m_t;
    if(m_knot)
        delete []m_knot;
    if(m_ddp)
        delete []m_ddp;
}

void CubicSpline::getValue(const QVector<double> &key, QVector<double> &value)
{
    if(!m_t)
        return;
    value.clear();
    for(int i=0;i<key.size();i++)
    {
        double mValue;
        if(m_knum > 3)
        {
             if(!m_ddp)
                 return;

            double dp, ddp;
            mValue = spline_cubic_val(m_knum, m_t, key[i], m_knot, m_ddp, &dp, &ddp);
        }
        else if(m_knum > 2)
        {
            double dp;
            spline_quadratic_val(m_knum, m_t, m_knot, key[i], &mValue, &dp);

        }
        else if(m_knum > 1)
        {
            double dp;
            spline_linear_val(m_knum, m_t, m_knot, key[i], &mValue, &dp);
        }

        value << Clamp(mValue,0.0,255.0);
    }


    for(int i = 0;i<(int)m_t[0]+1;i++)
        value[i]=m_knot[0];
    for(int i = (int)m_t[m_knum-1];i<key.size();i++)
        value[i]=m_knot[m_knum-1];
}


//****************************************************************************80

void r8vec_bracket ( int n, double x[], double xval, int *left,
                    int *right )

                    //****************************************************************************80
                    //
                    //  Purpose:
                    //
                    //    R8VEC_BRACKET searches a sorted array for successive brackets of a value.
                    //
                    //  Discussion:
                    //
                    //    If the values in the vector are thought of as defining intervals
                    //    on the real line, then this routine searches for the interval
                    //    nearest to or containing the given value.
                    //
                    //    It is always true that RIGHT = LEFT+1.
                    //
                    //    If XVAL < X[0], then LEFT = 1, RIGHT = 2, and
                    //      XVAL   < X[0] < X[1];
                    //    If X(1) <= XVAL < X[N-1], then
                    //      X[LEFT-1] <= XVAL < X[RIGHT-1];
                    //    If X[N-1] <= XVAL, then LEFT = N-1, RIGHT = N, and
                    //      X[LEFT-1] <= X[RIGHT-1] <= XVAL.
                    //
                    //    For consistency, this routine computes indices RIGHT and LEFT
                    //    that are 1-based, although it would be more natural in C and
                    //    C++ to use 0-based values.
                    //
                    //  Modified:
                    //
                    //    24 February 2004
                    //
                    //  Author:
                    //
                    //    John Burkardt
                    //
                    //  Parameters:
                    //
                    //    Input, int N, length of input array.
                    //
                    //    Input, double X[N], an array that has been sorted into ascending order.
                    //
                    //    Input, double XVAL, a value to be bracketed.
                    //
                    //    Output, int *LEFT, *RIGHT, the results of the search.
                    //
{
    int i;

    for ( i = 2; i <= n - 1; i++ )
    {
        if ( xval < x[i-1] )
        {
            *left = i - 1;
            *right = i;
            return;
        }

    }

    *left = n - 1;
    *right = n;

    return;
}

//****************************************************************************80

double *d3_np_fs ( int n, double a[], double b[] )

//****************************************************************************80
//
//  Purpose:
//
//    D3_NP_FS factors and solves a D3 system.
//
//  Discussion:
//
//    The D3 storage format is used for a tridiagonal matrix.
//    The superdiagonal is stored in entries (1,2:N), the diagonal in
//    entries (2,1:N), and the subdiagonal in (3,1:N-1).  Thus, the
//    original matrix is "collapsed" vertically into the array.
//
//    This algorithm requires that each diagonal entry be nonzero.
//    It does not use pivoting, and so can fail on systems that
//    are actually nonsingular.
//
//  Example:
//
//    Here is how a D3 matrix of order 5 would be stored:
//
//       *  A12 A23 A34 A45
//      A11 A22 A33 A44 A55
//      A21 A32 A43 A54  *
//
//  Modified:
//
//    15 November 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the order of the linear system.
//
//    Input/output, double A[3*N].
//    On input, the nonzero diagonals of the linear system.
//    On output, the data in these vectors has been overwritten
//    by factorization information.
//
//    Input, double B[N], the right hand side.
//
//    Output, double D3_NP_FS[N], the solution of the linear system.
//    This is NULL if there was an error because one of the diagonal
//    entries was zero.
//
{
    int i;
    double *x;
    double xmult;
    //
    //  Check.
    //
    for ( i = 0; i < n; i++ )
    {
        if ( a[1+i*3] == 0.0 )
        {
            return NULL;
        }
    }
    x = new double[n];

    for ( i = 0; i < n; i++ )
    {
        x[i] = b[i];
    }

    for ( i = 1; i < n; i++ )
    {
        xmult = a[2+(i-1)*3] / a[1+(i-1)*3];
        a[1+i*3] = a[1+i*3] - xmult * a[0+i*3];
        x[i] = x[i] - xmult * x[i-1];
    }

    x[n-1] = x[n-1] / a[1+(n-1)*3];
    for ( i = n-2; 0 <= i; i-- )
    {
        x[i] = ( x[i] - a[0+(i+1)*3] * x[i+1] ) / a[1+i*3];
    }

    return x;
}

//****************************************************************************80

double *spline_cubic_set ( int n, double t[], double y[], int ibcbeg,
                          double ybcbeg, int ibcend, double ybcend )

                          //****************************************************************************80
                          //
                          //  Purpose:
                          //
                          //    SPLINE_CUBIC_SET computes the second derivatives of a piecewise cubic spline.
                          //
                          //  Discussion:
                          //
                          //    For data interpolation, the user must call SPLINE_SET to determine
                          //    the second derivative data, passing in the data to be interpolated,
                          //    and the desired boundary conditions.
                          //
                          //    The data to be interpolated, plus the SPLINE_SET output, defines
                          //    the spline.  The user may then call SPLINE_VAL to evaluate the
                          //    spline at any point.
                          //
                          //    The cubic spline is a piecewise cubic polynomial.  The intervals
                          //    are determined by the "knots" or abscissas of the data to be
                          //    interpolated.  The cubic spline has continous first and second
                          //    derivatives over the entire interval of interpolation.
                          //
                          //    For any point T in the interval T(IVAL), T(IVAL+1), the form of
                          //    the spline is
                          //
                          //      SPL(T) = A(IVAL)
                          //             + B(IVAL) * ( T - T(IVAL) )
                          //             + C(IVAL) * ( T - T(IVAL) )**2
                          //             + D(IVAL) * ( T - T(IVAL) )**3
                          //
                          //    If we assume that we know the values Y(*) and YPP(*), which represent
                          //    the values and second derivatives of the spline at each knot, then
                          //    the coefficients can be computed as:
                          //
                          //      A(IVAL) = Y(IVAL)
                          //      B(IVAL) = ( Y(IVAL+1) - Y(IVAL) ) / ( T(IVAL+1) - T(IVAL) )
                          //        - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * ( T(IVAL+1) - T(IVAL) ) / 6
                          //      C(IVAL) = YPP(IVAL) / 2
                          //      D(IVAL) = ( YPP(IVAL+1) - YPP(IVAL) ) / ( 6 * ( T(IVAL+1) - T(IVAL) ) )
                          //
                          //    Since the first derivative of the spline is
                          //
                          //      SPL'(T) =     B(IVAL)
                          //              + 2 * C(IVAL) * ( T - T(IVAL) )
                          //              + 3 * D(IVAL) * ( T - T(IVAL) )**2,
                          //
                          //    the requirement that the first derivative be continuous at interior
                          //    knot I results in a total of N-2 equations, of the form:
                          //
                          //      B(IVAL-1) + 2 C(IVAL-1) * (T(IVAL)-T(IVAL-1))
                          //      + 3 * D(IVAL-1) * (T(IVAL) - T(IVAL-1))**2 = B(IVAL)
                          //
                          //    or, setting H(IVAL) = T(IVAL+1) - T(IVAL)
                          //
                          //      ( Y(IVAL) - Y(IVAL-1) ) / H(IVAL-1)
                          //      - ( YPP(IVAL) + 2 * YPP(IVAL-1) ) * H(IVAL-1) / 6
                          //      + YPP(IVAL-1) * H(IVAL-1)
                          //      + ( YPP(IVAL) - YPP(IVAL-1) ) * H(IVAL-1) / 2
                          //      =
                          //      ( Y(IVAL+1) - Y(IVAL) ) / H(IVAL)
                          //      - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * H(IVAL) / 6
                          //
                          //    or
                          //
                          //      YPP(IVAL-1) * H(IVAL-1) + 2 * YPP(IVAL) * ( H(IVAL-1) + H(IVAL) )
                          //      + YPP(IVAL) * H(IVAL)
                          //      =
                          //      6 * ( Y(IVAL+1) - Y(IVAL) ) / H(IVAL)
                          //      - 6 * ( Y(IVAL) - Y(IVAL-1) ) / H(IVAL-1)
                          //
                          //    Boundary conditions must be applied at the first and last knots.
                          //    The resulting tridiagonal system can be solved for the YPP values.
                          //
                          //  Modified:
                          //
                          //    06 February 2004
                          //
                          //  Author:
                          //
                          //    John Burkardt
                          //
                          //  Parameters:
                          //
                          //    Input, int N, the number of data points.  N must be at least 2.
                          //    In the special case where N = 2 and IBCBEG = IBCEND = 0, the
                          //    spline will actually be linear.
                          //
                          //    Input, double T[N], the knot values, that is, the points were data is
                          //    specified.  The knot values should be distinct, and increasing.
                          //
                          //    Input, double Y[N], the data values to be interpolated.
                          //
                          //    Input, int IBCBEG, left boundary condition flag:
                          //      0: the cubic spline should be a quadratic over the first interval;
                          //      1: the first derivative at the left endpoint should be YBCBEG;
                          //      2: the second derivative at the left endpoint should be YBCBEG.
                          //
                          //    Input, double YBCBEG, the values to be used in the boundary
                          //    conditions if IBCBEG is equal to 1 or 2.
                          //
                          //    Input, int IBCEND, right boundary condition flag:
                          //      0: the cubic spline should be a quadratic over the last interval;
                          //      1: the first derivative at the right endpoint should be YBCEND;
                          //      2: the second derivative at the right endpoint should be YBCEND.
                          //
                          //    Input, double YBCEND, the values to be used in the boundary
                          //    conditions if IBCEND is equal to 1 or 2.
                          //
                          //    Output, double SPLINE_CUBIC_SET[N], the second derivatives of the cubic spline.
                          //
{
    double *a;
    double *b;
    int i;
    double *ypp;
    //
    //  Check.
    //
    if ( n <= 1 )
    {
        cout << "\n";
        cout << "SPLINE_CUBIC_SET - Fatal error!\n";
        cout << "  The number of data points N must be at least 2.\n";
        cout << "  The input value is " << n << ".\n";
        return NULL;
    }

    for ( i = 0; i < n - 1; i++ )
    {
        if ( t[i+1] <= t[i] )
        {
            cout << "\n";
            cout << "SPLINE_CUBIC_SET - Fatal error!\n";
            cout << "  The knots must be strictly increasing, but\n";
            cout << "  T(" << i   << ") = " << t[i]   << "\n";
            cout << "  T(" << i+1 << ") = " << t[i+1] << "\n";
            return NULL;
        }
    }
    a = new double[3*n];
    b = new double[n];
    //
    //  Set up the first equation.
    //
    if ( ibcbeg == 0 )
    {
        b[0] = 0.0;
        a[1+0*3] = 1.0;
        a[0+1*3] = -1.0;
    }
    else if ( ibcbeg == 1 )
    {
        b[0] = ( y[1] - y[0] ) / ( t[1] - t[0] ) - ybcbeg;
        a[1+0*3] = ( t[1] - t[0] ) / 3.0;
        a[0+1*3] = ( t[1] - t[0] ) / 6.0;
    }
    else if ( ibcbeg == 2 )
    {
        b[0] = ybcbeg;
        a[1+0*3] = 1.0;
        a[0+1*3] = 0.0;
    }
    else
    {
        cout << "\n";
        cout << "SPLINE_CUBIC_SET - Fatal error!\n";
        cout << "  IBCBEG must be 0, 1 or 2.\n";
        cout << "  The input value is " << ibcbeg << ".\n";
        delete [] a;
        delete [] b;
        return NULL;
    }
    //
    //  Set up the intermediate equations.
    //
    for ( i = 1; i < n-1; i++ )
    {
        b[i] = ( y[i+1] - y[i] ) / ( t[i+1] - t[i] )
            - ( y[i] - y[i-1] ) / ( t[i] - t[i-1] );
        a[2+(i-1)*3] = ( t[i] - t[i-1] ) / 6.0;
        a[1+ i   *3] = ( t[i+1] - t[i-1] ) / 3.0;
        a[0+(i+1)*3] = ( t[i+1] - t[i] ) / 6.0;
    }
    //
    //  Set up the last equation.
    //
    if ( ibcend == 0 )
    {
        b[n-1] = 0.0;
        a[2+(n-2)*3] = -1.0;
        a[1+(n-1)*3] = 1.0;
    }
    else if ( ibcend == 1 )
    {
        b[n-1] = ybcend - ( y[n-1] - y[n-2] ) / ( t[n-1] - t[n-2] );
        a[2+(n-2)*3] = ( t[n-1] - t[n-2] ) / 6.0;
        a[1+(n-1)*3] = ( t[n-1] - t[n-2] ) / 3.0;
    }
    else if ( ibcend == 2 )
    {
        b[n-1] = ybcend;
        a[2+(n-2)*3] = 0.0;
        a[1+(n-1)*3] = 1.0;
    }
    else
    {
        cout << "\n";
        cout << "SPLINE_CUBIC_SET - Fatal error!\n";
        cout << "  IBCEND must be 0, 1 or 2.\n";
        cout << "  The input value is " << ibcend << ".\n";
        delete [] a;
        delete [] b;
        return NULL;
    }
    //
    //  Solve the linear system.
    //
    if ( n == 2 && ibcbeg == 0 && ibcend == 0 )
    {
        ypp = new double[2];

        ypp[0] = 0.0;
        ypp[1] = 0.0;
    }
    else
    {
        ypp = d3_np_fs ( n, a, b );

        if ( !ypp )
        {
            cout << "\n";
            cout << "SPLINE_CUBIC_SET - Fatal error!\n";
            cout << "  The linear system could not be solved.\n";
            delete [] a;
            delete [] b;
            return NULL;
        }

    }

    delete [] a;
    delete [] b;
    return ypp;
}

//****************************************************************************80

double spline_cubic_val ( int n, double t[], double tval, double y[],
                         double ypp[], double *ypval, double *yppval )

                         //****************************************************************************80
                         //
                         //  Purpose:
                         //
                         //    SPLINE_CUBIC_VAL evaluates a piecewise cubic spline at a point.
                         //
                         //  Discussion:
                         //
                         //    SPLINE_CUBIC_SET must have already been called to define the values of YPP.
                         //
                         //    For any point T in the interval T(IVAL), T(IVAL+1), the form of
                         //    the spline is
                         //
                         //      SPL(T) = A
                         //             + B * ( T - T(IVAL) )
                         //             + C * ( T - T(IVAL) )**2
                         //             + D * ( T - T(IVAL) )**3
                         //
                         //    Here:
                         //      A = Y(IVAL)
                         //      B = ( Y(IVAL+1) - Y(IVAL) ) / ( T(IVAL+1) - T(IVAL) )
                         //        - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * ( T(IVAL+1) - T(IVAL) ) / 6
                         //      C = YPP(IVAL) / 2
                         //      D = ( YPP(IVAL+1) - YPP(IVAL) ) / ( 6 * ( T(IVAL+1) - T(IVAL) ) )
                         //
                         //  Modified:
                         //
                         //    04 February 1999
                         //
                         //  Author:
                         //
                         //    John Burkardt
                         //
                         //  Parameters:
                         //
                         //    Input, int N, the number of knots.
                         //
                         //    Input, double Y[N], the data values at the knots.
                         //
                         //    Input, double T[N], the knot values.
                         //
                         //    Input, double TVAL, a point, typically between T[0] and T[N-1], at
                         //    which the spline is to be evalulated.  If TVAL lies outside
                         //    this range, extrapolation is used.
                         //
                         //    Input, double Y[N], the data values at the knots.
                         //
                         //    Input, double YPP[N], the second derivatives of the spline at
                         //    the knots.
                         //
                         //    Output, double *YPVAL, the derivative of the spline at TVAL.
                         //
                         //    Output, double *YPPVAL, the second derivative of the spline at TVAL.
                         //
                         //    Output, double SPLINE_VAL, the value of the spline at TVAL.
                         //
{
    double dt;
    double h;
    int i;
    int ival;
    double yval;
    //
    //  Determine the interval [ T(I), T(I+1) ] that contains TVAL.
    //  Values below T[0] or above T[N-1] use extrapolation.
    //
    ival = n - 2;

    for ( i = 0; i < n-1; i++ )
    {
        if ( tval < t[i+1] )
        {
            ival = i;
            break;
        }
    }
    //
    //  In the interval I, the polynomial is in terms of a normalized
    //  coordinate between 0 and 1.
    //
    dt = tval - t[ival];
    h = t[ival+1] - t[ival];

    yval = y[ival]
    + dt * ( ( y[ival+1] - y[ival] ) / h
        - ( ypp[ival+1] / 6.0 + ypp[ival] / 3.0 ) * h
        + dt * ( 0.5 * ypp[ival]
    + dt * ( ( ypp[ival+1] - ypp[ival] ) / ( 6.0 * h ) ) ) );

    *ypval = ( y[ival+1] - y[ival] ) / h
        - ( ypp[ival+1] / 6.0 + ypp[ival] / 3.0 ) * h
        + dt * ( ypp[ival]
    + dt * ( 0.5 * ( ypp[ival+1] - ypp[ival] ) / h ) );

    *yppval = ypp[ival] + dt * ( ypp[ival+1] - ypp[ival] ) / h;

    return yval;
}

//****************************************************************************80

void spline_quadratic_val ( int ndata, double tdata[], double ydata[],
                           double tval, double *yval, double *ypval )

                           //****************************************************************************80
                           //
                           //  Purpose:
                           //
                           //    SPLINE_QUADRATIC_VAL evaluates a piecewise quadratic spline at a point.
                           //
                           //  Discussion:
                           //
                           //    Because of the simple form of a piecewise quadratic spline,
                           //    the raw data points ( TDATA(I), YDATA(I)) can be used directly to
                           //    evaluate the spline at any point.  No processing of the data
                           //    is required.
                           //
                           //  Modified:
                           //
                           //    24 February 2004
                           //
                           //  Author:
                           //
                           //    John Burkardt
                           //
                           //  Parameters:
                           //
                           //    Input, int NDATA, the number of data points defining the spline.
                           //    NDATA should be odd and at least 3.
                           //
                           //    Input, double TDATA[NDATA], YDATA[NDATA], the values of the independent
                           //    and dependent variables at the data points.  The values of TDATA should
                           //    be distinct and increasing.
                           //
                           //    Input, double TVAL, the point at which the spline is to be evaluated.
                           //
                           //    Output, double *YVAL, *YPVAL, the value of the spline and its first
                           //    derivative dYdT at TVAL.  YPVAL is not reliable if TVAL is exactly
                           //    equal to TDATA(I) for some I.
                           //
{
    double dif1;
    double dif2;
    int left;
    int right;
    double t1;
    double t2;
    double t3;
    double y1;
    double y2;
    double y3;

    if ( ndata < 3 )
    {
        cout << "\n";
        cout << "SPLINE_QUADRATIC_VAL - Fatal error!\n";
        cout << "  NDATA < 3.\n";
        exit ( 1 );
    }

    if ( ndata % 2 == 0 )
    {
        cout << "\n";
        cout << "SPLINE_QUADRATIC_VAL - Fatal error!\n";
        cout << "  NDATA must be odd.\n";
        exit ( 1 );
    }
    //
    //  Find the interval [ TDATA(LEFT), TDATA(RIGHT) ] that contains, or is
    //  nearest to, TVAL.
    //
    r8vec_bracket ( ndata, tdata, tval, &left, &right );
    //
    //  Force LEFT to be odd.
    //
    if ( left % 2 == 0 )
    {
        left = left - 1;
    }
    //
    //  Copy out the three abscissas.
    //
    t1 = tdata[left-1];
    t2 = tdata[left  ];
    t3 = tdata[left+1];

    if ( t2 <= t1 || t3 <= t2 )
    {
        cout << "\n";
        cout << "SPLINE_QUADRATIC_VAL - Fatal error!\n";
        cout << "  T2 <= T1 or T3 <= T2.\n";
        exit ( 1 );
    }
    //
    //  Construct and evaluate a parabolic interpolant for the data
    //  in each dimension.
    //
    y1 = ydata[left-1];
    y2 = ydata[left  ];
    y3 = ydata[left+1];

    dif1 = ( y2 - y1 ) / ( t2 - t1 );

    dif2 = ( ( y3 - y1 ) / ( t3 - t1 )
        - ( y2 - y1 ) / ( t2 - t1 ) ) / ( t3 - t2 );

    *yval = y1 + ( tval - t1 ) * ( dif1 + ( tval - t2 ) * dif2 );
    *ypval = dif1 + dif2 * ( 2.0 * tval - t1 - t2 );

    return;
}

//****************************************************************************80

void spline_linear_val ( int ndata, double tdata[], double ydata[],
                        double tval, double *yval, double *ypval )

                        //****************************************************************************80
                        //
                        //  Purpose:
                        //
                        //    SPLINE_LINEAR_VAL evaluates a piecewise linear spline at a point.
                        //
                        //  Discussion:
                        //
                        //    Because of the extremely simple form of the linear spline,
                        //    the raw data points ( TDATA(I), YDATA(I)) can be used directly to
                        //    evaluate the spline at any point.  No processing of the data
                        //    is required.
                        //
                        //  Modified:
                        //
                        //    24 February 2004
                        //
                        //  Author:
                        //
                        //    John Burkardt
                        //
                        //  Parameters:
                        //
                        //    Input, int NDATA, the number of data points defining the spline.
                        //
                        //    Input, double TDATA[NDATA], YDATA[NDATA], the values of the independent
                        //    and dependent variables at the data points.  The values of TDATA should
                        //    be distinct and increasing.
                        //
                        //    Input, double TVAL, the point at which the spline is to be evaluated.
                        //
                        //    Output, double *YVAL, *YPVAL, the value of the spline and its first
                        //    derivative dYdT at TVAL.  YPVAL is not reliable if TVAL is exactly
                        //    equal to TDATA(I) for some I.
                        //
{
    int left;
    int right;
    //
    //  Find the interval [ TDATA(LEFT), TDATA(RIGHT) ] that contains, or is
    //  nearest to, TVAL.
    //
    r8vec_bracket ( ndata, tdata, tval, &left, &right );
    //
    //  Now evaluate the piecewise linear function.
    //
    *ypval = ( ydata[right-1] - ydata[left-1] )
        / ( tdata[right-1] - tdata[left-1] );

    *yval = ydata[left-1] +  ( tval - tdata[left-1] ) * (*ypval);

    return;
}

