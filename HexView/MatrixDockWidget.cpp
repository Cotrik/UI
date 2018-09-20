#include "MatrixDockWidget.h"
#include <QDir>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkIdTypeArray.h>
#include <vtkTriangleFilter.h>
#include <vtkTetra.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPlaneSource.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkObjectFactory.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkPolyData.h>

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>

#include <sstream>

enum Representation
{
    Surface,
    SurfaceWithEdge,
    Singularity,
    WireFrame
};

MatrixDockWidget::MatrixDockWidget(QMainWindow* &MainWindow, QString name, Qt::DockWidgetArea dockWidgetArea, const bool isPolyData)
    : QDockWidget(name, MainWindow)
    , isPolyData(isPolyData)
{
    this->setMinimumWidth(200);
    setupDockWidget(MainWindow, name,
                    widget,
                    label,
                    opacitySlider,
                    opacitySpinBox,
                    displayComboBox,
                    dockWidgetContents,
                    dockWidgetArea);
    displayComboBox->addItem(QIcon("../icon/surface.png"), QString("Surface"));
    displayComboBox->addItem(QIcon("../icon/surfaceWithEdges.png"), QString("Surface With Edges"));
    displayComboBox->addItem(QIcon("../icon/config.png"), QString("Singularity"));
    displayComboBox->addItem(QIcon("../icon/wireframe.png"), QString("Wireframe"));
    displayComboBox->setDisabled(true);

    qvtkWidget = new QVTKWidget(widget);

    QObject::connect(displayComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_DisplayCurrentIndexChanged(int)));
    QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_OpacityChanged(int)));
    QObject::connect(opacitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_OpacityChanged(double)));
}

void MatrixDockWidget::setupDockWidget(QMainWindow* &MainWindow, QString name,
                                       QWidget* &Widget,
                                       QLabel* &Label,
                                       QSlider* &OpacitySlider,
                                       QDoubleSpinBox* &OpacitySpinBox,
                                       QComboBox* &DisplayComboBox,
                                       QWidget* &DockWidgetContents,
                                       Qt::DockWidgetArea dockWidgetArea) {
    Widget = new QWidget;
    OpacitySlider = new QSlider(Widget);
    OpacitySlider->setObjectName(name + QString::fromUtf8("OpacitySlider"));
    OpacitySlider->setGeometry(QRect(60, 32, 71, 21));
    OpacitySlider->setMinimum(0);
    OpacitySlider->setMaximum(100);
    OpacitySlider->setValue(100);
    OpacitySlider->setOrientation(Qt::Horizontal);
    Label = new QLabel(Widget);
    Label->setObjectName(QString::fromUtf8("Opacity"));
    Label->setGeometry(QRect(5, 32, 67, 17));
    //Label->setText(QApplication::translate("MainWindow", "Opacity", 0, /*QApplication::UnicodeUTF8*/0));
    OpacitySpinBox = new QDoubleSpinBox(Widget);
    OpacitySpinBox->setObjectName(name + QString::fromUtf8("OpacitySpinBox"));
    OpacitySpinBox->setGeometry(QRect(130, 32, 71, 27));
    OpacitySpinBox->setMaximum(1);
    OpacitySpinBox->setValue(1);
//    ListWidget = new QListWidget(Widget);
//    ListWidget->setObjectName(name + QString::fromUtf8("ListWidget"));
//    ListWidget->setGeometry(QRect(5, 70, 220, 450));
//    ListWidget->setResizeMode(QListView::Adjust);
//    //ListWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
//    //ListWidget->setMinimumSize(200, 200);
    DisplayComboBox = new QComboBox(Widget);
    DisplayComboBox->setObjectName(name + QString::fromUtf8("DisplayComboBox"));
    DisplayComboBox->setGeometry(QRect(5, 0, 200, 27));
    DisplayComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    DisplayComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QWidget *centralwidget = new QWidget(Widget);
    centralwidget->setObjectName(QString::fromUtf8("matrixwidget"));
    qvtkWidget = new QVTKWidget(centralwidget);
    qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
    qvtkWidget->setGeometry(QRect(10, 70, 500, 500));
//    qvtkWidget = new QVTKWidget(Widget);
//    qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
    Label = new QLabel(Widget);
    Label->setObjectName(QString::fromUtf8("Opacity"));
    Label->setGeometry(QRect(5, 32, 67, 17));
    //Label->setText(QApplication::translate("MainWindow", "Opacity", 0, /*QApplication::UnicodeUTF8*/0));
    this->setObjectName(name + QString::fromUtf8("DockWidget"));
    DockWidgetContents = new QWidget();
    DockWidgetContents->setObjectName(name + QString::fromUtf8("DockWidgetContents"));
    this->setWidget(Widget);
    MainWindow->addDockWidget(dockWidgetArea, this);
}

void MatrixDockWidget::resizeEvent(QResizeEvent *event)
{
//    QDockWidget:resizeEvent(event);
    const QSize &newsize = event->size();
}

void MatrixDockWidget::on_DisplayCurrentIndexChanged(int index) {
    displayCurrentIndex = index;
    if (index == Surface) {
        m_vtkActors->GetProperty()->EdgeVisibilityOff();
        m_vtkActors->GetProperty()->SetLineWidth(6);
        m_vtkActors->GetProperty()->SetRepresentationToSurface();
    }
    else if (index == SurfaceWithEdge) {
        m_vtkActors->GetProperty()->EdgeVisibilityOn();
        m_vtkActors->GetProperty()->SetEdgeColor(0, 0, 0);
        m_vtkActors->GetProperty()->SetLineWidth(8);
        m_vtkActors->GetProperty()->SetRepresentationToSurface();
        m_vtkActors->GetProperty()->SetRenderLinesAsTubes(1);
    }
    else if (index == WireFrame) {
        m_vtkActors->GetProperty()->SetLineWidth(2);
        m_vtkActors->GetProperty()->SetRepresentationToWireframe();
    }
    qvtkWidget->update();
}

void MatrixDockWidget::on_OpacityChanged(int val) {
    double dval = ((double)val) / opacitySlider->maximum();
    opacitySlider->setValue(val);
    opacitySpinBox->setValue(dval);
    m_vtkActors->GetProperty()->SetOpacity(dval);
    qvtkWidget->update();
}

void MatrixDockWidget::on_OpacityChanged(double val) {
    int ival = val * opacitySlider->maximum();
    opacitySpinBox->setValue(val);
    opacitySlider->setValue(ival);
    m_vtkActors->GetProperty()->SetOpacity(val);
    qvtkWidget->update();
}
