#include "SheetDecompositionsDockWidget.h"
#include "MatrixDockWidget.h"

#include <QDir>
#include <QProcess>
#include <QTreeWidget>

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
#include <fstream>

enum Representation
{
    Surface,
    SurfaceWithEdge,
    Singularity,
    WireFrame
};

SheetDecompositionsDockWidget::SheetDecompositionsDockWidget(QMainWindow* &MainWindow, QString name, Qt::DockWidgetArea dockWidgetArea,
                             QVTKWidget* &qvtkWidget, vtkRenderer* &renderer, const bool isPolyData, std::vector<vtkSmartPointer<vtkActor> > *m_vtkActors)
    : QDockWidget(name, MainWindow)
    , qvtkWidget(qvtkWidget)
    , m_renderer(renderer)
    , loadingBar(NULL)
    , numOfListItems(0)
    , isPolyData(isPolyData)
    , m_vtkActors(m_vtkActors)
{
    setupDockWidget(MainWindow,
                    name,
                    widget,
                    label,
                    opacitySlider,
                    opacitySpinBox,
                    treeWidget,
                    displayComboBox,
                    dockWidgetContents,
                    dockWidgetArea);
//    displayComboBox->addItem(QIcon("../icon/surface.png"), QString("Surface"));
//    displayComboBox->addItem(QIcon("../icon/surfaceWithEdges.png"), QString("Surface With Edges"));
//    displayComboBox->addItem(QIcon("../icon/config.png"), QString("Singularity"));
//    displayComboBox->addItem(QIcon("../icon/wireframe.png"), QString("Wireframe"));
//    displayComboBox->setDisabled(true);

//    QObject::connect(displayComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_DisplayCurrentIndexChanged(int)));
//    QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_OpacityChanged(int)));
//    QObject::connect(opacitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_OpacityChanged(double)));
    QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_Clicked(QTreeWidgetItem *, int)));
    QObject::connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(on_CurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
    if(QTreeWidgetItem* header = treeWidget->headerItem()) header->setText(0, name);
    else treeWidget->setHeaderLabel(name);
}

void SheetDecompositionsDockWidget::setupDockWidget(QMainWindow* &MainWindow, QString name,
        QWidget* &Widget,
        QLabel* &Label,
        QSlider* &OpacitySlider,
        QDoubleSpinBox* &OpacitySpinBox,
        QTreeWidget* &TreeWidget,
        QComboBox* &DisplayComboBox,
        QWidget* &DockWidgetContents,
        Qt::DockWidgetArea dockWidgetArea) {

    mainWindow = MainWindow;
    Widget = new QWidget;
//    OpacitySlider = new QSlider(Widget);
//    OpacitySlider->setObjectName(name + QString::fromUtf8("OpacitySlider"));
//    OpacitySlider->setGeometry(QRect(60, 32, 71, 21));
//    OpacitySlider->setMinimum(0);
//    OpacitySlider->setMaximum(100);
//    OpacitySlider->setValue(100);
//    OpacitySlider->setOrientation(Qt::Horizontal);
//    Label = new QLabel(Widget);
//    Label->setObjectName(QString::fromUtf8("Opacity"));
//    Label->setGeometry(QRect(5, 32, 67, 17));
//    Label->setText(QApplication::translate("MainWindow", "Opacity", 0, QApplication::UnicodeUTF8));
//    OpacitySpinBox = new QDoubleSpinBox(Widget);
//    OpacitySpinBox->setObjectName(name + QString::fromUtf8("OpacitySpinBox"));
//    OpacitySpinBox->setGeometry(QRect(130, 32, 71, 27));
//    OpacitySpinBox->setMaximum(1);
//    OpacitySpinBox->setValue(1);
    TreeWidget = new QTreeWidget(Widget);
    TreeWidget->setObjectName(name + QString::fromUtf8("TreeWidget"));
    TreeWidget->setGeometry(QRect(5, /*70*/5, 220, 450));
    //TreeWidget->setResizeMode(QListView::Adjust);
//    DisplayComboBox = new QComboBox(Widget);
//    DisplayComboBox->setObjectName(name + QString::fromUtf8("DisplayComboBox"));
//    DisplayComboBox->setGeometry(QRect(5, 0, 200, 27));
//    DisplayComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
//    DisplayComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    this->setObjectName(name + QString::fromUtf8("DockWidget"));
    DockWidgetContents = new QWidget();
    DockWidgetContents->setObjectName(name + QString::fromUtf8("DockWidgetContents"));
    this->setWidget(Widget);
    mainWindow->addDockWidget(dockWidgetArea, this);
}

void SheetDecompositionsDockWidget::setActors(std::vector<vtkSmartPointer<vtkActor> >* _vtkActors) {
    this->m_vtkActors = _vtkActors;
}

void SheetDecompositionsDockWidget::setIconFilename(QString filename) {
    iconFilename = filename;
}

void SheetDecompositionsDockWidget::setListItemFileNamePrefix(const std::string prefix) {
    strListItemFileNamePrefix = prefix;
}

void SheetDecompositionsDockWidget::setFolderPath(const std::string path) {
    strFolderPath = path;
}

void SheetDecompositionsDockWidget::resizeEvent(QResizeEvent *event)
{
    const QSize &newsize = event->size();
    treeWidget->setMinimumWidth(newsize.width() - 15);
    treeWidget->setMinimumHeight(newsize.height() > 110 ? newsize.height() - 110 : 10);
    treeWidget->setMaximumWidth(newsize.width());
    treeWidget->setMaximumHeight(newsize.height() > 120 ? newsize.height() - 110 : 20);

    if (loadingBar && numOfListItems != 0) loadingBar->setValue(treeWidget->topLevelItemCount() * 100 / numOfListItems);
}

void SheetDecompositionsDockWidget::on_DisplayCurrentIndexChanged(int index) {
    if (index == Surface)
        for (int id = 0; id < numOfListItems; id++) {
            m_vtkActors->at(id)->GetProperty()->EdgeVisibilityOff();
            m_vtkActors->at(id)->GetProperty()->SetLineWidth(1);
            m_vtkActors->at(id)->GetProperty()->SetRepresentationToSurface();
        }
    else if (index == SurfaceWithEdge)
        for (int id = 0; id < numOfListItems; id++) {
            m_vtkActors->at(id)->GetProperty()->EdgeVisibilityOn();
            m_vtkActors->at(id)->GetProperty()->SetEdgeColor(0, 0, 0);
            m_vtkActors->at(id)->GetProperty()->SetLineWidth(2);
            m_vtkActors->at(id)->GetProperty()->SetRepresentationToSurface();
        }
    else if (index == WireFrame)
        for (int id = 0; id < numOfListItems; id++) {
            m_vtkActors->at(id)->GetProperty()->SetLineWidth(2);
            m_vtkActors->at(id)->GetProperty()->SetRepresentationToWireframe();
        }
    qvtkWidget->update();
}

void SheetDecompositionsDockWidget::on_OpacityChanged(int val) {
    double dval = ((double)val) / opacitySlider->maximum();
    opacitySlider->setValue(val);
    opacitySpinBox->setValue(dval);
    for (int id = 0; id < numOfListItems; id++)
        m_vtkActors->at(id)->GetProperty()->SetOpacity(dval);
    qvtkWidget->update();
}

void SheetDecompositionsDockWidget::on_OpacityChanged(double val) {
    int ival = val * opacitySlider->maximum();
    opacitySpinBox->setValue(val);
    opacitySlider->setValue(ival);
    for (int id = 0; id < numOfListItems; id++)
        m_vtkActors->at(id)->GetProperty()->SetOpacity(val);
    qvtkWidget->update();
}

void SheetDecompositionsDockWidget::on_Clicked(QTreeWidgetItem * item, int column) {
    //QVariant variant = item->data(Qt::UserRole);
    //int data = variant.toInt();
    //QString text = QString("%1:%2 is clicked.").arg(item->text()).arg(data);
    //qDebug() << text;
    std::string str = item->text(column).toStdString();
    int pos = str.find_first_of('_');
    int id = QString(str.substr(pos + 1).c_str()).toInt();
    std::string prefix = str.substr(0, pos);

    if (prefix != "sheet") {
        currentDecompositionId = id;
        for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
            for (int j = 0; j < treeWidget->topLevelItem(i)->childCount(); ++j)
                treeWidget->topLevelItem(i)->child(j)->setCheckState(0, Qt::Unchecked);
        }
        for (auto& sheetids : sheets_coverSheetIds)
            m_renderer->RemoveActor(m_vtkActors->at(sheetids.front()));
        for (auto sheetid : sheets_coverSheetIds[id])
            m_renderer->AddActor(m_vtkActors->at(sheetid));
        int j = 0;
        for (auto sheetid : sheets_coverSheetIds[id])
            treeWidget->topLevelItem(id)->child(j++)->setCheckState(0, Qt::Checked);
    } else {
        int id = QString(str.substr(pos + 1).c_str()).toInt();
        if (item->checkState(column) == Qt::Checked) m_renderer->AddActor(m_vtkActors->at(id));
        else if (item->checkState(column) == Qt::Unchecked) m_renderer->RemoveActor(m_vtkActors->at(id));
    }
    m_renderer->SetBackground(1, 1, 1);
    static bool flag = false;
    if (!flag) {
        qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);
        flag = true;
    }
    qvtkWidget->update();
}

void SheetDecompositionsDockWidget::on_CurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    if (current != NULL) {
        std::string currentstr = current->text(0).toStdString();
        int currentpos = currentstr.find_first_of('_');
        int currentid = QString(currentstr.substr(currentpos + 1).c_str()).toInt();
        std::string currentprefix = currentstr.substr(0, currentpos);

        if (currentprefix != "sheet") {
            currentDecompositionItem = current;
            on_Clicked(current, 0);
            MatrixDockWidget *matrixWidget = mainWindow->findChild<MatrixDockWidget *>("MatrixDockWidget");
            if (matrixWidget) {
                matrixWidget->loadMatrices(currentid);
            }
            return;
        } else if (previous != NULL) {
            std::string previousstr = previous->text(0).toStdString();
            int previouspos = previousstr.find_first_of('_');
            int previousid = QString(previousstr.substr(previouspos + 1).c_str()).toInt();
            std::string previousprefix = previousstr.substr(0, previouspos);

            if (previousprefix != "sheet") {
                for (int j = 0; j < treeWidget->topLevelItem(previousid)->childCount(); ++j) {
                    treeWidget->topLevelItem(previousid)->child(j)->setCheckState(0, Qt::Unchecked);
                    m_renderer->RemoveActor(m_vtkActors->at(sheets_coverSheetIds[previousid].at(j)));
                }
                current->setCheckState(0, Qt::Checked);
                m_renderer->AddActor(m_vtkActors->at(currentid));
            } else {
                current->setCheckState(0, Qt::Checked);
                m_renderer->AddActor(m_vtkActors->at(currentid));
            }
        }
        else {
            current->setCheckState(0, Qt::Checked);
            m_renderer->AddActor(m_vtkActors->at(currentid));
        }
        //else m_renderer->AddActor(m_vtkActors.at(listid_id[currentid]));
    }
//    if (previous != NULL) {
//        std::string previousstr = previous->text(0).toStdString();
//        int previouspos = previousstr.find_first_of('_');
//        int previousid = QString(previousstr.substr(previouspos + 1).c_str()).toInt();
//        if (previouspos == previousstr.npos) ;
//        else if (previous->checkState(0) == Qt::Unchecked) m_renderer->RemoveActor(m_vtkActors->at(previousid));
//    }
    m_renderer->SetBackground(1, 1, 1);
    static bool flag = false;
    if (!flag) {
        qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);
        flag = true;
    }
    qvtkWidget->update();
}

void SheetDecompositionsDockWidget::loadTreeWidgetItems(QString filename, const std::string prefix, QString iconFilename) {
//    while (treeWidget->topLevelItemCount() > 0)
//        delete treeWidget->takeTopLevelItem(0);
    //widget->setEnabled(false);
    treeWidget->clear();
    sheets_coverSheetIds.clear();
    std::string str(filename.toStdString());
    size_t pos = str.find_last_of('/');
    std::string strFolderLocation = str.substr(0, pos);
    strFolderPath = strFolderLocation.c_str();

//    QProcess process;
//    QString cmd = QString("ExtractSheetDecompositions ") + filename;
//    process.startDetached(cmd);
    std::ifstream ifs(strFolderPath + "/sheet_decompositions.txt");
    std::string line;
    while (getline(ifs, line)) {
        std::vector<size_t> decomposition;
        std::stringstream ss(line);
        int num;
        while (ss >> num) decomposition.push_back(num);
        sheets_coverSheetIds.push_back(decomposition);
    }

    for (int i = 0; i < sheets_coverSheetIds.size(); i++) {
//        QTreeWidgetItem* item = new QTreeWidgetItem(QIcon(iconFilename), QString(QString::fromStdString(prefix) + "_%1").arg(i));
//        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
//        item->setCheckState(0, Qt::Unchecked); // AND initialize check state
//        treeWidget->addItem(item);
        std::vector<QString> decomposition;
        for (auto sheetid : sheets_coverSheetIds[i]) decomposition.push_back(QString(QString::fromStdString("sheet") + "_%1").arg(sheetid));
        addTreeRoot(QString(QString("<") + std::to_string(sheets_coverSheetIds[i].size()).c_str() + QString(">") +
                    QString::fromStdString(prefix) + "_%1").arg(i), "Sheet", decomposition);
    }
}

void SheetDecompositionsDockWidget::addTreeRoot(QString name, QString description, std::vector<QString> children)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);

    treeItem->setText(0, name);
    treeItem->setIcon(0, QIcon("../icon/dec.png"));
//    treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable);
//    treeItem->setCheckState(0, Qt::Unchecked);
    for (size_t i = 0; i < children.size(); i++)
        addTreeChild(treeItem, children.at(i), "Child");
}

void SheetDecompositionsDockWidget::addTreeChild(QTreeWidgetItem *parent, QString name, QString description)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    //treeItem->setText(1, description);
    treeItem->setIcon(0, QIcon("../icon/sheets.png"));
    treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable);
    treeItem->setCheckState(0, Qt::Unchecked);
    parent->addChild(treeItem);
}
