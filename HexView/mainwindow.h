#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QListWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QTreeWidget>
#include <QBrush>
#include <QtGui/QComboBox>

#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

#include "HexDockWidget.h"
#include "MouseInteractorStyle.h"

struct RepresentationProperty
{
    bool bWireFrame;
    bool bSurface;
    bool bSurfaceWithEdge;
    bool bVolume;
};

enum Representation
{
    Surface,
    SurfaceWithEdge,
    Singularity,
    WireFrame
};

class LoadingThread : public QObject
{
    Q_OBJECT
public:
    LoadingThread(HexDockWidget* hexDockWidget, const std::string path, const std::string prefix, QString iconFilename);
    ~LoadingThread();
public slots:
    void process();
private:
    HexDockWidget* hexDockWidget;
    std::string strFolderPath;
    std::string prefix;
    QString iconFilename;
signals:
    void finished();
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* listMenu;

    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;

    QAction* wireFrameAction;
    QAction* surfaceAction;
    QAction* surfaceWithEdgeAction;
    QAction* volumeAction;

    QAction* sheetListAction;
    QAction* componentListAction;
    QAction* baseComplexAction;
    QAction* singularityAction;

    QAction* stopLoadingAction;
    QAction* openFolderAction;

    QToolBar* toolBar;
    QSlider* opacitySlider;
    QDoubleSpinBox* opacitySpinBox;

    QSlider* opacityBaseComplexSlider;
    QDoubleSpinBox* opacityBaseComplexSpinBox;

    QSlider* opacitySingularitySlider;
    QDoubleSpinBox* opacitySingularitySpinBox;
    QComboBox* sheetCellDataFieldComboBox;
    ///////////////////////////////////
    // vtk member variables
    vtkNew<vtkPolyDataReader> m_vtkBaseComplexReader;
    vtkNew<vtkPolyDataReader> m_vtkSingularityReader;
    vtkNew<vtkUnstructuredGridReader> m_vtkUnstructuredGridReader;
    vtkNew<vtkUnstructuredGridWriter> m_vtkUnstructuredGridWriter;
    vtkNew<vtkDataSetMapper> m_vtkDataSetMapper;
    vtkNew<vtkActor> m_vtkActor;
    vtkNew<vtkActor> m_vtkBaseComplexActor;
    vtkNew<vtkActor> m_vtkSingularityActor;
    vtkRenderer* m_renderer;
    vtkNew<InteractorStyleTrackballCamera> m_mouseInteractorStyle;

    LoadingThread * componentLoadingThread;
    LoadingThread * chordLoadingThread;
    LoadingThread * sheetLoadingThread;
    LoadingThread * chordFaceAndEdgeLoadingThread;
    LoadingThread * chordCurveLoadingThread;
    LoadingThread * sheetFaceAndEdgeLoadingThread;
    LoadingThread * sheetDualLoadingThread;

    QThread * componentThread;
    QThread * chordThread;
    QThread * sheetThread;
    QThread * chordFaceAndEdgeThread;
    QThread * chordCurveThread;
    QThread * sheetFaceAndEdgeThread;
    QThread * sheetDualThread;

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void addTreeRoot(QString name, QString description, std::vector<QString> children);
    void addTreeChild(QTreeWidgetItem *parent, QString name, QString description);
    void deleteAllItems(QTreeWidget *qTreeWidget);

public:
    virtual void Update();
    virtual std::vector<std::vector<int> > GetScalarFields(vtkSmartPointer<vtkUnstructuredGridReader> pReader, int numOfFields = 4);
    virtual std::vector<std::vector<int> > GetScalarFields(vtkSmartPointer<vtkPolyDataReader> pReader, int numOfFields = 1);
    virtual void AddBaseComplexActor(const char* filename);
    virtual void AddSingularityActor(const char* filename);
    //virtual void DisplaySheets(const std::vector<int>& sheetIndices);
    virtual void DeactivateAllSheets();
    virtual bool IsSheetsEmpty() const;
    void ModifyColorMap(HexDockWidget* dockWidget, vtkSmartPointer<vtkLookupTable> colorLookupTable, double tableMaxRange, int fieldId, const char* iconFilename);
    void runThread(QThread* thread, LoadingThread* loadingThread);
private:
    QString strFilename;
    std::string strFolderPath;

private slots:
    virtual void resizeEvent(QResizeEvent *event);  // 改变大小

    virtual void slotExit();
    // File Menu
    virtual void on_home_clicked();
    virtual void on_fileOpen_clicked();
    virtual bool on_fileSave_clicked();
    virtual bool on_fileSaveAs_clicked();
    // Edit Menu
    virtual void on_wireFrameAction_clicked();
    virtual void on_surfaceAction_clicked();
    virtual void on_surfaceWithEdgeAction_clicked();
    virtual void on_volumeAction_clicked();
    // List Menu
    virtual void on_sheetListAction_clicked();
    virtual void on_componentListAction_clicked();
    virtual void on_baseComplexAction_clicked();
    virtual void on_singularityAction_clicked();

    virtual void on_stopLoading_clicked();
    virtual void on_openFolder_clicked();

    virtual void on_opacityChanged(int val);
    virtual void on_opacityChanged(double val);
    virtual void on_opacityBaseComplexChanged(int val);
    virtual void on_opacityBaseComplexChanged(double val);
    virtual void on_opacitySingularityChanged(int val);
    virtual void on_opacitySingularityChanged(double val);

    void on_sheetCellDataFieldCurrentIndexChanged(int index);
    virtual void on_ModelClicked(QListWidgetItem* item);
};
extern MainWindow* g_mainWindow;
extern Ui::MainWindow* g_ui;
#endif // MAINWINDOW_H
