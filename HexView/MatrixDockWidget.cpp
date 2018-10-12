#include "MatrixDockWidget.h"
#include "circleWidget.h"
#include "rectWidget.h"

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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

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
                    layout,
                    scrollArea,
                    dockWidgetArea);
    // displayComboBox->addItem(QIcon("../icon/surface.png"), QString("Surface"));
    // displayComboBox->addItem(QIcon("../icon/surfaceWithEdges.png"), QString("Surface With Edges"));
    // displayComboBox->addItem(QIcon("../icon/config.png"), QString("Singularity"));
    // displayComboBox->addItem(QIcon("../icon/wireframe.png"), QString("Wireframe"));
    // displayComboBox->setDisabled(true);

    // qvtkWidget = new QVTKWidget(widget);

    // QObject::connect(displayComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_DisplayCurrentIndexChanged(int)));
    // QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_OpacityChanged(int)));
    // QObject::connect(opacitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_OpacityChanged(double)));
}

void MatrixDockWidget::setupDockWidget(QMainWindow* &MainWindow, QString name,
                                       QWidget* &Widget,
                                       QLabel* &Label,
                                       QSlider* &OpacitySlider,
                                       QDoubleSpinBox* &OpacitySpinBox,
                                       QComboBox* &DisplayComboBox,
                                       QWidget* &DockWidgetContents,
                                       QGridLayout* &matrixLayout,
                                       QScrollArea* &scrollArea,
                                       Qt::DockWidgetArea dockWidgetArea) {
    
    Widget = new QWidget;
    scrollArea = new QScrollArea();
    scrollArea->setBackgroundRole(QPalette::Light);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumHeight(250);
    this->setWidget(scrollArea);
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

void MatrixDockWidget::loadMatrices(QString filename) {
    std::string str(filename.toStdString());
    size_t pos = str.find_last_of('/');
    std::string strFolderLocation = str.substr(0, pos);
    std::string strFolderPath = strFolderLocation.c_str();
    // for(int row = 0; row < 10; row++) {
    //     for (int col = 0; col < 10; col++) {
    //         // CircleWidget* circle = new CircleWidget((row * 25) + 5, (col * 25) + 5, 20, Widget);
    //         RectWidget* rect = new RectWidget((row * 25) + 5, (col * 25) + 5, 20, 20, widget);
    //     }
    // }
    
    adjacentMatrix = getMatrixFromFile(strFolderPath+"/adjacent.mat");
    hybridMatrix = getMatrixFromFile(strFolderPath+"/hybrid.mat");
    intersectingMatrix = getMatrixFromFile(strFolderPath+"/intersecting.mat");
    diagonalMatrix = getMatrixFromFile(strFolderPath+"/diagonal.mat");
    int n_rows = adjacentMatrix.size() + 1;
    int n_cols = adjacentMatrix[0].size() + 1;

    // labels for matrix
    setupMatrixLabels(n_rows, n_cols);
    populateMatrix(n_rows - 1, n_cols - 1);
    widget->setMinimumSize(n_rows * 55, n_cols * 55);
    scrollArea->setWidget(widget);
}

void MatrixDockWidget::setupMatrixLabels(int n_rows, int n_cols) {
    for (int i = 0; i < n_rows; i++) {
        if (i == 1) { n_cols = 1; }
        for (int j = 0; j < n_cols; j++) {
            if (i == 0 && j == 0) {
                QLabel* l = new QLabel("", widget);
                l->setGeometry(QRect(0, 0, 25, 25));
            } else if (n_cols == 1) {
                QLabel* l = new QLabel(QString("%1").arg(i - 1), widget);
                l->setGeometry(QRect(0, (i * 25) + 5, 25, 25));
            } else {
                QLabel* l = new QLabel(QString("%1").arg(j - 1), widget);
                l->setGeometry(QRect((j * 25) + 5, 0, 25, 25));
            }
        }
    }
}

void MatrixDockWidget::populateMatrix(int rows, int cols) {
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= cols; j++) {
            if (diagonalMatrix[i - 1][j - 1] > 0) {
                RectWidget* rect = new RectWidget((i * 25) + 5, (j * 25) + 5, diagonalMatrix[i - 1][j - 1], widget);
            }
            if (hybridMatrix[i - 1][j - 1] > 0) {
                CircleWidget* circle = new CircleWidget((i * 25) + 5, (j * 25) + 5, hybridMatrix[i - 1][j - 1], Qt::red, widget);
            }
            if (intersectingMatrix[i - 1][j - 1] > 0) {
                CircleWidget* circle = new CircleWidget((i * 25) + 5, (j * 25) + 5, intersectingMatrix[i - 1][j - 1], Qt::blue, widget);
            }
            if (adjacentMatrix[i - 1][j - 1] > 0) {
                CircleWidget* circle = new CircleWidget((i * 25) + 5, (j * 25) + 5, adjacentMatrix[i - 1][j - 1], Qt::green, widget);
            }
        }
    }
}

std::vector<std::vector<double>> MatrixDockWidget::getMatrixFromFile(std::string filename) {
    std::vector<std::vector<double>> matrix;
    std::vector<double> temp;
    std::string line;
    double input;
    ifstream inFile;
    inFile.open(filename);
    if (!inFile) {
        cout << "Unable to open file: " + filename << endl;
    }
    while (std::getline(inFile, line)) {
        std::string number;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] == '\t' || line[i] == ' ') {
                temp.push_back(std::stod(number));
                number = "";
                continue;
            }
            number += line[i];
        }
        matrix.push_back(temp);
        temp.clear();
    }
    return matrix;
}
