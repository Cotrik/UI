#include "HexDockWidget.h"
//#include "mainwindow.h"
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

HexDockWidget::HexDockWidget(QMainWindow* &MainWindow, QString name, Qt::DockWidgetArea dockWidgetArea,
                             QVTKWidget* &qvtkWidget, vtkRenderer* &renderer, const bool isPolyData)
    : QDockWidget(name, MainWindow)
    , qvtkWidget(qvtkWidget)
    , m_renderer(renderer)
    , loadingBar(nullptr)
    , numOfListItems(0)
    , isPolyData(isPolyData)
{
    setupDockWidget(MainWindow,
                    name,
                    widget,
                    label,
                    opacitySlider,
                    opacitySpinBox,
                    listWidget,
                    displayComboBox,
                    dockWidgetContents,
                    dockWidgetArea);
    displayComboBox->addItem(QIcon("../icon/surface.png"), QString("Surface"));
    displayComboBox->addItem(QIcon("../icon/surfaceWithEdges.png"), QString("Surface With Edges"));
    displayComboBox->addItem(QIcon("../icon/config.png"), QString("Singularity"));
    displayComboBox->addItem(QIcon("../icon/wireframe.png"), QString("Wireframe"));
    displayComboBox->setDisabled(true);

    QObject::connect(displayComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_DisplayCurrentIndexChanged(int)));
    QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_OpacityChanged(int)));
    QObject::connect(opacitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_OpacityChanged(double)));
    QObject::connect(listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(on_Clicked(QListWidgetItem *)));
    QObject::connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(on_CurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));

}

void HexDockWidget::setupDockWidget(QMainWindow* &MainWindow, QString name,
        QWidget* &Widget,
        QLabel* &Label,
        QSlider* &OpacitySlider,
        QDoubleSpinBox* &OpacitySpinBox,
        QListWidget* &ListWidget,
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
    ListWidget = new QListWidget(Widget);
    ListWidget->setObjectName(name + QString::fromUtf8("ListWidget"));
    ListWidget->setGeometry(QRect(5, 70, 220, 450));
    ListWidget->setResizeMode(QListView::Adjust);
    //ListWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    //ListWidget->setMinimumSize(200, 200);
    DisplayComboBox = new QComboBox(Widget);
    DisplayComboBox->setObjectName(name + QString::fromUtf8("DisplayComboBox"));
    DisplayComboBox->setGeometry(QRect(5, 0, 200, 27));
    DisplayComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    DisplayComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    this->setObjectName(name + QString::fromUtf8("DockWidget"));
    DockWidgetContents = new QWidget();
    DockWidgetContents->setObjectName(name + QString::fromUtf8("DockWidgetContents"));
    this->setWidget(Widget);
    MainWindow->addDockWidget(dockWidgetArea, this);
}

void HexDockWidget::setIconFilename(QString filename) {
    iconFilename = filename;
}

void HexDockWidget::setListItemFileNamePrefix(const std::string prefix) {
    strListItemFileNamePrefix = prefix;
}

void HexDockWidget::setFolderPath(const std::string path) {
    strFolderPath = path;
}

void HexDockWidget::resizeEvent(QResizeEvent *event)
{
//    QDockWidget:resizeEvent(event);
    const QSize &newsize = event->size();
    listWidget->setMinimumWidth(newsize.width() - 15);
    listWidget->setMinimumHeight(newsize.height() > 110 ? newsize.height() - 110 : 10);
    listWidget->setMaximumWidth(newsize.width());
    listWidget->setMaximumHeight(newsize.height() > 120 ? newsize.height() - 110 : 20);

//    this->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
//    this->setMinimumWidth(200);
//    this->setMaximumWidth(250);

    if (loadingBar && numOfListItems != 0) loadingBar->setValue(listWidget->count() * 100 / numOfListItems);
}

void HexDockWidget::on_DisplayCurrentIndexChanged(int index) {
    displayCurrentIndex = index;
    if (index == Surface)
        for (int id = 0; id < numOfListItems; id++) {
            m_vtkActors.at(id)->GetProperty()->EdgeVisibilityOff();
            m_vtkActors.at(id)->GetProperty()->SetLineWidth(6);
            m_vtkActors.at(id)->GetProperty()->SetRepresentationToSurface();
        }
    else if (index == SurfaceWithEdge)
        for (int id = 0; id < numOfListItems; id++) {
            m_vtkActors.at(id)->GetProperty()->EdgeVisibilityOn();
            m_vtkActors.at(id)->GetProperty()->SetEdgeColor(0, 0, 0);
            if (isQuadDual)
                m_vtkActors.at(id)->GetProperty()->SetLineWidth(8);
            else if (isSingularity)
                m_vtkActors.at(id)->GetProperty()->SetLineWidth(6);
            else
                m_vtkActors.at(id)->GetProperty()->SetLineWidth(2);
            m_vtkActors.at(id)->GetProperty()->SetRepresentationToSurface();
            m_vtkActors.at(id)->GetProperty()->SetRenderLinesAsTubes(1);
        }
    else if (index == WireFrame)
        for (int id = 0; id < numOfListItems; id++) {
            m_vtkActors.at(id)->GetProperty()->SetLineWidth(2);
            m_vtkActors.at(id)->GetProperty()->SetRepresentationToWireframe();
        }
    qvtkWidget->update();
}

void HexDockWidget::on_OpacityChanged(int val) {
    double dval = ((double)val) / opacitySlider->maximum();
    opacitySlider->setValue(val);
    opacitySpinBox->setValue(dval);
    for (int id = 0; id < numOfListItems; id++)
        m_vtkActors.at(id)->GetProperty()->SetOpacity(dval);
    qvtkWidget->update();
}

void HexDockWidget::on_OpacityChanged(double val) {
    int ival = val * opacitySlider->maximum();
    opacitySpinBox->setValue(val);
    opacitySlider->setValue(ival);
    for (int id = 0; id < numOfListItems; id++)
        m_vtkActors.at(id)->GetProperty()->SetOpacity(val);
    qvtkWidget->update();
}

void HexDockWidget::on_Clicked(QListWidgetItem * item) {
    QVariant variant = item->data(Qt::UserRole);
    int data = variant.toInt();
    //QString text = QString("%1:%2 is clicked.").arg(item->text()).arg(data);
    //qDebug() << text;
    std::string str = item->text().toStdString();
    int pos = str.find_first_of('_');
    int id = QString(str.substr(pos + 1).c_str()).toInt();

    if (pos == str.npos) {
        for (int id = 0; id < numOfListItems; id++)
            if (item->checkState() == Qt::Checked) {
                m_renderer->AddActor(m_vtkActors.at(id));
                for (int i = 0; i < listWidget->count(); i++)
                    listWidget->item(i)->setCheckState(Qt::Checked);
            }
            else if (item->checkState() == Qt::Unchecked){
                m_renderer->RemoveActor(m_vtkActors.at(id));
                for (int i = 0; i < listWidget->count(); i++)
                    listWidget->item(i)->setCheckState(Qt::Unchecked);
            }
    } else {
        int id = QString(str.substr(pos + 1).c_str()).toInt();
        if (item->checkState() == Qt::Checked) m_renderer->AddActor(m_vtkActors.at(listid_id[id]));
        else if (item->checkState() == Qt::Unchecked) m_renderer->RemoveActor(m_vtkActors.at(listid_id[id]));
    }
    m_renderer->SetBackground(1, 1, 1);
    static bool flag = false;
    if (!flag) {
        qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);
        flag = true;
    }
    qvtkWidget->update();
}

void HexDockWidget::on_CurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if (current != nullptr) {
        std::string currentstr = current->text().toStdString();
        int currentpos = currentstr.find_first_of('_');
        int currentid = QString(currentstr.substr(currentpos + 1).c_str()).toInt();
        if (currentpos == currentstr.npos) ;
        //else m_renderer->AddActor(m_vtkActors.at(currentid));
        else m_renderer->AddActor(m_vtkActors.at(listid_id[currentid]));
    }
    if (previous != nullptr) {
        std::string previousstr = previous->text().toStdString();
        int previouspos = previousstr.find_first_of('_');
        int previousid = QString(previousstr.substr(previouspos + 1).c_str()).toInt();
        if (previouspos == previousstr.npos) ;
        else if (previous->checkState() == Qt::Unchecked) m_renderer->RemoveActor(m_vtkActors.at(listid_id[previousid]));
    }
    m_renderer->SetBackground(1, 1, 1);
    static bool flag = false;
    if (!flag) {
        qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);
        flag = true;
    }
    qvtkWidget->update();
}
void HexDockWidget::loadListWidgetItems(const std::string path, const std::string prefix, QString iconFilename)
{
    isLoadingFinished = false;
    setFolderPath(path);
    setListItemFileNamePrefix(prefix);
    setIconFilename(iconFilename);
    listWidget->clear();
    for (int i = 0; i < numOfListItems; i++)
        m_renderer->RemoveActor(m_vtkActors.at(i));
    m_vtkActors.clear();

    QDir dir(QString::fromStdString(strFolderPath));
    QStringList nameFilters;
    nameFilters << (prefix + "*").c_str();
    dir.setNameFilters(nameFilters);
    numOfListItems = dir.count();

    QFileInfoList list = dir.entryInfoList();
    std::vector<int> listid(list.size());
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        //std::cout << qPrintable(QString("%1").arg(fileInfo.fileName()));
        //std::cout << std::endl;
        std::string name = fileInfo.fileName().toStdString();
        for (char& c : name)
            if (!isdigit(c)) c = ' ';
        int id;
        std::stringstream ss(name.c_str());
        ss >> id;
        listid[i] = id;
    }
    std::sort(listid.begin(), listid.end());
    for (int i = 0; i < listid.size(); ++i)
        listid_id[listid[i]] = i;


    opacitySpinBox->setEnabled(false);
    opacitySlider->setEnabled(false);
    displayComboBox->setEnabled(false);

    cellDataFields.clear();
    for (int i = 0; i < numOfListItems; i++) {
        QListWidgetItem* item = new QListWidgetItem(QIcon(iconFilename), QString(QString::fromStdString(prefix) + "_%1").arg(listid[i]));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
        item->setFlags(item->flags() & (~Qt::ItemIsEnabled));
        listWidget->addItem(item);
//        QString filename = QString::fromStdString(strFolderPath) + QString("/") + QString::fromStdString(prefix) + QString::number(i) + QString(".vtk");
//        AddActor(filename.toStdString().c_str(), i, m_vtkActors, isPolyData);
        QString filename = QString::fromStdString(strFolderPath) + QString("/") + QString::fromStdString(prefix) + QString::number(listid[i]) + QString(".vtk");
        AddActor(filename.toStdString().c_str(), listid[i], m_vtkActors, isPolyData);
        // if (loadingBar && numOfListItems != 0) loadingBar->setValue(i * 100 / numOfListItems);
    }
    for (int i = 0; i < numOfListItems; i++)
        listWidget->item(i)->setFlags(listWidget->item(i)->flags() | Qt::ItemIsEnabled);
    QListWidgetItem* item = new QListWidgetItem(QIcon(iconFilename), QString("all"));
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    //listWidget->addItem(item);
    listWidget->insertItem(0, item);
    on_DisplayCurrentIndexChanged(displayCurrentIndex);

    opacitySpinBox->setEnabled(true);
    opacitySlider->setEnabled(true);
    displayComboBox->setEnabled(true);
    isLoadingFinished = true;
}

void HexDockWidget::AddActor(const char* filename, const int id, std::vector<vtkSmartPointer<vtkActor> >& actors, bool isPolyData) {
    if (isPolyData) {
        AddPolyDataActor(filename, id, actors);
    } else {
        AddUnstructedGridActor(filename, id, actors);
    }
}

void HexDockWidget::AddUnstructedGridActor(const char* filename, const int id, std::vector<vtkSmartPointer<vtkActor> >& actors)
{
    vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    if (isSingularity) {
        auto cellDatas = GetScalarFields(reader, 1);
        cout << "number of cellDatas = " << cellDatas.size() << endl;
        vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
        colorLookupTable->SetTableRange(0, 8);
        colorLookupTable->Build();
        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors");

        double dcolor_valence[8][3] =
        {{0, 0, 1.0},
        {1.0, 1.0, 1.0},
        {0, 1.0, 0},
        {0, 1.0, 1.0},
        {1.0, 1.0, 0},
        {1.0, 0, 0.0},
        {1.0, 0, 1.0},
        {0.0, 0, 0.0}};

        //colorLookupTable->GetColor(0, dcolor);
        unsigned char color_valence[8][3];
        for(unsigned int i = 0; i < 8; i++)
            for(unsigned int j = 0; j < 3; j++)
                color_valence[i][j] = static_cast<unsigned char>(255.0 * dcolor_valence[i][j]);

        for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
            colors->InsertNextTypedTuple(color_valence[cellDatas[0][i] - 1]);

        reader->GetOutput()->GetCellData()->SetScalars(colors);
    } else {
        //////////////////////////////////
        // Create the color map
        vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
        cellDataFields.push_back(GetScalarFields(reader));
        colorLookupTable->SetTableRange(0, numOfListItems);
        colorLookupTable->Build();
        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors");

        double dcolor[3];
        colorLookupTable->GetColor(id, dcolor);
        unsigned char color[3];
        for(unsigned int j = 0; j < 3; j++)
            color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);

        for(int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
            //colors->InsertNextTupleValue(color);
            colors->InsertNextTypedTuple(color);

        reader->GetOutput()->GetCellData()->SetScalars(colors);
    }
    //////////////////////////////////
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->ImmediateModeRenderingOn();
    vtkSmartPointer < vtkActor > actor = vtkSmartPointer < vtkActor > ::New();
    actor->SetMapper(mapper);
    actors.push_back(actor);
}

void HexDockWidget::AddPolyDataActor(const char* filename, const int id, std::vector<vtkSmartPointer<vtkActor> >& actors)
{
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    //////////////////////////////////
    // Create the color map
    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    cellDataFields.push_back(GetScalarFields(reader));
    colorLookupTable->SetTableRange(0, numOfListItems);
    colorLookupTable->Build();
    // Generate the colors for each point based on the color map
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    double dcolor[3];
    colorLookupTable->GetColor(id, dcolor);
    unsigned char color[3];
    for(unsigned int j = 0; j < 3; j++)
        color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);

    for(int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
        //colors->InsertNextTupleValue(color);
        colors->InsertNextTypedTuple(color);

    reader->GetOutput()->GetCellData()->SetScalars(colors);
    //////////////////////////////////
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    //mapper->StaticOn();
    mapper->ImmediateModeRenderingOn();
    vtkSmartPointer < vtkActor > actor = vtkSmartPointer < vtkActor >::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(1);
    actor->GetProperty()->SetAmbient(0.1);
    actors.push_back(actor);
}

std::vector<std::vector<int> > HexDockWidget::GetScalarFields(vtkSmartPointer<vtkUnstructuredGridReader> pReader, int numOfFields)
{
    std::vector<std::vector<int> > fields;
    for (int f = 0; f < numOfFields; f++) {
        vtkCellData* cellData = pReader->GetOutput()->GetCellData();
        pReader->SetScalarsName(pReader->GetScalarsNameInFile(f));
        pReader->Update();
        cellData->Update();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(cellData);
        vtkIntArray* scalarDataInt = vtkIntArray::SafeDownCast(attribute->GetScalars(pReader->GetScalarsNameInFile(f)));
        if (scalarDataInt) {
            int nc = scalarDataInt->GetNumberOfTuples();
            std::vector<int> scalarField(nc);
            for (int i = 0; i < nc; i++)
                scalarField.at(i) = scalarDataInt->GetValue(i);
            fields.push_back(scalarField);
        }
    }
    return fields;
}

std::vector<std::vector<int> > HexDockWidget::GetScalarFields(vtkSmartPointer<vtkPolyDataReader> pReader, int numOfFields)
{
    std::vector<std::vector<int> > fields;
    for (int f = 0; f < numOfFields; f++) {
        vtkCellData* cellData = pReader->GetOutput()->GetCellData();
        pReader->SetScalarsName(pReader->GetScalarsNameInFile(f));
        pReader->Update();
        cellData->Update();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(cellData);
        vtkIntArray* scalarDataInt = vtkIntArray::SafeDownCast(attribute->GetScalars(pReader->GetScalarsNameInFile(f)));
        if (scalarDataInt) {
            int nc = scalarDataInt->GetNumberOfTuples();
            std::vector<int> scalarField(nc);
            for (int i = 0; i < nc; i++)
                scalarField.at(i) = scalarDataInt->GetValue(i);
            fields.push_back(scalarField);
        }
    }
    return fields;
}
