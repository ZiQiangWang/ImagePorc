#ifndef CURVE_H
#define CURVE_H

#include "qcustomplot.h"

class CubicSpline;
class CurveEditor : public QCustomPlot
{
    Q_OBJECT

public:
    explicit CurveEditor(QWidget *parent = 0);
    ~CurveEditor();

    void insertPoint(QPointF insertPoint);
    void setChannel(int channel);
    void channelChanged();
    void resetChannel(int channel);

public slots:
    void onReplot();
signals:
    void valueChanged(QVector<double> value);
    void getPosition(QPoint pos);
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
private:
    void initParam();
    void initView();
    void resetItems();
private:
    QVector<QPointF> points[4];
    QVector<double>  m_key,m_value[4];
    int select_index[4];
    CubicSpline *m_spline;
    QPointF oldPoint;
    bool overBound;
    int m_channel;
};

class Tracer : public QCPItemTracer
{
public:
    Tracer(QCustomPlot *customPlot, QCPGraph *graph);
    void setGraphKey(double key);
};
#endif // CURVE_H
