#include "CurveEditor.h"
#include "Utils.h"
#include "Spline.h"
#include <QDebug>
CurveEditor::CurveEditor(QWidget *parent):
    QCustomPlot(parent)
{

    initParam();

    initView();

    connect(this,SIGNAL(afterReplot()),this,SLOT(onReplot()));
}

CurveEditor::~CurveEditor()
{
    SafeDelete(m_spline);
}

void CurveEditor::initParam()
{
    m_channel = 0;

    oldPoint = QPointF(-1,-1);
    overBound = false;

    for(int i=0;i<256;i++)
    {
        m_key << i;
    }

    for(int i=0;i<4;i++)
    {
        select_index[i] = -1;
        points[i] << QPointF(0.0,0.0)<<QPointF(255.0,255.0);
    }



    m_spline = new CubicSpline(points[m_channel]);
    m_spline->getValue(m_key,m_value[m_channel]);
}

void CurveEditor::initView()
{
    this->xAxis->setRange(0,255);
    this->yAxis->setRange(-1,256);
    this->xAxis2->setVisible(true);
    this->yAxis2->setVisible(true);
    this->axisRect()->setAutoMargins(false);
    this->axisRect()->setMargins(QMargins(1,1,1,1));
    this->axisRect()->setBackgroundScaled(true);
    this->axisRect()->setBackgroundScaledMode(Qt::IgnoreAspectRatio);
    this->addGraph();
    QVector<double> base;
    base << 0.0<<255.0;
    this->graph(0)->setData(base,base);
    this->graph(0)->setPen(QPen(Qt::lightGray));

    this->addGraph();
    this->graph(1)->setData(m_key,m_value[m_channel]);
    this->graph(1)->setPen(QPen(Qt::black,1,Qt::DashLine));
    this->graph(1)->setSelectedPen(QPen(Qt::black,1.5));
    this->graph(1)->setSelected(true);

    this->addGraph();
    this->graph(2)->setData(m_key,m_value[m_channel]);
    this->graph(2)->setPen(QPen(Qt::red,1,Qt::DashLine));
    this->graph(2)->setSelectedPen(QPen(Qt::red,1.5));

    this->addGraph();
    this->graph(3)->setData(m_key,m_value[m_channel]);
    this->graph(3)->setPen(QPen(Qt::green,1,Qt::DashLine));
    this->graph(3)->setSelectedPen(QPen(Qt::green,1.5));

    this->addGraph();
    this->graph(4)->setData(m_key,m_value[m_channel]);
    this->graph(4)->setPen(QPen(Qt::blue,1,Qt::DashLine));
    this->graph(4)->setSelectedPen(QPen(Qt::blue,1.5,Qt::SolidLine));

    Tracer *tracer0 = new Tracer(this,this->graph(0));
    tracer0->setGraphKey(0.0);
    this->addItem(tracer0);

    Tracer *tracer255 = new Tracer(this,this->graph(0));
    tracer255->setGraphKey(255.0);
    this->addItem(tracer255);

    this->replot();
}

void CurveEditor::mouseMoveEvent(QMouseEvent *event)
{
    double key = this->xAxis->pixelToCoord(event->x());
    double value = this->yAxis->pixelToCoord(event->y());

    emit getPosition(QPoint(key,value));

    if(event->buttons()&Qt::LeftButton)
    {
        if(overBound)
            return;

        key = Clamp(key,0.0,255.0);
        value = Clamp(value,0.0,255.0);

        static_cast<Tracer *>(this->item(select_index[m_channel]))->setGraphKey(key);
        points[m_channel][select_index[m_channel]] = QPointF(key,value);
        if(select_index[m_channel]>0 && select_index[m_channel]<points[m_channel].size()-1)
        {
            if( points[m_channel][select_index[m_channel]].x()<points[m_channel][select_index[m_channel]-1].x()+1 || points[m_channel][select_index[m_channel]].x() > points[m_channel][select_index[m_channel]+1].x()-1)
            {
                overBound = true;
                points[m_channel].remove(select_index[m_channel]);
                select_index[m_channel]=-1;
                resetItems();

                SafeDelete(m_spline);
                m_spline = new CubicSpline(points[m_channel]);
                m_spline->getValue(m_key,m_value[m_channel]);

                this->graph(m_channel+1)->setData(m_key,m_value[m_channel]);
                this->replot();
                return;
            }
        }
        else if(select_index[m_channel]==0)
        {
            if(points[m_channel][select_index[m_channel]].x() > points[m_channel][select_index[m_channel]+1].x()-1)
            {
                overBound = true;
                points[m_channel].remove(select_index[m_channel]);
                                select_index[m_channel]=-1;
                resetItems();

                SafeDelete(m_spline);
                m_spline = new CubicSpline(points[m_channel]);
                m_spline->getValue(m_key,m_value[m_channel]);
                this->graph(m_channel+1)->setData(m_key,m_value[m_channel]);
                this->replot();
                return;
            }
        }
        else if(select_index[m_channel] == points[m_channel].size()-1)
        {
            if(points[m_channel][select_index[m_channel]].x()<points[m_channel][select_index[m_channel]-1].x()+1)
            {
                overBound = true;
                points[m_channel].remove(select_index[m_channel]);
                select_index[m_channel]=-1;
                resetItems();

                SafeDelete(m_spline);
                m_spline = new CubicSpline(points[m_channel]);
                m_spline->getValue(m_key,m_value[m_channel]);
                this->graph(m_channel+1)->setData(m_key,m_value[m_channel]);
                this->replot();
                return;
            }
        }

        SafeDelete(m_spline);
        m_spline = new CubicSpline(points[m_channel]);
        m_spline->getValue(m_key,m_value[m_channel]);


        this->graph(m_channel+1)->setData(m_key,m_value[m_channel]);
        this->replot();
    }
    QCustomPlot::mouseMoveEvent(event);
}

void CurveEditor::mousePressEvent(QMouseEvent *event)
{
    double key = this->xAxis->pixelToCoord(event->x());
    double value = this->yAxis->pixelToCoord(event->y());
    if(event->buttons()&Qt::LeftButton)
    {
        oldPoint = QPointF(key,value);
        insertPoint(QPointF(key,value));
        SafeDelete(m_spline);
        m_spline = new CubicSpline(points[m_channel]);
        m_spline->getValue(m_key,m_value[m_channel]);

        this->graph(m_channel+1)->setData(m_key,m_value[m_channel]);

        for(int i=0;i<this->itemCount();i++)
        {
            this->item(i)->setSelected(false);
        }
        this->item(select_index[m_channel])->setSelected(true);
        this->replot();
    }
    else if(event->buttons()&Qt::RightButton)
    {
        if(select_index[m_channel] != -1)
        {
            points[m_channel].remove(select_index[m_channel]);
            select_index[m_channel] = -1;

            SafeDelete(m_spline);
            m_spline = new CubicSpline(points[m_channel]);
            m_spline->getValue(m_key,m_value[m_channel]);
            this->graph(1)->setData(m_key,m_value[m_channel]);
            resetItems();
            this->replot();
        }
    }

    QCustomPlot::mousePressEvent(event);
}

void CurveEditor::mouseReleaseEvent(QMouseEvent *event)
{
    oldPoint = QPointF(-1,-1);
    overBound = false;
    QCustomPlot::mouseReleaseEvent(event);
}

void CurveEditor::leaveEvent(QEvent *event)
{
    emit getPosition(QPoint(-1,-1));
    QCustomPlot::leaveEvent(event);
}

void CurveEditor::insertPoint(QPointF insertPoint)
{
    for(int i=0;i<points[m_channel].size();i++)
    {
        if(insertPoint.x() <= points[m_channel][i].x()+10 && insertPoint.x() >= points[m_channel][i].x()-10)
        {
            static_cast<Tracer *>(this->item(i))->setGraphKey(insertPoint.x());

            points[m_channel][i]=insertPoint;
            resetItems();
            select_index[m_channel] = i;
            break;
        }
        else if(insertPoint.x()<points[m_channel][i].x())
        {

            points[m_channel].insert(i,insertPoint);
            resetItems();
            select_index[m_channel] = i;
            break;
        }
        else if(i == points[m_channel].size()-1)
        {
            points[m_channel].push_back(insertPoint);
            resetItems();
            select_index[m_channel] = i+1;
            break;
        }
    }
}

void CurveEditor::resetItems()
{
    this->clearItems();
    for(int i=0;i<points[m_channel].size();i++)
    {
        Tracer *tracer = new Tracer(this,this->graph(m_channel+1));
        tracer->setGraphKey(points[m_channel][i].x());
        this->addItem(tracer);
    }
}

void CurveEditor::setChannel(int channel)
{
    m_channel = channel;
}

void CurveEditor::onReplot()
{
    emit valueChanged(m_value[m_channel]);
}

void CurveEditor::channelChanged()
{
    resetItems();

    if(select_index[m_channel]!=-1)
        this->item(select_index[m_channel])->setSelected(true);

    for(int i=1;i<=4;i++)
    {
        this->graph(i)->setSelected(false);
    }
    this->graph(m_channel+1)->setSelected(true);
}

void CurveEditor::resetChannel(int channel)
{
    m_value[m_channel].clear();
    for(int i=0;i<256;i++)
    {
        m_value[m_channel] << i;
    }
    points[channel].clear();
    points[channel]<<QPointF(0.0,0.0)<<QPointF(255.0,255.0);

    resetItems();
    select_index[channel]=-1;
    this->graph(channel+1)->setData(m_key,m_value[m_channel]);
    this->replot();
}

Tracer::Tracer(QCustomPlot *customPlot,QCPGraph *graph)
    :QCPItemTracer(customPlot)
{
    this->setGraph(graph);
    this->setInterpolating(true);
    this->setStyle(QCPItemTracer::tsCircle);
    this->setPen(QPen(Qt::black));
    this->setSize(8);
    this->setSelectable(true);
    this->setSelectedBrush(Qt::black);
}

void Tracer::setGraphKey(double key)
{
    if(key>=255)
        key=255;
    else if(key<=0)
        key=0;
    QCPItemTracer::setGraphKey(key);
}
