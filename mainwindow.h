#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Picture;
class RecentOpened;
class ToolLayerManager;
class UndoHistory;

QT_BEGIN_NAMESPACE
class QUndoGroup;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initMenu();

    void loadImage(const QString &fileName);

private slots:
    void setScale(double scale);

    void onCurrentChanged(int index);

    void onTabClose(int index);

private slots:
    void on_actionOpenImage_triggered();

    void actionRecent_triggered(int index);

    void on_actionExit_triggered();

    void on_actionSave_triggered();

    void on_actionCloseImage_triggered();

    void on_actionClockwiseRotate90_triggered();

    void on_actionAntiClockwiseRotate90_triggered();

    void on_actionRotate180_triggered();

    void on_actionRotateZoom_triggered();

    void on_actionFlipHorizontal_triggered();

    void on_actionFlipVertical_triggered();

    void on_actionPerspective_triggered();

    void on_actionGrayScale_triggered();

    void on_actionInverting_triggered();

    void on_actionThreshold_triggered();

    void on_actionColorBalance_triggered();

    void on_actionBrightAndContrasty_triggered();

    void on_actionPosterize_triggered();

    void on_actionColorize_triggered();

    void on_actionHSL_triggered();

    void on_actionColorEnhance_triggered();

    void on_actionEqualize_triggered();

    void on_actionLevels_triggered();

    void on_actionCurve_triggered();

    void on_actionWhiteBalance_triggered();

    void on_actionClear_triggered();

    void on_actionSaveAs_triggered();

    void on_actionRectangle_toggled(bool checked);

    void on_actionEllipse_toggled(bool checked);

    void on_actionLasso_toggled(bool checked);

    void on_actionBrowse_toggled(bool arg1);

    void on_actionHistogram_triggered();

    void on_actionZoomBlur_triggered();

    void on_actionRadialBlur_triggered();

    void on_actionGaussianBlur_triggered();

    void on_actionMotionBlur_triggered();

    void on_actionSurfaceBlur_triggered();

    void on_actionMosaic_triggered();

    void on_actionSobelEdge_triggered();

    void on_actionLaplacianEdge_triggered();

    void on_actionCanny_triggered();

    void on_actionCanvas_triggered();

    void on_actionRedEyeRemoval_triggered();

    void on_actionSharpen_triggered();

    void on_actionUnsharpMask_triggered();

    void on_actionEmboss_triggered();

    void on_actionOpenAsLayer_triggered();

private:
    Ui::MainWindow *ui;
    QUndoGroup *m_undoGroup;
    RecentOpened *m_recent;
    ToolLayerManager *m_layerMng;
    UndoHistory *m_undoHistory;
    QList<Picture*> m_picList;
    int m_curIndex;
};

#endif // MAINWINDOW_H
