#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Picture.h"
#include "Layer.h"
#include "PixmapItem.h"
#include "graphicsview.h"
#include "Perspective.h"
#include "TransRotateZoom.h"
#include "ColorThreshold.h"
#include "ColorBalance.h"
#include "ColorBrightnessContrast.h"
#include "ColorPosterize.h"
#include "ColorColorize.h"
#include "ColorHueSaturation.h"
#include "ColorLevels.h"
#include "ColorCurve.h"
#include "ToolHistogram.h"
#include "FilterZoomBlur.h"
#include "FilterRadialBlur.h"
#include "FilterGaussianBlur.h"
#include "FilterMotionBlur.h"
#include "FilterSurfaceBlur.h"
#include "FilterMosaic.h"
#include "FilterRedEyeRemoval.h"
#include "FilterUnsharpMask.h"
#include "FilterEmboss.h"
#include "Commands.h"
#include "RecentOpened.h"
#include "StatusBar.h"
#include "ToolLayerManager.h"
#include "UndoHistory.h"
#include <QGraphicsScene>
#include <QFileDialog>
#include <QDebug>
#include <QUndoGroup>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QSettings>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_undoGroup = new QUndoGroup(this);
    m_curIndex = -1;

    m_layerMng = new ToolLayerManager();
    m_undoHistory = new UndoHistory();
    m_layerMng->setEnabled(false);
    m_undoHistory->setEnabled(false);
    m_undoHistory->setUndoGroup(m_undoGroup);
    m_recent = new RecentOpened(ui->menu_recent);

    this->addDockWidget(Qt::RightDockWidgetArea,m_layerMng);
    this->addDockWidget(Qt::RightDockWidgetArea,m_undoHistory);
    ui->statusBar->widget()->resetStatusBar(false);

    initMenu();
    connect(m_recent,&RecentOpened::SelectedItem,this,&MainWindow::actionRecent_triggered);
    connect(ui->statusBar->widget(),&StatusBar::sendScale,this,&MainWindow::setScale);
    connect(ui->tabWidget,&QTabWidget::currentChanged,this,&MainWindow::onCurrentChanged);
    connect(ui->tabWidget,&QTabWidget::tabCloseRequested,this,&MainWindow::onTabClose);
}

MainWindow::~MainWindow()
{
    delete ui;
    SafeDelete(m_recent);
    SafeDelete(m_layerMng);
    SafeDelete(m_undoHistory);
    SafeDelete(m_undoGroup);
    for(int i=0;i<m_picList.count();i++)
        SafeDelete(m_picList[i]);
}

/**
 * @brief 初始化菜单
 */
void MainWindow::initMenu()
{
    QAction *undo = m_undoGroup->createUndoAction(this, tr("撤销(&U)"));
    undo->setShortcut(QKeySequence::Undo);
    undo->setIcon(QIcon(":/icon/edit/Image/stock-edit-undo.png"));
    QAction *redo = m_undoGroup->createRedoAction(this, tr("重做(&R)"));
    redo->setShortcut(QKeySequence::Redo);
    redo->setIcon(QIcon(":/icon/edit/Image/stock-edit-redo.png"));

    ui->menu_edit->addAction(undo);
    ui->menu_edit->addAction(redo);
    ui->mainToolBar->addAction(undo);
    ui->mainToolBar->addAction(redo);
    ui->mainToolBar->addSeparator();

    ui->menu_view->addAction(m_layerMng->toggleViewAction());
    ui->menu_view->addAction(m_undoHistory->toggleViewAction());

    QActionGroup *group = new QActionGroup(this);
    group->addAction(ui->actionRectangle);
    group->addAction(ui->actionEllipse);
    group->addAction(ui->actionLasso);
    group->addAction(ui->actionBrowse);
}

void MainWindow::setScale(double scale)
{
    GraphicsView *graph = (GraphicsView*)ui->tabWidget->widget(m_curIndex);
    graph->resetTransform();
    graph->scale(scale,scale);
}

void MainWindow::onCurrentChanged(int index)
{
    m_curIndex = index;
    if(m_curIndex>=0)
    {
        // 使工具箱可用
        m_layerMng->setEnabled(true);
        m_undoHistory->setEnabled(true);
        // 切换状态栏信息
        ui->statusBar->widget()->resetStatusBar(true);
        ui->statusBar->widget()->setImageSize(QString("%1,%2").arg(m_picList[m_curIndex]->getWidth()).arg(m_picList[m_curIndex]->getHeight()));
        // 切换图片时，设置相应的撤销重做栈
        m_undoGroup->setActiveStack(m_picList[m_curIndex]->getUndoStack());
        // 修改窗口标题为文件名
        this->setWindowTitle(ui->tabWidget->tabText(index));
        // 切换用以显示图层的list Model
        m_layerMng->switchModel(m_picList[m_curIndex]);

        for(int i=0;i<m_picList.count();i++)
        {
            if(i == index)
            {
                connect(m_picList[m_curIndex]->Model(),&QStandardItemModel::itemChanged,m_layerMng,&ToolLayerManager::checkStateChanged);
                connect(m_picList[m_curIndex],&Picture::setCurrentItem,m_layerMng,&ToolLayerManager::setCurrentIndex);
            }
            else
            {
                disconnect(m_picList[i]->Model(),&QStandardItemModel::itemChanged,m_layerMng,&ToolLayerManager::checkStateChanged);
                disconnect(m_picList[m_curIndex],&Picture::setCurrentItem,m_layerMng,&ToolLayerManager::setCurrentIndex);
            }
        }

        // 选择工具的切换
        int flag = ((GraphicsView*)ui->tabWidget->currentWidget())->getToolFlag();
        switch(flag)
        {
        case GraphicsView::SELECTOR_NONE:
            ui->actionBrowse->setChecked(true);
            break;
        case GraphicsView::SELECTOR_RECT:
            ui->actionRectangle->setChecked(true);
            break;
        case GraphicsView::SELECTOR_ELLIPSE:
            ui->actionEllipse->setChecked(true);
            break;
        case GraphicsView::SELECTOR_PATH:
            ui->actionLasso->setChecked(true);
            break;
        }
    }
    else
    {
        m_layerMng->setEnabled(false);
        m_undoHistory->setEnabled(false);
        ui->statusBar->widget()->resetStatusBar(true);
        this->setWindowTitle("图像处理");
        ui->actionBrowse->setChecked(true);
    }
}

void MainWindow::onTabClose(int index)
{
    SafeDelete(m_picList[index]);
    m_picList.removeAt(index);
    ui->tabWidget->widget(index)->close();
    ui->tabWidget->removeTab(index);
}

//---------------------------------------------------------//
// 开始菜单
//---------------------------------------------------------//

/**
 * @brief 打开图像
 */
void MainWindow::on_actionOpenImage_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开图像"),
                                                    ".",tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName.isEmpty())
        return;
    m_recent->UpdateList(fileName);
    this->loadImage(fileName);
}

/**
 * @brief 选择最近打开的文件
 * @param index     文件索引
 */
void MainWindow::actionRecent_triggered(int index)
{
    QString fileName;
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("RecentlyOpened");
    fileName = settings.value(QString::number(index)).toString();
    settings.endGroup();
    this->loadImage(fileName);
}

/**
 * @brief 清除最近打开的文件记录
 */
void MainWindow::on_actionClear_triggered()
{
    m_recent->clearActions();
}

/**
 * @brief 载入图片
 * @param fileName  图片路径
 */
void MainWindow::loadImage(const QString &fileName)
{
    GraphicsView *graph = new GraphicsView(ui->tabWidget);
    Picture *pic = new Picture(graph->PixItem(),fileName);
    m_picList.append(pic);
    m_undoGroup->addStack(pic->getUndoStack());
    graph->setSceneRect(graph->PixItem()->boundingRect());

    connect(graph,&GraphicsView::getSelection,pic,&Picture::setMask);
    connect(graph,&GraphicsView::getPosition,ui->statusBar->widget(),&StatusBar::setPosition);
    connect(graph,&GraphicsView::getScale,ui->statusBar->widget(),&StatusBar::setScale);

    QFileInfo info(fileName);
    ui->tabWidget->addTab(graph,info.fileName());
    m_curIndex = ui->tabWidget->indexOf(graph);
    ui->tabWidget->setCurrentIndex(m_curIndex);
}

/**
 * @brief 作为图层打开
 */
void MainWindow::on_actionOpenAsLayer_triggered()
{

}
/**
 * @brief 关闭图像
 */
void MainWindow::on_actionCloseImage_triggered()
{
    this->onTabClose(ui->tabWidget->currentIndex());
}

/**
 * @brief 保存图像
 */
void MainWindow::on_actionSave_triggered()
{
    if(!m_picList.count())
        return;
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("RecentlyOpened");
    QString fileName = settings.value("RecentlyOpened/1").toString();
    QStringList keys = settings.childKeys();
    for(int i=0;i<keys.size();i++)
    {
        fileName = settings.value(keys[i]).toString();
        if(fileName.contains(ui->tabWidget->tabText(ui->tabWidget->currentIndex())))
            break;
    }
    settings.endGroup();
    PixmapItem *item = ((GraphicsView *)ui->tabWidget->currentWidget())->PixItem();
    item->pixmap().save(fileName);
}

/**
 * @brief 图像另存为
 */
void MainWindow::on_actionSaveAs_triggered()
{
    if(!m_picList.count())
        return;
    PixmapItem *item = ((GraphicsView *)ui->tabWidget->currentWidget())->PixItem();

    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("保存"),
                                                    "未命名",
                                                    tr("*.jpg;; *.png;; *.bmp"),
                                                    &selectedFilter);
    if(selectedFilter.contains("bmp"))
        item->pixmap().save(filename+".bmp","bmp");
    else if(selectedFilter.contains("png"))
        item->pixmap().save(filename+".png","png");
    else if(selectedFilter.contains("jpg"))
        item->pixmap().save(filename+".jpg","jpg");
}

/**
 * @brief 退出程序
 */
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

//---------------------------------------------------------//
// 编辑菜单
//---------------------------------------------------------//


//---------------------------------------------------------//
// 变换菜单
//---------------------------------------------------------//

/**
 * @brief 水平翻转
 */
void MainWindow::on_actionFlipHorizontal_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->FlipHorizontal();
}

/**
 * @brief 垂直翻转
 */
void MainWindow::on_actionFlipVertical_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->FlipVertical();
}

/**
 * @brief 顺时针旋转90度
 */
void MainWindow::on_actionClockwiseRotate90_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->ClockwiseRotate90();
}

/**
 * @brief 逆时针旋转90度
 */
void MainWindow::on_actionAntiClockwiseRotate90_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->AntiClockwiseRotate90();
}

/**
 * @brief 旋转180度
 */
void MainWindow::on_actionRotate180_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->Rotate180();
}

/**
 * @brief 旋转-偏移-缩放
 */
void MainWindow::on_actionRotateZoom_triggered()
{
    if(!m_picList.count())
        return;
    TransRotateZoom *trans = new TransRotateZoom(m_picList[m_curIndex],this);
    trans->exec();
}

/**
 * @brief 透视变换
 */
void MainWindow::on_actionPerspective_triggered()
{
    if(!m_picList.count())
        return;
    Perspective *pers = new Perspective(m_picList[m_curIndex],this);
    pers->exec();
}

//---------------------------------------------------------//
// 颜色菜单
//---------------------------------------------------------//

/**
 * @brief 去色，即灰度化
 */
void MainWindow::on_actionGrayScale_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->GrayScale();
}

/**
 * @brief 反相
 */
void MainWindow::on_actionInverting_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->InvertColor();
}

/**
 * @brief 阈值
 */
void MainWindow::on_actionThreshold_triggered()
{
    if(!m_picList.count())
        return;
    ColorThreshold *threshold = new ColorThreshold(m_picList[m_curIndex],this);
    threshold->exec();
}

/**
 * @brief 色彩平衡
 */
void MainWindow::on_actionColorBalance_triggered()
{
    if(!m_picList.count())
        return;
    ColorBalance *balance = new ColorBalance(m_picList[m_curIndex],this);
    balance->exec();
}

/**
 * @brief 亮度-对比度
 */
void MainWindow::on_actionBrightAndContrasty_triggered()
{
    if(!m_picList.count())
        return;
    ColorBrightnessContrast *bc = new ColorBrightnessContrast(m_picList[m_curIndex],this);
    bc->exec();
}

/**
 * @brief 色调分离
 */
void MainWindow::on_actionPosterize_triggered()
{
    if(!m_picList.count())
        return;
    ColorPosterize *posterize = new ColorPosterize(m_picList[m_curIndex],this);
    posterize->exec();
}

/**
 * @brief 单色化
 */
void MainWindow::on_actionColorize_triggered()
{
    if(!m_picList.count())
        return;
    ColorColorize *colorize = new ColorColorize(m_picList[m_curIndex],this);
    colorize->exec();
}

/**
 * @brief 色调-亮度-饱和度
 */
void MainWindow::on_actionHSL_triggered()
{
    if(!m_picList.count())
        return;
    ColorHueSaturation *hsl = new ColorHueSaturation(m_picList[m_curIndex],this);
    hsl->exec();
}

/**
 * @brief 色调均化
 */
void MainWindow::on_actionEqualize_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->Equalize();
}

/**
 * @brief 白平衡
 */
void MainWindow::on_actionWhiteBalance_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->WhiteBalance();
}

/**
 * @brief 颜色增强
 */
void MainWindow::on_actionColorEnhance_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->ColorEnhance();
}

/**
 * @brief 色阶
 */
void MainWindow::on_actionLevels_triggered()
{
    if(!m_picList.count())
        return;
    ColorLevels *levels = new ColorLevels(m_picList[m_curIndex],((GraphicsView*)ui->tabWidget->currentWidget())->PixItem());
    levels->show();
}

/**
 * @brief 曲线
 */
void MainWindow::on_actionCurve_triggered()
{
    if(!m_picList.count())
        return;
    ColorCurve *curve = new ColorCurve(m_picList[m_curIndex],this);
    curve->exec();
}

//---------------------------------------------------------//
// 工具菜单
//---------------------------------------------------------//

/**
 * @brief 矩形选择工具
 */
void MainWindow::on_actionRectangle_toggled(bool checked)
{
    if(!m_picList.count())
    {
        ui->actionBrowse->setChecked(true);
        return;
    }
    GraphicsView *graph = (GraphicsView*)ui->tabWidget->currentWidget();

    if(checked)
    {
        graph->setToolFlag(GraphicsView::SELECTOR_RECT);
    }
    else
    {
        graph->setToolFlag(GraphicsView::SELECTOR_NONE);
    }
}

/**
 * @brief 椭圆选择工具
 */
void MainWindow::on_actionEllipse_toggled(bool checked)
{
    if(!m_picList.count())
    {
        ui->actionBrowse->setChecked(true);
        return;
    }
    GraphicsView *graph = (GraphicsView*)ui->tabWidget->currentWidget();
    if(checked)
    {
        graph->setToolFlag(GraphicsView::SELECTOR_ELLIPSE);
    }
    else
    {
        graph->setToolFlag(GraphicsView::SELECTOR_NONE);
    }
}

/**
 * @brief 套索选择工具
 */
void MainWindow::on_actionLasso_toggled(bool checked)
{
    if(!m_picList.count())
    {
        ui->actionBrowse->setChecked(true);
        return;
    }
    GraphicsView *graph = (GraphicsView*)ui->tabWidget->currentWidget();
    if(checked)
        graph->setToolFlag(GraphicsView::SELECTOR_PATH);
    else
        graph->setToolFlag(GraphicsView::SELECTOR_NONE);
}

/**
 * @brief 浏览工具
 */
void MainWindow::on_actionBrowse_toggled(bool checked)
{
    if(!m_picList.count())
    {
        ui->actionBrowse->setChecked(true);
        return;
    }
    GraphicsView *graph = (GraphicsView*)ui->tabWidget->currentWidget();
    if(checked)
    {
        graph->setDragMode(QGraphicsView::ScrollHandDrag);
        graph->setToolFlag(GraphicsView::SELECTOR_NONE);
    }
}

/**
 * @brief 直方图
 */
void MainWindow::on_actionHistogram_triggered()
{
    if(!m_picList.count())
        return;
    ToolHistogram *histogram = new ToolHistogram(m_picList[m_curIndex]->getCurrentLayer());
    histogram->show();
}

//---------------------------------------------------------//
// 滤镜菜单
//---------------------------------------------------------//

/**
 * @brief 径向模糊
 */
void MainWindow::on_actionZoomBlur_triggered()
{
    if(!m_picList.count())
        return;
    FilterZoomBlur *blur = new FilterZoomBlur(m_picList[m_curIndex],this);
    blur->exec();
}

/**
 * @brief 旋转模糊
 */
void MainWindow::on_actionRadialBlur_triggered()
{
    if(!m_picList.count())
        return;
    FilterRadialBlur *blur = new FilterRadialBlur(m_picList[m_curIndex],this);
    blur->exec();
}

/**
 * @brief 高斯模糊
 */
void MainWindow::on_actionGaussianBlur_triggered()
{
    if(!m_picList.count())
        return;
    FilterGaussianBlur *blur = new FilterGaussianBlur(m_picList[m_curIndex],this);
    blur->exec();
}

/**
 * @brief 运动模糊
 */
void MainWindow::on_actionMotionBlur_triggered()
{
    if(!m_picList.count())
        return;
    FilterMotionBlur *blur = new FilterMotionBlur(m_picList[m_curIndex],this);
    blur->exec();
}

/**
 * @brief 表面模糊
 */
void MainWindow::on_actionSurfaceBlur_triggered()
{
    if(!m_picList.count())
        return;
    FilterSurfaceBlur *blur = new FilterSurfaceBlur(m_picList[m_curIndex],this);
    blur->exec();
}

/**
 * @brief 马赛克
 */
void MainWindow::on_actionMosaic_triggered()
{
    if(!m_picList.count())
        return;
    FilterMosaic *blur = new FilterMosaic(m_picList[m_curIndex],this);
    blur->exec();
}

/**
 * @brief Sobel边缘检测
 */
void MainWindow::on_actionSobelEdge_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->SobelEdge();
}

/**
 * @brief Laplacian边缘检测
 */
void MainWindow::on_actionLaplacianEdge_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->LaplacianEdge();
}

/**
 * @brief Canny边缘检测
 */
void MainWindow::on_actionCanny_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->CannyEdge();
}

/**
 * @brief 添加画布纹理
 */
void MainWindow::on_actionCanvas_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->Canvas();
}

/**
 * @brief 浮雕
 */
void MainWindow::on_actionEmboss_triggered()
{
    if(!m_picList.count())
        return;
    FilterEmboss *filter = new FilterEmboss(m_picList[m_curIndex]);
    filter->exec();
}


/**
 * @brief 去除红眼
 */
void MainWindow::on_actionRedEyeRemoval_triggered()
{
    if(!m_picList.count())
        return;
    FilterRedEyeRemoval *filter = new FilterRedEyeRemoval(m_picList[m_curIndex],this);
    filter->exec();
}

/**
 * @brief 锐化
 */
void MainWindow::on_actionSharpen_triggered()
{
    if(!m_picList.count())
        return;
    m_picList[m_curIndex]->Sharpen();
}

/**
 * @brief 虚光蒙版
 */
void MainWindow::on_actionUnsharpMask_triggered()
{
    if(!m_picList.count())
        return;
    FilterUnsharpMask *filter = new FilterUnsharpMask(m_picList[m_curIndex],this);
    filter->exec();
}


