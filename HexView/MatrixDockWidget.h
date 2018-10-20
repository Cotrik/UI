#ifndef MATRIXDOCKWIDGET_H
#define MATRIXDOCKWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QResizeEvent>
#include <QListWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QTreeWidget>
#include <QBrush>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QSlider>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>
#include <QWidget>
#include <QProgressBar>
#include <QGridLayout>
#include <QScrollArea>
#include "QVTKWidget.h"

#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

#include "MouseInteractorStyle.h"

#include <unordered_map>

class MatrixDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    MatrixDockWidget(QMainWindow* &MainWindow, QString name, Qt::DockWidgetArea dockWidgetArea, const bool isPolyData = false);

public:
    //void setupUi(QMainWindow *MainWindow);
    void setupDockWidget(QMainWindow* &MainWindow, QString name,
            QWidget* &sheetWidget,
            QLabel* &sheetLabel,
            QSlider* &OpacitySlider,
            QDoubleSpinBox* &OpacitySpinBox,
            QComboBox* &DisplayComboBox,
            QWidget* &sheetDockWidgetContents,
            QGridLayout* &matrixLayout,
            QScrollArea* &scrollArea,
            Qt::DockWidgetArea dockWidgetArea = Qt::LeftDockWidgetArea);
    
    void setFilesPath(QString path);
    void loadMatrices(int index = 0);
    std::vector<std::vector<double>> getMatrixFromFile(std::string filename);
    void setupMatrixLabels(int n_rows, int n_cols);
    void populateMatrix(int rows, int cols);
    std::vector<std::vector<double>> adjacentMatrix;
    std::vector<std::vector<double>> hybridMatrix;
    std::vector<std::vector<double>> intersectingMatrix;
    std::vector<std::vector<double>> diagonalMatrix;
    std::string file_path;
public:

private:
    QMainWindow *mainWindow;
public slots:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void on_OpacityChanged(int val);
    virtual void on_OpacityChanged(double val);
    virtual void on_DisplayCurrentIndexChanged(int index);
public:
    QWidget *widget;
    QLabel *label;
    QSlider *opacitySlider;
    QDoubleSpinBox *opacitySpinBox;
    QComboBox *displayComboBox;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *layout;
    QScrollArea *scrollArea;
    bool isPolyData;
    std::unordered_map<int, int> listid_id;
    int displayCurrentIndex = 0; // Display Surface
public:
    std::vector<int> cellDataFields;
    QVTKWidget* qvtkWidget;
    vtkNew<vtkRenderer> m_renderer;
    vtkNew<vtkActor> m_vtkActors;
};

#endif // MATRIXDOCKWIDGET_H
