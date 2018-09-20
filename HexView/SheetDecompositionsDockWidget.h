#ifndef SHEET_DECOMPOSITIONS_DOCKWIDGET_H
#define SHEET_DECOMPOSITIONS_DOCKWIDGET_H

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
#include "QVTKWidget.h"

#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <unordered_map>

class SheetDecompositionsDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    SheetDecompositionsDockWidget(QMainWindow* &MainWindow, QString name, Qt::DockWidgetArea dockWidgetArea,
                  QVTKWidget* &qvtkWidget, vtkRenderer* &m_renderer, const bool isPolyData = false, std::vector<vtkSmartPointer<vtkActor> >* m_vtkActors = NULL);
    void setupDockWidget(QMainWindow* &MainWindow, QString name,
            QWidget* &sheetWidget,
            QLabel* &sheetLabel,
            QSlider* &sheetOpacitySlider,
            QDoubleSpinBox* &sheetOpacitySpinBox,
            QTreeWidget* &treeWidget,
            QComboBox* &sheetDisplayComboBox,
            QWidget* &sheetDockWidgetContents,
            Qt::DockWidgetArea dockWidgetArea = Qt::LeftDockWidgetArea);
    void setActors(std::vector<vtkSmartPointer<vtkActor> >* _vtkActors);
public:
    void setIconFilename(QString filename);
    void setListItemFileNamePrefix(const std::string prefix);
    void setFolderPath(const std::string path);

    virtual void loadTreeWidgetItems(QString filename, const std::string prefix, QString iconFilename);
    void addTreeRoot(QString name, QString description, std::vector<QString> children);
    void addTreeChild(QTreeWidgetItem *parent, QString name, QString description);
public slots:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void on_Clicked(QTreeWidgetItem* item, int column);
    virtual void on_OpacityChanged(int val);
    virtual void on_OpacityChanged(double val);
    virtual void on_DisplayCurrentIndexChanged(int index);
    virtual void on_CurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
signals:
    void finishLoading();

public:
    QWidget *widget;
    QLabel *label;
    QSlider *opacitySlider;
    QDoubleSpinBox *opacitySpinBox;
    //QListWidget *listWidget;
    QTreeWidget *treeWidget;
    QComboBox *displayComboBox;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QProgressBar* loadingBar;
    bool isPolyData;
public:
    QString iconFilename;
    std::string strListItemFileNamePrefix;
    std::string strFolderPath;
    size_t numOfListItems;
    std::vector<std::vector<std::vector<int > > > cellDataFields;
    QVTKWidget *qvtkWidget;
    vtkRenderer* m_renderer;
    std::vector<vtkSmartPointer<vtkActor> >* m_vtkActors;
public:

    std::vector<std::vector<size_t>> sheets_coverSheetIds; // begin with sheetId, find a list of neighbor sheets that cover all components;
    QTreeWidgetItem *currentDecompositionItem;
    int currentDecompositionId = 0;
    QTreeWidgetItem *currentSheetItem;
};

#endif // SHEET_DECOMPOSITIONS_DOCKWIDGET_H
