#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QThread>

#include "dataobject.h"
#include <qpainter.h>

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
#include <vtkPointData.h>
#include <vtkColorTransferFunction.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkScalarBarActor.h>

#include <unordered_set>
#include <set>

enum SheetCellDataField{
    COMPONENT_COLOR = 0,
    RAW_COMPONENT_ID,
    COMPONENT_ID,
    CHORD_ID,
    SHEET_ID,
    CHORD_LINK,
    MAIN_SHEET
};

MainWindow* g_mainWindow = nullptr;
Ui::MainWindow* g_ui = nullptr;

const unsigned char char_html_colors[16][3] = {
    {255, 255, 255},    // WHITE,   #FFFFFF RGB(255, 255, 255)
    {192, 192, 192},    // SILVER,  #C0C0C0	RGB(192, 192, 192)
    {128, 128, 128},    // GRAY,    #808080	RGB(128, 128, 128)
    {  0,   0,   0},    // BLACK,   #000000	RGB(0, 0, 0)
    {255,   0,   0},    // RED,     #FF0000	RGB(255, 0, 0)
    {128,   0,   0},    // MAROON,  #800000	RGB(128, 0, 0)
    {255, 255,   0},    // YELLOW,  #FFFF00	RGB(255, 255, 0)
    {128, 128,   0},    // OLIVE,   #808000	RGB(128, 128, 0)
    {  0, 255,   0},    // LIME,    #00FF00	RGB(0, 255, 0)
    {  0, 128,   0},    // GREEN,   #008000	RGB(0, 128, 0)
    {  0, 255, 255},    // AQUA,    #00FFFF	RGB(0, 255, 255)
    {  0, 128, 128},    // TEAL,    #008080	RGB(0, 128, 128)
    {  0,   0, 255},    // BLUE,    #0000FF	RGB(0, 0, 255)
    {  0,   0, 128},    // NAVY,    #000080	RGB(0, 0, 128)
    {255,   0, 255},    // FUCHSIA, #FF00FF	RGB(255, 0, 255))
    {128,   0, 128}     // PURPLE,  #800080	RGB(128, 0, 128))
};

const double double_html_colors[16][3] = {
    {1.00, 1.00, 1.00}, // {255, 255, 255},
    {0.75, 0.75, 0.75}, // {192, 192, 192},
    {0.50, 0.50, 0.50}, // {128, 128, 128},
    {0.00, 0.00, 0.00}, // {0.00, 0.00, 0.00},
    {1.00, 0.00, 0.00}, // {255, 0, 0},
    {0.50, 0.00, 0.00}, // {128, 0, 0},
    {1.00, 1.00, 0.00}, // {255, 255, 0},
    {0.50, 0.50, 0.00}, // {128, 128, 0},
    {0.00, 1.00, 0.00}, // {0, 255, 0},
    {0.00, 0.50, 0.00}, // {0, 128, 0},
    {0.00, 1.00, 1.00}, // {0, 255, 255},
    {0.00, 0.50, 0.50}, // {0, 128, 128},
    {0.00, 0.00, 1.00}, // {0, 0, 255},
    {0.00, 0.00, 0.50}, // {0, 0, 128},
    {1.00, 0.00, 1.00}, // {255, 0, 255},
    {0.50, 0.00, 0.50}, // {128, 0, 128}
};

static void GetColorPixmap(unsigned char color[3], QPixmap& pixmap)
{
    QColor qcolor;
    // set color value
    qcolor.setRed(color[0]);
    qcolor.setGreen(color[1]);
    qcolor.setBlue(color[2]);
    // initialize painter to draw on a pixmap and set composition mode
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setBrush(qcolor);
    painter.setPen(qcolor);
    painter.drawRect(pixmap.rect());
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    strFolderPath("/home/cotrik/svn/testcase/test_base_complex")
{
    ui->setupUi(this);
    m_renderer = ui->m_renderer;
    createActions();
    createMenus();
    createToolBars();
    Update();
    g_mainWindow = this;
    g_ui = ui;
    ui->modelBrowser->loadModels(strFolderPath.c_str());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotExit()
{
    qApp->exit();
}

void MainWindow::createActions()
{
    // QAction: 一建立, 二设置属性, 如图标, 快捷键, 事件处理.
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon("../icon/home.png"));
    //newAction->setShortcut(tr("Ctrl+N"));
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(QString("Create a new file"));
    //QObject::connect(newAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect(newAction, SIGNAL(triggered()), this, SLOT(on_home_clicked()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon("../icon/open.png"));
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    QObject::connect(openAction, SIGNAL(triggered()), this, SLOT(on_fileOpen_clicked()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon("../icon/file_save.png"));
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the document to disk"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_fileSave_clicked()));

    saveAsAction = new QAction(tr("Save as"), this);
    saveAsAction->setIcon(QIcon("../icon/file_save_as.png"));
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save the document under a new name"));

    wireFrameAction = new QAction(tr("Wireframe"), this);
    wireFrameAction->setIcon(QIcon("../icon/wireframe.png"));
    wireFrameAction->setStatusTip(QString("Display wireframe"));
//    wireFrameAction->setCheckable(true);
//    wireFrameAction->setChecked(false);
    wireFrameAction->setDisabled(true);
    QObject::connect(wireFrameAction, SIGNAL(triggered()), this, SLOT(on_wireFrameAction_clicked()));

    surfaceAction = new QAction(tr("Surface"), this);
    surfaceAction->setIcon(QIcon("../icon/surface.png"));
    surfaceAction->setStatusTip(QString("Display surface"));
//    surfaceAction->setCheckable(true);
//    surfaceAction->setChecked(false);
    surfaceAction->setDisabled(true);
    QObject::connect(surfaceAction, SIGNAL(triggered()), this, SLOT(on_surfaceAction_clicked()));

    surfaceWithEdgeAction = new QAction(tr("SurfaceWithEdge"), this);
    surfaceWithEdgeAction->setIcon(QIcon("../icon/surfaceWithEdges.png"));
    surfaceWithEdgeAction->setStatusTip(QString("Display surface with Edge"));
//    surfaceWithEdgeAction->setCheckable(true);
//    surfaceWithEdgeAction->setChecked(false);
    surfaceWithEdgeAction->setDisabled(true);
    QObject::connect(surfaceWithEdgeAction, SIGNAL(triggered()), this, SLOT(on_surfaceWithEdgeAction_clicked()));

    volumeAction = new QAction(tr("Volume"), this);
    volumeAction->setIcon(QIcon("../icon/volume.png"));
    volumeAction->setCheckable(true);
    volumeAction->setChecked(true);
    volumeAction->setDisabled(true);
    QObject::connect(volumeAction, SIGNAL(triggered()), this, SLOT(on_volumeAction_clicked()));

    sheetListAction = new QAction(tr("Sheet List"), this);
    sheetListAction->setIcon(QIcon("../icon/sheet.png"));
    sheetListAction->setCheckable(true);
    sheetListAction->setChecked(false);
    QObject::connect(sheetListAction, SIGNAL(triggered()), this, SLOT(on_sheetListAction_clicked()));

    componentListAction = new QAction(tr("Component List"), this);
    componentListAction->setIcon(QIcon("../icon/component.png"));
    componentListAction->setCheckable(true);
    componentListAction->setChecked(false);
    QObject::connect(componentListAction, SIGNAL(triggered()), this, SLOT(on_componentListAction_clicked()));

    baseComplexAction = new QAction(tr("Base-Complex"), this);
    baseComplexAction->setIcon(QIcon("../icon/base_complex.png"));
    baseComplexAction->setCheckable(true);
    baseComplexAction->setChecked(false);
    baseComplexAction->setDisabled(true);
    QObject::connect(baseComplexAction, SIGNAL(triggered()), this, SLOT(on_baseComplexAction_clicked()));

    singularityAction = new QAction(tr("Singularity"), this);
    singularityAction->setIcon(QIcon("../icon/singularity.png"));
    singularityAction->setCheckable(true);
    singularityAction->setChecked(false);
    singularityAction->setDisabled(true);
    QObject::connect(singularityAction, SIGNAL(triggered()), this, SLOT(on_singularityAction_clicked()));

//    stopLoadingAction = new QAction(tr("&Stop"), this);
//    stopLoadingAction->setIcon(QIcon("../icon/stop.png"));
//    stopLoadingAction->setShortcuts(QKeySequence::Copy);
//    stopLoadingAction->setStatusTip(tr("Stop loading files"));
//    QObject::connect(stopLoadingAction, SIGNAL(triggered()), this, SLOT(on_stopLoading_clicked()));
//    ui->sheetCellDataFieldComboBox->setDisabled(true);


//    //-------------------------------------------
//    // Set the number of columns in the tree
//    ui->sheetTreeWidget->setColumnCount(1);
//    // Set the headers
//    ui->sheetTreeWidget->setHeaderLabels(QStringList() << "Sheet");
//    QObject::connect(ui->sheetTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeSheetItemClicked(QTreeWidgetItem *, int)));

    QObject::connect(ui->modelBrowser->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(on_ModelClicked(QListWidgetItem *)));
    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timout()), this, SLOT(TimerHandlerFunction()));
//    m_timer.setSingleShot(false); // if you only want it to fire once
//    //m_timer.setInterval(1000);
    m_timer->start(1000);
}

void MainWindow::createMenus()
{
    // menuBar()第一次被调用时QMainWindow会生一个一QMenuBar在窗体上,
    // 且返回一个QMenu的指针.
    // menuBar()->addMenu()会生成一个QMenu且返回他的指针.
    // QMenu加入一个QAction, 就可以对事件进行反应了
    // 一个QAction可以被多个地方使用, 与Java的Action一样.
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(wireFrameAction);
    editMenu->addAction(surfaceAction);
    editMenu->addAction(surfaceWithEdgeAction);
    editMenu->addAction(volumeAction);

    listMenu = menuBar()->addMenu(tr("&View"));
    listMenu->addAction(sheetListAction);
    listMenu->addAction(componentListAction);
    listMenu->addAction(baseComplexAction);
    listMenu->addAction(singularityAction);
}

void MainWindow::createToolBars()
{
    // 跟创建菜单一个相似
    toolBar = addToolBar(tr("&File"));
    toolBar->addAction(newAction);
    toolBar->addAction(openAction);
    toolBar->addSeparator();
    toolBar->addAction(saveAction);
    toolBar->addAction(saveAsAction);

    toolBar = addToolBar(tr("&Edit"));
    toolBar->addAction(wireFrameAction);
    toolBar->addAction(surfaceAction);
    toolBar->addAction(surfaceWithEdgeAction);
    toolBar->addAction(volumeAction);
    //toolBar->addAction(stopLoadingAction);
    //////////////////////////////////////
    opacitySlider = new QSlider(Qt::Horizontal);
    opacitySlider->setMinimum(0);
    opacitySlider->setMaximum(100);
    opacitySlider->setValue(100);
    opacitySpinBox = new QDoubleSpinBox();
    opacitySpinBox->setMaximum(1);
    opacitySpinBox->setValue(1);
    QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_opacityChanged(int)));
    QObject::connect(opacitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_opacityChanged(double)));
    opacitySlider->setDisabled(true);
    opacitySpinBox->setDisabled(true);

    opacityBaseComplexSlider = new QSlider(Qt::Horizontal);
    opacityBaseComplexSlider->setMinimum(0);
    opacityBaseComplexSlider->setMaximum(100);
    opacityBaseComplexSlider->setValue(100);
    opacityBaseComplexSpinBox = new QDoubleSpinBox();
    opacityBaseComplexSpinBox->setMaximum(1);
    opacityBaseComplexSpinBox->setValue(1);
    QObject::connect(opacityBaseComplexSlider, SIGNAL(valueChanged(int)), this, SLOT(on_opacityBaseComplexChanged(int)));
    QObject::connect(opacityBaseComplexSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_opacityBaseComplexChanged(double)));
    opacityBaseComplexSlider->setDisabled(true);
    opacityBaseComplexSpinBox->setDisabled(true);
//    QRect rb = opacitySingularitySlider->geometry();
//    opacitySingularitySlider->setGeometry(rb.x(), rb.y(), 100, rb.height());

    opacitySingularitySlider = new QSlider(Qt::Horizontal);
    opacitySingularitySlider->setMinimum(0);
    opacitySingularitySlider->setMaximum(100);
    opacitySingularitySlider->setValue(100);
    opacitySingularitySpinBox = new QDoubleSpinBox();
    opacitySingularitySpinBox->setMaximum(1);
    opacitySingularitySpinBox->setValue(1);
    QObject::connect(opacitySingularitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_opacitySingularityChanged(int)));
    QObject::connect(opacitySingularitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_opacitySingularityChanged(double)));
    opacitySingularitySlider->setDisabled(true);
    opacitySingularitySpinBox->setDisabled(true);
//    QRect r = opacitySingularitySlider->geometry();
//    opacitySingularitySlider->setGeometry(r.x(), r.y(), 100, r.height());
    //////////////////////////////////////
    QAction* opacity = new QAction(QString("Opacity:"), nullptr);
    toolBar->addAction(opacity);
    toolBar->addWidget(opacitySlider);
    toolBar->addWidget(opacitySpinBox);

    toolBar = addToolBar(tr("&View"));
    toolBar->addAction(sheetListAction);
    toolBar->addAction(componentListAction);
    toolBar->addAction(baseComplexAction);
    QAction* opacityBaseComplex = new QAction(QString("Opacity:"), nullptr);
    toolBar->addAction(opacityBaseComplex);
    toolBar->addWidget(opacityBaseComplexSlider);
    toolBar->addWidget(opacityBaseComplexSpinBox);
    toolBar->addAction(singularityAction);
    QAction* opacitySingularity = new QAction(QString("Opacity:"), nullptr);
    toolBar->addAction(opacitySingularity);
    toolBar->addWidget(opacitySingularitySlider);
    toolBar->addWidget(opacitySingularitySpinBox);

    singularityDisplayTypeComboBox = new QComboBox;
    singularityDisplayTypeComboBox->setObjectName(QString::fromUtf8("sheetCellDataFieldComboBox"));
    singularityDisplayTypeComboBox->setGeometry(QRect(0, 0, 200, 27));
    singularityDisplayTypeComboBox->addItem(QIcon("../icon/sheets.png"), QString("Valence"));
    singularityDisplayTypeComboBox->addItem(QIcon("../icon/sheet.png"), QString("Index"));
    singularityDisplayTypeComboBox->setCurrentIndex(1);
    QObject::connect(singularityDisplayTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_singularityDisplyTypeCurrentIndexChanged(int)));
    toolBar->addWidget(singularityDisplayTypeComboBox);

    sheetCellDataFieldComboBox = new QComboBox;
    sheetCellDataFieldComboBox->setObjectName(QString::fromUtf8("sheetCellDataFieldComboBox"));
    sheetCellDataFieldComboBox->setGeometry(QRect(0, 0, 200, 27));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/num.png"), QString("Component"));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/volume.png"), QString("Raw Component id"));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/id.png"), QString("Component id"));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/chord.png"), QString("Chord id"));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/sheet.png"), QString("Sheet id"));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/link.png"), QString("Chord link"));
    sheetCellDataFieldComboBox->addItem(QIcon("../icon/sheet.png"), QString("Main sheet"));
    sheetCellDataFieldComboBox->setCurrentIndex(COMPONENT_COLOR);
    QObject::connect(sheetCellDataFieldComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_sheetCellDataFieldCurrentIndexChanged(int)));
    toolBar->addWidget(sheetCellDataFieldComboBox);

    openFolderAction = new QAction(tr("&OpenFolder"), this);
    openFolderAction->setIcon(QIcon("../icon/models.png"));
    //openFolderAction->setShortcuts(QKeySequence::Open);
    openFolderAction->setStatusTip(tr("Open an existing folder"));
    QObject::connect(openFolderAction, SIGNAL(triggered()), this, SLOT(on_openFolder_clicked()));
    toolBar->addAction(openFolderAction);

    if (ui->componentDockWidget != nullptr) {
        sheetsSetAction = new QAction(tr("&SheetsSet"), this);
        sheetsSetAction->setIcon(QIcon("../icon/check.png"));
        sheetsSetAction->setStatusTip(tr("Extract Minimal Sheets Set"));
        QObject::connect(sheetsSetAction, SIGNAL(triggered()), this, SLOT(on_sheetsSet_clicked()));
        toolBar->addAction(sheetsSetAction);
    }
    if (ui->componentDockWidget != nullptr) {
        ui->componentDockWidget->loadingBar = new QProgressBar;
        ui->componentDockWidget->loadingBar->setOrientation(Qt::Horizontal);
        ui->componentDockWidget->loadingBar->setRange(0, 100);
        ui->componentDockWidget->loadingBar->setValue(0);
        ui->componentDockWidget->loadingBar->setGeometry(0, 0, 200, 27);
        toolBar->addWidget(ui->componentDockWidget->loadingBar);
    }

    sheetDisplayTypeComboBox = new QComboBox;
    sheetDisplayTypeComboBox->setObjectName(QString::fromUtf8("sheetCellDataFieldComboBox"));
    sheetDisplayTypeComboBox->setGeometry(QRect(0, 0, 200, 27));
    sheetDisplayTypeComboBox->addItem(QIcon("../icon/sheets.png"), QString("Hex"));
    sheetDisplayTypeComboBox->addItem(QIcon("../icon/sheet.png"), QString("Quad"));
    sheetDisplayTypeComboBox->addItem(QIcon("../icon/link.png"), QString("Chord"));
    sheetDisplayTypeComboBox->setCurrentIndex(0);
    QObject::connect(sheetDisplayTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_sheetDisplyTypeCurrentIndexChanged(int)));
    toolBar->addWidget(sheetDisplayTypeComboBox);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::on_home_clicked() {
    Update();
    ui->qvtkWidget->update();
}

void MainWindow::on_openFolder_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this,  tr("Open Directory"), strFolderPath.c_str(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    strFolderPath = filename.toStdString();
    ui->modelBrowser->loadModels(strFolderPath.c_str());
}

void FindSetCombination(std::vector<std::set<size_t>>& input, std::set<size_t>& target, std::vector<std::set<size_t>>& output) {
    std::set<int> full;
    for (auto it : input)
        full.insert(it.begin(), it.end());

    if (!includes(full.begin(), full.end(), target.begin(), target.end())) return;

    for (size_t i = input.size() - 1; i > 0; --i) {
        std::vector<bool> vec(input.size(), false);
        std::fill(vec.begin() + i, vec.end(), true);
        std::set<int> comb;

        do {
            for (size_t j = 0; j < vec.size(); ++j)
                if (vec[j]) comb.insert(input[j].begin(), input[j].end());
            if (includes(comb.begin(), comb.end(), target.begin(), target.end())) {
                for (size_t j = 0; j < vec.size(); ++j)
                    if (vec[j]) output.push_back(input[j]);
                return;
            }
            comb.clear();

        } while (next_permutation(vec.begin(), vec.end()));
    }
}

void MainWindow::on_sheetsSet_clicked()
{
    if (ui->componentDockWidget == NULL) return;
    if (strFilename.isEmpty()) return;
    std::vector<std::set<size_t>> input;
    std::set<size_t> target;
    std::vector<std::set<size_t>> output;
    for (auto& i : ui->sheetDockWidget->cellDataFields)
        input.push_back(std::set<size_t>(i[1].begin(), i[1].end()));
    for (auto i = 0; i < ui->componentDockWidget->numOfListItems; target.insert(i++));
    FindSetCombination(input, target, output);
    std::cout << "*** sheets set size " << output.size() << " ***" << std::endl;
    std::set<size_t> ids;
    for (auto i : output)
        for (auto j = 0; j < input.size(); ++j)
            if (i == input[j]) {
                ids.insert(j);
                break;
            }
    std::cout << "***************************" << std::endl;
    for (auto i : ids)
        std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "***************************" << std::endl;

    for (int i = 0; i < ui->sheetDockWidget->numOfListItems; ++i)
        ui->sheetDockWidget->listWidget->item(i)->setCheckState(Qt::Unchecked);
    for (auto id : ids) {
        ui->sheetDockWidget->listWidget->item(id + 1)->setCheckState(Qt::Checked);
        ui->sheetDockWidget->m_renderer->AddActor(ui->sheetDockWidget->m_vtkActors.at(ui->sheetDockWidget->listid_id[id]));
    }
    ui->qvtkWidget->update();
}

void MainWindow::on_fileOpen_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,  tr("Open File"), strFolderPath.c_str(),
        "All files (*.*);;Text File (*.txt);;Vtk File (*.vtk);;Mesh File (*.mesh);;Off File (*.off);;Stl File (*.stl);;Obj File (*.obj)");
    //QMessageBox::information(this, tr("File Name"), strFilename);
    if (filename == strFilename)
        return;
    else {
        if (filename.toStdString().find(".vtk") == std::string::npos) return;
        strFilename = filename;
    }
    std::string str(strFilename.toStdString());
    size_t pos = str.find_last_of('/');
    std::string strFolderLocation = str.substr(0, pos);
    strFolderPath = strFolderLocation.c_str();
    Update();
    surfaceAction->setEnabled(true);
    surfaceWithEdgeAction->setEnabled(true);
    wireFrameAction->setEnabled(true);
    volumeAction->setEnabled(true);

    opacitySlider->setEnabled(true);
    opacitySpinBox->setEnabled(true);
    sheetListAction->setEnabled(true);
    sheetListAction->setChecked(false);
    componentListAction->setEnabled(true);
    componentListAction->setChecked(false);
    opacityBaseComplexSlider->setDisabled(true);
    opacityBaseComplexSpinBox->setDisabled(true);
    opacitySingularitySlider->setDisabled(true);
    opacitySingularitySpinBox->setDisabled(true);
}

bool MainWindow::on_fileSave_clicked()
{
    if (strFilename.isEmpty()) {
        return on_fileSaveAs_clicked();
    } else {
        return saveFile(strFilename);
    }
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    strFilename = fileName;
    //textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = strFilename;
    if (strFilename.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

bool MainWindow::on_fileSaveAs_clicked()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}


void MainWindow::Update()
{
    m_renderer->RemoveActor(m_vtkActor.GetPointer());
    ui->qvtkWidget->GetRenderWindow()->RemoveRenderer(m_renderer);
    m_renderer->GetActors()->RemoveAllItems();
    //vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
    m_vtkUnstructuredGridReader->SetFileName(strFilename.toStdString().c_str());
    m_vtkUnstructuredGridReader->Update();

   //vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    m_vtkDataSetMapper->SetInputConnection(m_vtkUnstructuredGridReader->GetOutputPort());
    //vtkSmartPointer < vtkActor > actor = vtkSmartPointer < vtkActor > ::New();
    m_vtkActor->SetMapper(m_vtkDataSetMapper.GetPointer());
    m_vtkActor->GetProperty()->SetAmbient(0.1);
    m_vtkActor->GetProperty()->SetSpecular(0.01);

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(1);
    lut->SetTableRange(0.0, 1);
    lut->SetTableValue(0.0, 1.0, 1.0, 1.0);
    lut->Build();
    m_vtkDataSetMapper.GetPointer()->SetLookupTable(lut);
    m_vtkDataSetMapper.GetPointer()->SetScalarRange(0.0, 1);

    // VTK Renderer
    //vtkSmartPointer < vtkRenderer > leftRenderer = vtkSmartPointer < vtkRenderer > ::New();
    m_renderer->AddActor(m_vtkActor.GetPointer());
    m_renderer->SetBackground(1, 1, 1);
    m_renderer->ResetCamera();
    // VTK/Qt wedded
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

    m_mouseInteractorStyle->SetDefaultRenderer(m_renderer);
    m_mouseInteractorStyle->SetData(m_vtkUnstructuredGridReader->GetOutput());
    ui->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_mouseInteractorStyle.GetPointer());

//    m_renderWindowInteractor->SetRenderWindow(ui->qvtkWidget->GetRenderWindow());
//    m_renderWindowInteractor->Initialize();
//    m_mouseInteractorStyle->SetDefaultRenderer(m_renderer);
//    m_mouseInteractorStyle->SetData(m_vtkUnstructuredGridReader->GetOutput());
//    m_renderWindowInteractor->SetInteractorStyle(m_mouseInteractorStyle.GetPointer());

//    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
//        renderWindowInteractor->SetRenderWindow(ui->qvtkWidget->GetRenderWindow());
//        renderWindowInteractor->Initialize();
//        renderWindowInteractor->SetInteractorStyle(m_mouseInteractorStyle);

//        ui->qvtkWidget->GetRenderWindow()->Render();
//            renderWindowInteractor->Start();
}

void MainWindow::on_wireFrameAction_clicked()
{
    m_vtkActor->GetProperty()->SetRepresentationToWireframe();
//    m_renderer->SetBackground(1, 1, 1);
    ui->qvtkWidget->update();
//    m_leftRenderer->AddActor(m_vtkActor.GetPointer());
//    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_leftRenderer.GetPointer());

   // m_vtkUnstructuredGridReader.GetPointer()->Update();
}

void MainWindow::on_surfaceAction_clicked()
{
    m_vtkActor->GetProperty()->EdgeVisibilityOff();
    m_vtkActor->GetProperty()->SetRepresentationToSurface();
    //m_renderer->SetBackground(1, 1, 1);
    ui->qvtkWidget->update();
}
void MainWindow::on_surfaceWithEdgeAction_clicked()
{
    m_vtkActor->GetProperty()->EdgeVisibilityOn();
    m_vtkActor->GetProperty()->SetRepresentationToSurface();
    m_vtkActor->GetProperty()->SetEdgeColor(0, 0, 0);
//    m_renderer->SetBackground(1, 1, 1);
//    m_rightRenderer.GetPointer()->SetBackground(1, 1, 1);

//    vtkSmartPointer < vtkActor > actor = vtkSmartPointer < vtkActor > ::New();

//    actor->SetMapper(m_vtkDataSetMapper.GetPointer());
//    m_leftRenderer->AddActor(actor);
//     ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_leftRenderer.GetPointer());

//    m_mouseInteractorStyle->SetDefaultRenderer(m_leftRenderer.GetPointer());
//    m_mouseInteractorStyle->vtkUnstructuredGridData = m_vtkUnstructuredGridReader->GetOutput();
//    //renderWindowInteractor->SetInteractorStyle(style);
//    ui->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_mouseInteractorStyle.GetPointer());
    ui->qvtkWidget->update();
}
void MainWindow::on_volumeAction_clicked()
{
    if (!volumeAction->isChecked())
        m_renderer->RemoveActor(m_vtkActor.GetPointer());
    else {
        m_renderer->AddActor(m_vtkActor.GetPointer());
    }
    ui->qvtkWidget->update();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    const QSize &newsize = event->size();
//    ui->centralwidget->setFixedSize(newsize.width(), newsize.height() - 50);
    ui->qvtkWidget->GetRenderWindow()->SetSize(/*1460*/newsize.width(), newsize.height() - 50);
//    const QSize &sheetDockWidgetSize = ui->sheetDockWidget->widget()->size();
//    ui->componentDisplayComboBox->setSizeIncrement(sheetDockWidgetSize);
}

std::vector<std::vector<int> > MainWindow::GetScalarFields(vtkSmartPointer<vtkUnstructuredGridReader> pReader, int numOfFields)
{
    std::vector<std::vector<int> > fields;
    for (int f = 0; f < numOfFields; f++)
    {
        vtkCellData* cellData = pReader->GetOutput()->GetCellData();
        pReader->SetScalarsName(pReader->GetScalarsNameInFile(f));
        pReader->Update();
        cellData->Update();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(cellData);
        vtkIntArray* scalarDataInt = vtkIntArray::SafeDownCast(attribute->GetScalars(pReader->GetScalarsNameInFile(f)));
        if (scalarDataInt)
        {
            int nc = scalarDataInt->GetNumberOfTuples();
            //std::cout << "There are " << nc << " components in " << pReader->GetScalarsNameInFile(0) << std::endl;
            std::vector<int> scalarField(nc);
            for (int i = 0; i < nc; i++){
                scalarField.at(i) = scalarDataInt->GetValue(i);
                //std::cout << scalarField.at(i) << " ";
            }
            //std::cout << std::endl;
            fields.push_back(scalarField);
        }
    }
    return fields;
    //std::cout << std::endl;
    //sheetCellDataFields.push_back(fields);
}

std::vector<std::vector<int> > MainWindow::GetScalarFields(vtkSmartPointer<vtkPolyDataReader> pReader, int numOfFields)
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

std::vector<std::vector<double> > MainWindow::GetDoubleScalarFields(vtkSmartPointer<vtkPolyDataReader> pReader, int numOfFields)
{
    std::vector<std::vector<double> > fields;
    for (int f = 0; f < numOfFields; f++) {
        vtkCellData* cellData = pReader->GetOutput()->GetCellData();
        pReader->SetScalarsName(pReader->GetScalarsNameInFile(f));
        pReader->Update();
        cellData->Update();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(cellData);
        vtkDoubleArray* scalarDataInt = vtkDoubleArray::SafeDownCast(attribute->GetScalars(pReader->GetScalarsNameInFile(f)));
        if (scalarDataInt) {
            int nc = scalarDataInt->GetNumberOfTuples();
            std::vector<double> scalarField(nc);
            for (int i = 0; i < nc; i++)
                scalarField.at(i) = scalarDataInt->GetValue(i);
            fields.push_back(scalarField);
        }
    }
    return fields;
}

std::vector<double> MainWindow::GetDoubleScalarField(vtkSmartPointer<vtkPolyDataReader> pReader, std::string fieldname, int numOfFields)
{
    std::vector<double> field;
    for (int f = 0; f < numOfFields; f++) {
        vtkCellData* cellData = pReader->GetOutput()->GetCellData();
        pReader->SetScalarsName(pReader->GetScalarsNameInFile(f));
        pReader->Update();
        cellData->Update();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(cellData);
        vtkDoubleArray* scalarDataInt = vtkDoubleArray::SafeDownCast(attribute->GetScalars(pReader->GetScalarsNameInFile(f)));
        if (scalarDataInt) {
            int nc = scalarDataInt->GetNumberOfTuples();
            std::vector<double> scalarField(nc);
            for (int i = 0; i < nc; i++)
                scalarField.at(i) = scalarDataInt->GetValue(i);
            field = scalarField;
        }
        if (std::string(pReader->GetScalarsNameInFile(f)) == fieldname) break;
    }
    return field;
}

VTKData MainWindow::GetAllData(vtkPolyData* polydata) {
    std::cout << "Normals: " << polydata->GetPointData()->GetNormals() << std::endl;

    vtkIdType numberOfPointArrays = polydata->GetPointData()->GetNumberOfArrays();
    std::cout << "Number of PointData arrays: " << numberOfPointArrays << std::endl;

    vtkIdType numberOfCellArrays = polydata->GetCellData()->GetNumberOfArrays();
    std::cout << "Number of CellData arrays: " << numberOfCellArrays << std::endl;

    std::cout << "Type table/key: " << std::endl;;
    //more values can be found in <VTK_DIR>/Common/vtkSetGet.h
    std::cout << VTK_UNSIGNED_CHAR << " unsigned char" << std::endl;
    std::cout << VTK_UNSIGNED_INT << " unsigned int" << std::endl;
    std::cout << VTK_FLOAT << " float" << std::endl;
    std::cout << VTK_DOUBLE << " double" << std::endl;

    VTKData res;
    for(vtkIdType i = 0; i < numberOfPointArrays; i++) {
        // The following two lines are equivalent
        //arrayNames.push_back(polydata->GetPointData()->GetArray(i)->GetName());
        //arrayNames.push_back(polydata->GetPointData()->GetArrayName(i));
        int dataTypeID = polydata->GetPointData()->GetArray(i)->GetDataType();
        std::cout << "Array " << i << ": " << polydata->GetPointData()->GetArrayName(i)
                  << " (type: " << dataTypeID << ")" << std::endl;
        auto name = polydata->GetPointData()->GetArrayName(i);
        vtkPointData* data = polydata->GetPointData();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(data);

        if (dataTypeID == VTK_INT) {
            vtkIntArray* scalarData = vtkIntArray::SafeDownCast(attribute->GetScalars(name));
            if (scalarData) {
                Data field(name);
                int nc = scalarData->GetNumberOfTuples();
                field.intData.resize(nc);
                for (int i = 0; i < nc; i++)
                    field.intData.at(i) = scalarData->GetValue(i);
                res.pointsData.push_back(field);
            }
        } else if (dataTypeID == VTK_FLOAT) {
            vtkFloatArray* scalarData = vtkFloatArray::SafeDownCast(attribute->GetScalars(name));
            if (scalarData) {
                Data field(name);
                int nc = scalarData->GetNumberOfTuples();
                field.floatData.resize(nc);
                for (int i = 0; i < nc; i++)
                    field.intData.at(i) = scalarData->GetValue(i);
                res.pointsData.push_back(field);
            }
        } else if (dataTypeID == VTK_DOUBLE) {
            vtkDoubleArray* scalarData = vtkDoubleArray::SafeDownCast(attribute->GetScalars(name));
            if (scalarData) {
                Data field(name);
                int nc = scalarData->GetNumberOfTuples();
                field.floatData.resize(nc);
                for (int i = 0; i < nc; i++)
                    field.doubleData.at(i) = scalarData->GetValue(i);
                res.pointsData.push_back(field);
            }
        }
    }

    for(vtkIdType i = 0; i < numberOfCellArrays; i++)
    {
        int dataTypeID = polydata->GetCellData()->GetArray(i)->GetDataType();
        std::cout << "Array " << i << ": " << polydata->GetCellData()->GetArrayName(i)
                  << " (type: " << dataTypeID << ")" << std::endl;
        auto name = polydata->GetCellData()->GetArrayName(i);
        vtkCellData* data = polydata->GetCellData();
        vtkDataSetAttributes* attribute = vtkDataSetAttributes::SafeDownCast(data);

        if (dataTypeID == VTK_INT) {
            vtkIntArray* scalarData = vtkIntArray::SafeDownCast(attribute->GetScalars(name));
            if (scalarData) {
                Data field(name);
                int nc = scalarData->GetNumberOfTuples();
                field.intData.resize(nc);
                for (int i = 0; i < nc; i++)
                    field.intData.at(i) = scalarData->GetValue(i);
                res.cellsData.push_back(field);
            }
        } else if (dataTypeID == VTK_FLOAT) {
            vtkFloatArray* scalarData = vtkFloatArray::SafeDownCast(attribute->GetScalars(name));
            if (scalarData) {
                Data field(name);
                int nc = scalarData->GetNumberOfTuples();
                field.floatData.resize(nc);
                for (int i = 0; i < nc; i++)
                    field.intData.at(i) = scalarData->GetValue(i);
                res.cellsData.push_back(field);
            }
        } else if (dataTypeID == VTK_DOUBLE) {
            vtkDoubleArray* scalarData = vtkDoubleArray::SafeDownCast(attribute->GetScalars(name));
            if (scalarData) {
                Data field(name);
                int nc = scalarData->GetNumberOfTuples();
                field.floatData.resize(nc);
                for (int i = 0; i < nc; i++)
                    field.doubleData.at(i) = scalarData->GetValue(i);
                res.cellsData.push_back(field);
            }
        }
    }
}

void MainWindow::AddBaseComplexActor(const char* filename)
{
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename);
    reader->Update();

    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    colorLookupTable->SetTableRange(0, 1);
    colorLookupTable->Build();
    // Generate the colors for each point based on the color map
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    double dcolor[3] = {0.0, 0, 0.0};
    //colorLookupTable->GetColor(0, dcolor);
    unsigned char color[3];
    for(unsigned int j = 0; j < 3; j++)
        color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);

    for(int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
        //colors->InsertNextTupleValue(color);
        colors->InsertNextTypedTuple(color);

    reader->GetOutput()->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->ImmediateModeRenderingOn();
    m_vtkBaseComplexActor.GetPointer()->SetMapper(mapper);
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->EdgeVisibilityOn();
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetEdgeColor(0, 0, 0);
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetLineWidth(3);
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetPointSize(5);
    //m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetColor(1, 0, 0);
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetRepresentationToSurface();
}

std::vector<std::vector<int> > GetScalarFields_(vtkSmartPointer<vtkPolyDataReader> pReader)
{
    std::vector<std::vector<int> > fields;
    int numOfFields = pReader->GetOutput()->GetCellData()->GetNumberOfArrays();
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

void MainWindow::AddSingularityActor(const char* filename)
{
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    auto cellDatas = GetScalarFields(reader);
    cout << "number of cellDatas = " << cellDatas.size() << endl;
    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    colorLookupTable->SetTableRange(0, 1);
    colorLookupTable->Build();
    // Generate the colors for each point based on the color map
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    double dcolor[3] = {1.0, 0, 1.0};
    //colorLookupTable->GetColor(0, dcolor);
    unsigned char color[3];
    for(unsigned int j = 0; j < 3; j++)
        color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);

    for(int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
        //colors->InsertNextTupleValue(color);
        colors->InsertNextTypedTuple(color);

    reader->GetOutput()->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->ImmediateModeRenderingOn();
    m_vtkSingularityActor.GetPointer()->SetMapper(mapper);
    m_vtkSingularityActor.GetPointer()->GetProperty()->EdgeVisibilityOn();
    m_vtkSingularityActor.GetPointer()->GetProperty()->SetEdgeColor(1, 1, 0);
    m_vtkSingularityActor.GetPointer()->GetProperty()->SetLineWidth(6);
    m_vtkSingularityActor.GetPointer()->GetProperty()->SetPointSize(12);
    //m_vtkSingularityActor.GetPointer()->GetProperty()->SetColor(1, 0, 0);
    m_vtkSingularityActor.GetPointer()->GetProperty()->SetRepresentationToSurface();
    m_vtkSingularityActor->GetProperty()->SetRenderLinesAsTubes(1);
    m_vtkSingularityActor->GetProperty()->SetRenderPointsAsSpheres(1);
    //m_vtkSingularityActor->GetProperty()->SetVertexVisibility(1);
    m_vtkSingularityActor->GetProperty()->SetVertexColor(1, 0, 0);
}

//void MainWindow::AddSingularityVActor(const char* filename) {
//    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
//    reader->SetFileName(filename);
//    reader->Update();
//    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
//    colorLookupTable->SetTableRange(0, 1);
//    colorLookupTable->Build();
//    // Generate the colors for each point based on the color map
//    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
//    colors->SetNumberOfComponents(3);
//    colors->SetName("Colors");
//
//    double dcolor[3] = { 1.0, 0, 0.0 };
//    //colorLookupTable->GetColor(0, dcolor);
//    unsigned char color[3];
//    for (unsigned int j = 0; j < 3; j++)
//        color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
//
//    for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
//        //colors->InsertNextTupleValue(color);
//        colors->InsertNextTypedTuple(color);
//
//    reader->GetOutput()->GetCellData()->SetScalars(colors);
//
//    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//    mapper->SetInputConnection(reader->GetOutputPort());
//    mapper->ImmediateModeRenderingOn();
//    m_vtkSingularityVActor.GetPointer()->SetMapper(mapper);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->EdgeVisibilityOn();
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetEdgeColor(1, 1, 0);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetLineWidth(6);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetPointSize(12);
//    //m_vtkSingularityActor.GetPointer()->GetProperty()->SetColor(1, 0, 0);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetRepresentationToSurface();
//    m_vtkSingularityVActor->GetProperty()->SetRenderLinesAsTubes(1);
//    m_vtkSingularityVActor->GetProperty()->SetRenderPointsAsSpheres(1);
//    //m_vtkSingularityActor->GetProperty()->SetVertexVisibility(1);
//    m_vtkSingularityVActor->GetProperty()->SetVertexColor(1, 0, 0);
//}

//void MainWindow::AddSingularityVActor(const char* filename) {
//    const auto id = singularityDisplayTypeComboBox->currentIndex();
//    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
//    reader->SetFileName(filename);
//    reader->Update();
//    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
////    colorLookupTable->SetTableRange(0, 1);
////    colorLookupTable->Build();
//    // Generate the colors for each point based on the color map
//    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
//    colors->SetNumberOfComponents(3);
//    colors->SetName("Colors");
//    double dcolor[3];
//    unsigned char color[3];
//    std::set<int> valences;
//    std::set<double> indices;
//    if (id == 0) {
//        auto cellDataValence = GetScalarFields(reader, 2);
//        for (auto v : cellDataValence[0])
//            if (valences.find(v) == valences.end()) valences.insert(v);
//        // valences.insert(cellDataValence[0].begin(), cellDataValence[0].end());
//        auto minv = 1000;
//        auto maxv = -1000;
//        for (auto v : cellDataValence[0]) {
//            minv = std::min(v, minv);
//            maxv = std::max(v, maxv);
//        }
//        colorLookupTable->SetTableRange(minv, maxv);
//        colorLookupTable->Build();
//        for (int i = 0; i < cellDataValence[0].size(); i++) {
//            colorLookupTable->GetColor(cellDataValence[0][i], dcolor);
//            for(unsigned int j = 0; j < 3; j++)
//                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
//            colors->InsertNextTypedTuple(color);
//        }
//    }
//    else {
//        auto cellDatas = GetDoubleScalarField(reader, "index", 2);
//        indices.insert(cellDatas.begin(), cellDatas.end());
//        auto minv = 1000.0;
//        auto maxv = -1000.0;
//        for (auto v : cellDatas) {
//            minv = std::min(v, minv);
//            maxv = std::max(v, maxv);
//        }
//        colorLookupTable->SetTableRange(double(minv), double(maxv));
//        colorLookupTable->Build();
//        for (int i = 0; i < cellDatas.size(); i++) {
//            colorLookupTable->GetColor(cellDatas[i], dcolor);
//            for(unsigned int j = 0; j < 3; j++)
//                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
//            colors->InsertNextTypedTuple(color);
//        }
//    }
//    reader->GetOutput()->GetCellData()->SetScalars(colors);
//    //m_vtkSingularityVActor.GetPointer()->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);

////    double dcolor_valence[8][3] =
////    {{0, 0, 1.0},
////    {1.0, 1.0, 1.0},
////    {0, 1.0, 0},
////    {0, 1.0, 1.0},
////    {1.0, 1.0, 0},
////    {1.0, 0, 0.0},
////    {1.0, 0, 1.0},
////    {0.0, 0, 0.0}};
////
////    //colorLookupTable->GetColor(0, dcolor);
////    unsigned char color_valence[8][3];
////    for(unsigned int i = 0; i < 8; i++)
////        for(unsigned int j = 0; j < 3; j++)
////            color_valence[i][j] = static_cast<unsigned char>(255.0 * dcolor_valence[i][j]);
////
////    const auto id = singularityDisplayTypeComboBox->currentIndex();
////    if (id == 0) {
////        auto cellDatas = GetScalarFields(reader, 2);
////        cout << "------------ number of cellDatas = " << cellDatas.size() << endl;
////        for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
////            colors->InsertNextTypedTuple(color_valence[int(cellDatas[0][i]) - 1]);
////    }
////    else {
////        auto cellDatas = GetDoubleScalarField(reader, "index", 2);
////        cout << "------------ number of cellDatas = " << cellDatas.size() << endl;
////        for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
////            colors->InsertNextTypedTuple(color_valence[int(cellDatas[i] * 8) + 1]);
////    }

////    reader->GetOutput()->GetCellData()->SetScalars(colors);

//    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//    mapper->SetInputConnection(reader->GetOutputPort());
//    mapper->ImmediateModeRenderingOn();
//    m_vtkSingularityVActor.GetPointer()->SetMapper(mapper);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->EdgeVisibilityOn();
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetEdgeColor(1, 1, 0);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetLineWidth(6);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetPointSize(12);
//    //m_vtkSingularityActor.GetPointer()->GetProperty()->SetColor(1, 0, 0);
//    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetRepresentationToSurface();
//    m_vtkSingularityVActor->GetProperty()->SetRenderLinesAsTubes(1);
//    m_vtkSingularityVActor->GetProperty()->SetRenderPointsAsSpheres(1);
//    //m_vtkSingularityActor->GetProperty()->SetVertexVisibility(1);
//    m_vtkSingularityVActor->GetProperty()->SetVertexColor(1, 0, 0);

//    if (id == 0) {
//        //vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
//        scalarBar->SetLookupTable(mapper->GetLookupTable());
//        scalarBar->SetTitle("Valence");
//        scalarBar->SetNumberOfLabels(valences.size());
//        scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
//        scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);

//        vtkSmartPointer<vtkColorTransferFunction> ctf = vtkSmartPointer<vtkColorTransferFunction>::New();
//        for (auto v : valences) {
//            colorLookupTable->GetColor(v, dcolor);
//            ctf->AddRGBPoint(v, dcolor[0], dcolor[1], dcolor[2]);
//        }
////        ctf->AddRGBPoint(1, 0, 0, 1.0);
////        ctf->AddRGBPoint(2, 1.0, 1.0, 1.0);
////        ctf->AddRGBPoint(3, 0, 1.0, 0);
////        ctf->AddRGBPoint(4, 0, 1.0, 1.0);
////        ctf->AddRGBPoint(5, 1.0, 1.0, 0.0);
////        ctf->AddRGBPoint(6, 1.0, 0, 0.0);
////        ctf->AddRGBPoint(7, 1.0, 0, 1.0);
////        ctf->AddRGBPoint(8, 0.0, 0, 0.0);
//        ctf->SetColorSpaceToDiverging();
//        ctf->Build();
//        mapper->SetLookupTable(ctf);
//        scalarBar->SetLookupTable(ctf);
//        //m_renderer->RemoveActor2D(scalarBar.GetPointer());
//        //m_renderer->AddActor2D(scalarBar.GetPointer());
//    } else {
//        //vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
//        scalarBar->SetLookupTable(mapper->GetLookupTable());
//        scalarBar->SetTitle("Index");
//        scalarBar->SetNumberOfLabels(indices.size());
//        scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
//        scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);

//        vtkSmartPointer<vtkColorTransferFunction> ctf = vtkSmartPointer<vtkColorTransferFunction>::New();

//        for (auto v : indices) {
//            colorLookupTable->GetColor(v, dcolor);
//            ctf->AddRGBPoint(v, dcolor[0], dcolor[1], dcolor[2]);
//        }
////        ctf->AddRGBPoint(-0.125, 0, 0, 1.0);
////        ctf->AddRGBPoint(0, 1.0, 1.0, 1.0);
////        ctf->AddRGBPoint(0.125, 0, 1.0, 0);
////        ctf->AddRGBPoint(0.25, 0, 1.0, 1.0);
////        ctf->AddRGBPoint(0.375, 1.0, 0, 0.0);
////        ctf->AddRGBPoint(0.5, 1.0, 1.0, 0.0);
//        ctf->SetColorSpaceToDiverging();
//        ctf->Build();
//        mapper->SetLookupTable(ctf);
//        scalarBar->SetLookupTable(ctf);
//        //m_renderer->RemoveActor2D(scalarBar.GetPointer());
//        //m_renderer->AddActor2D(scalarBar.GetPointer());
//    }
//}

void MainWindow::AddSingularityVActor(const char* filename) {
    const auto id = singularityDisplayTypeComboBox->currentIndex();
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    colorLookupTable->SetTableRange(0, 1);
    colorLookupTable->Build();
    // Generate the colors for each point based on the color map
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    double dcolor[3] = { 1.0, 0, 0.0 };
    //colorLookupTable->GetColor(0, dcolor);
    unsigned char color[3];
    for (unsigned int j = 0; j < 3; j++)
        color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);

    for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
        //colors->InsertNextTupleValue(color);
        colors->InsertNextTypedTuple(color);
    reader->GetOutput()->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->ImmediateModeRenderingOn();
    m_vtkSingularityVActor.GetPointer()->SetMapper(mapper);
    m_vtkSingularityVActor.GetPointer()->GetProperty()->EdgeVisibilityOn();
    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetEdgeColor(1, 1, 0);
    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetLineWidth(6);
    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetPointSize(12);
    //m_vtkSingularityActor.GetPointer()->GetProperty()->SetColor(1, 0, 0);
    m_vtkSingularityVActor.GetPointer()->GetProperty()->SetRepresentationToSurface();
    m_vtkSingularityVActor->GetProperty()->SetRenderLinesAsTubes(1);
    m_vtkSingularityVActor->GetProperty()->SetRenderPointsAsSpheres(1);
    //m_vtkSingularityActor->GetProperty()->SetVertexVisibility(1);
    m_vtkSingularityVActor->GetProperty()->SetVertexColor(1, 0, 0);
}

void MainWindow::AddSingularityEActor(const char* filename)
{
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename);
    reader->Update();
    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    colorLookupTable->SetTableRange(0, 5);
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

    const auto id = singularityDisplayTypeComboBox->currentIndex();
    if (id == 0) {
        auto cellDatas = GetScalarFields(reader, 3);
        cout << "------------ number of cellDatas = " << cellDatas.size() << endl;
        for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
            colors->InsertNextTypedTuple(color_valence[int(cellDatas[1][i]) - 1]);
    }
    else {
        auto cellDatas = GetDoubleScalarField(reader, "index", 3);
        cout << "------------ number of cellDatas = " << cellDatas.size() << endl;
        for (int i = 0; i < reader->GetOutput()->GetNumberOfCells(); i++)
            colors->InsertNextTypedTuple(color_valence[3 - int(cellDatas[i] * 4)]);
    }

    reader->GetOutput()->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->ImmediateModeRenderingOn();
    m_vtkSingularityEActor.GetPointer()->SetMapper(mapper);
    m_vtkSingularityEActor.GetPointer()->GetProperty()->EdgeVisibilityOn();
    m_vtkSingularityEActor.GetPointer()->GetProperty()->SetEdgeColor(1, 1, 0);
    m_vtkSingularityEActor.GetPointer()->GetProperty()->SetLineWidth(6);
    m_vtkSingularityEActor.GetPointer()->GetProperty()->SetPointSize(12);
    //m_vtkSingularityActor.GetPointer()->GetProperty()->SetColor(1, 0, 0);
    m_vtkSingularityEActor.GetPointer()->GetProperty()->SetRepresentationToSurface();
    m_vtkSingularityEActor->GetProperty()->SetRenderLinesAsTubes(1);
    m_vtkSingularityEActor->GetProperty()->SetRenderPointsAsSpheres(1);
    //m_vtkSingularityActor->GetProperty()->SetVertexVisibility(1);
    m_vtkSingularityEActor->GetProperty()->SetVertexColor(1, 0, 0);
    m_vtkSingularityEActor->GetProperty()->SetAmbient(0.1);
}

//void MainWindow::DisplaySheets(const std::vector<int>& sheetIndices)
//{
//    for (int id = 0; id < sheetIndices.size(); id++) {
//        const int sheetIndex = sheetIndices.at(id);
//        QListWidgetItem* item = ui->sheetListWidget->item(sheetIndex + 1);
//        item->setCheckState(Qt::Checked);
//        m_renderer->AddActor(m_vtkSheetActors.at(sheetIndex));
//    }
//}

void MainWindow::DeactivateAllSheets()
{
    ui->sheetDockWidget->listWidget->item(0)->setCheckState(Qt::Unchecked);
    for (int id = 0; id < ui->sheetDockWidget->numOfListItems; id++) {
        QListWidgetItem* item = ui->sheetDockWidget->listWidget->item(id + 1);
        item->setCheckState(Qt::Unchecked);
        m_renderer->RemoveActor(ui->sheetDockWidget->m_vtkActors.at(id));
    }
}

bool MainWindow::IsSheetsEmpty() const
{
    return ui->sheetDockWidget->m_vtkActors.empty();
}


LoadingThread::LoadingThread(HexDockWidget* hexDockWidget, const std::string path, const std::string prefix, QString iconFilename)
    : hexDockWidget(hexDockWidget), strFolderPath(path), prefix(prefix), iconFilename(iconFilename) {

}
LoadingThread::~LoadingThread() {

}
void LoadingThread::process()
{
    hexDockWidget->loadListWidgetItems(strFolderPath, prefix, iconFilename);
    emit finished();
}

void MainWindow::runThread(QThread* thread, LoadingThread* loadingThread) {
    loadingThread->moveToThread(thread);
    connect(thread, SIGNAL(started()), loadingThread, SLOT (process()));
    connect(loadingThread, SIGNAL (finished()), thread, SLOT (quit()));
    connect(loadingThread, SIGNAL (finished()), loadingThread, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
}

void MainWindow::on_sheetListAction_clicked()
{
    m_renderer->RemoveActor(m_vtkSingularityActor.GetPointer());
    m_renderer->RemoveActor(m_vtkBaseComplexActor.GetPointer());
    m_renderer->GetActors()->RemoveAllItems();

//    ui->componentDockWidget->moveToThread(&componentThread);
//    ui->chordDockWidget->moveToThread(&chordThread);
//    ui->sheetDockWidget->moveToThread(&sheetThread);
//    ui->chordFaceAndEdgeDockWidget->moveToThread(&chordFaceAndEdgeThread);
//    ui->sheetFaceAndEdgeDockWidget->moveToThread(&sheetFaceAndEdgeThread);
//    connect(&componentThread, &QThread::finished, ui->componentDockWidget, &QObject::deleteLater);

//    QThread* thread = new QThread;
//    LoadingThread *loadingThread =
//            new LoadingThread(ui->componentDockWidget, strFolderPath, "ComponentCells", QString("../icon/volume.png"));
//    runThread(thread, loadingThread);

//    thread = new QThread;
//    loadingThread = new LoadingThread(ui->chordDockWidget, strFolderPath, "ChordCells", QString("../icon/chord.png"));
//    runThread(thread, loadingThread);

//    thread = new QThread;
//    loadingThread = new LoadingThread(ui->sheetDockWidget, strFolderPath, "SheetCells", QString("../icon/sheets.png"));
//    runThread(thread, loadingThread);

//    thread = new QThread;
//    loadingThread = new LoadingThread(ui->chordFaceAndEdgeDockWidget, strFolderPath, "ChordFacesAndEdges", QString("../icon/chord1.png"));
//    runThread(thread, loadingThread);

//    thread = new QThread;
//    loadingThread = new LoadingThread(ui->sheetFaceAndEdgeDockWidget, strFolderPath, "SheetFacesAndEdges", QString("../icon/sheet.png"));
//    runThread(thread, loadingThread);

    if (ui->componentDockWidget != NULL) {
        componentThread = new QThread;
        componentLoadingThread = new LoadingThread(ui->componentDockWidget, strFolderPath, "ComponentCells", QString("../icon/volume.png"));
        runThread(componentThread, componentLoadingThread);
    }
    if (ui->chordDockWidget != NULL) {
        chordThread = new QThread;
        chordLoadingThread = new LoadingThread(ui->chordDockWidget, strFolderPath, "ChordCells", QString("../icon/chord.png"));
        runThread(chordThread, chordLoadingThread);
    }
    if (ui->sheetDockWidget != NULL) {
        sheetThread = new QThread;
        sheetLoadingThread = new LoadingThread(ui->sheetDockWidget, strFolderPath, "SheetCells", QString("../icon/sheets.png"));
        runThread(sheetThread, sheetLoadingThread);
    }
    if (ui->chordFaceAndEdgeDockWidget != NULL) {
        chordFaceAndEdgeThread = new QThread;
        chordFaceAndEdgeLoadingThread = new LoadingThread(ui->chordFaceAndEdgeDockWidget, strFolderPath, "ChordFacesAndEdges", QString("../icon/chord1.png"));
        runThread(chordFaceAndEdgeThread, chordFaceAndEdgeLoadingThread);
    }
    if (ui->chordCurveDockWidget != NULL) {
        chordCurveThread = new QThread;
        chordCurveLoadingThread = new LoadingThread(ui->chordCurveDockWidget, strFolderPath, "ChordCurves", QString("../icon/curve.png"));
        runThread(chordCurveThread, chordCurveLoadingThread);
    }
    if (ui->sheetFaceAndEdgeDockWidget != NULL) {
        sheetFaceAndEdgeThread = new QThread;
        sheetFaceAndEdgeLoadingThread = new LoadingThread(ui->sheetFaceAndEdgeDockWidget, strFolderPath, "SheetFacesAndEdges", QString("../icon/sheet.png"));
        runThread(sheetFaceAndEdgeThread, sheetFaceAndEdgeLoadingThread);
    }
    if (ui->sheetDualDockWidget != NULL) {
        sheetDualThread = new QThread;
        sheetDualLoadingThread = new LoadingThread(ui->sheetDualDockWidget, strFolderPath, "SheetDual", QString("../icon/dual.png"));
        runThread(sheetDualThread, sheetDualLoadingThread);
    }
    if (ui->quadDualDockWidget != NULL) {
        quadDualThread = new QThread;
        quadDualLoadingThread = new LoadingThread(ui->quadDualDockWidget, strFolderPath, "QuadDual", QString("../icon/dual.png"));
        runThread(quadDualThread, quadDualLoadingThread);
        if (ui->sheetDecompositionsDockWidget != NULL)
            ui->sheetDecompositionsDockWidget->loadTreeWidgetItems(strFilename, "Decomposition", QString("../icon/dec.png"));
    }
    if (ui->faceSegmentDockWidget != NULL) {
        faceSegmentThread = new QThread;
        faceSegmentLoadingThread = new LoadingThread(ui->faceSegmentDockWidget, strFolderPath, "FacePatches", QString("../icon/face_patch.png"));
        runThread(faceSegmentThread, faceSegmentLoadingThread);
    }
    if (ui->singularFacesDockWidget != NULL) {
        singularFacesThread = new QThread;
        singularFacesLoadingThread = new LoadingThread(ui->singularFacesDockWidget, strFolderPath, "SingularFaces", QString("../icon/sf.png"));
        runThread(singularFacesThread, singularFacesLoadingThread);
    }
    if (ui->sliceDockWidget != NULL) {
        sliceThread = new QThread;
        sliceLoadingThread = new LoadingThread(ui->sliceDockWidget, strFolderPath, "Slice", QString("../icon/slice.png"));
        runThread(sliceThread, sliceLoadingThread);
    }
    if (ui->sheetCellsConnectedSingularitiesDockWidget != NULL) {
        sheetCellsConnectedSingularitiesThread = new QThread;
        sheetCellsConnectedSingularitiesLoadingThread = new LoadingThread(ui->sheetCellsConnectedSingularitiesDockWidget, strFolderPath, "SheetConnectedSingularities", QString("../icon/sheets.png"));
        runThread(sheetCellsConnectedSingularitiesThread, sheetCellsConnectedSingularitiesLoadingThread);
    }
    if (ui->matrixDockWidget != NULL) {
        ui->matrixDockWidget->loadMatrices(strFilename);
    }
//    ui->componentDockWidget->loadListWidgetItems(strFolderPath, "ComponentCells", QString("../icon/volume.png"));
//    ui->chordDockWidget->loadListWidgetItems(strFolderPath, "ChordCells", QString("../icon/chord.png"));
//    ui->sheetDockWidget->loadListWidgetItems(strFolderPath, "SheetCells", QString("../icon/sheets.png"));
//    ui->chordFaceAndEdgeDockWidget->loadListWidgetItems(strFolderPath, "ChordFacesAndEdges", QString("../icon/chord1.png"));
//    ui->sheetFaceAndEdgeDockWidget->loadListWidgetItems(strFolderPath, "SheetFacesAndEdges", QString("../icon/sheet.png"));

    baseComplexAction->setEnabled(true);
    singularityAction->setEnabled(true);
    opacityBaseComplexSlider->setEnabled(true);
    opacityBaseComplexSpinBox->setEnabled(true);
    opacitySingularitySlider->setEnabled(true);
    opacitySingularitySpinBox->setEnabled(true);

    AddBaseComplexActor((QString::fromStdString(strFolderPath) + QString("/BaseComplexEdges.vtk")).toStdString().c_str());
    AddSingularityActor((QString::fromStdString(strFolderPath) + QString("/singularities.vtk")).toStdString().c_str());
    AddSingularityVActor((QString::fromStdString(strFolderPath) + QString("/singularV.vtk")).toStdString().c_str());
    AddSingularityEActor((QString::fromStdString(strFolderPath) + QString("/singularE.vtk")).toStdString().c_str());
    sheetListAction->setDisabled(true);
    componentListAction->setDisabled(true);
    ui->qvtkWidget->update();

}

void MainWindow::addTreeRoot(QString name, QString description, std::vector<QString> children)
{
    // QTreeWidgetItem(QTreeWidget * parent, int type = Type)
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->sheetTreeWidget);

    // QTreeWidgetItem::setText(int column, const QString & text)
    treeItem->setText(0, name);
    //treeItem->setText(1, description);
    treeItem->setIcon(0, QIcon("../icon/sheets.png"));
    treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    treeItem->setCheckState(0, Qt::Unchecked); // AND initialize check state
    for (size_t i = 0; i < children.size(); i++)
        addTreeChild(treeItem, children.at(i), "Child");
}

void MainWindow::addTreeChild(QTreeWidgetItem *parent, QString name, QString description)
{
    // QTreeWidgetItem(QTreeWidget * parent, int type = Type)
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();

    // QTreeWidgetItem::setText(int column, const QString & text)
    treeItem->setText(0, name);
    //treeItem->setText(1, description);
    treeItem->setIcon(0, QIcon("../icon/volume.png"));
    treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    treeItem->setCheckState(0, Qt::Unchecked); // AND initialize check state
    // QTreeWidgetItem::addChild(QTreeWidgetItem * child)
    parent->addChild(treeItem);
}

void MainWindow::deleteAllItems(QTreeWidget *qTreeWidget)
{
    for (int i = qTreeWidget->topLevelItemCount() - 1; i >=0; --i) {
        QTreeWidgetItem *treeItem = qTreeWidget->takeTopLevelItem(i);
        for(int j = treeItem->childCount() - 1; j >= 0; j--) {
            treeItem->removeChild(treeItem->child(j));
        }
        delete qTreeWidget->takeTopLevelItem(i);
    }
}

void MainWindow::on_componentListAction_clicked()
{
    on_sheetListAction_clicked();
}

void MainWindow::on_baseComplexAction_clicked()
{
    if (baseComplexAction->isChecked()) {
        m_renderer->AddActor(m_vtkBaseComplexActor.GetPointer());
        //m_mouseInteractorStyle->SetData(m_vtkBaseComplexReader->GetOutput());
        m_mouseInteractorStyle->SetData(((vtkPolyDataMapper*) m_vtkBaseComplexActor.GetPointer()->GetMapper())->GetInput());
    }
    else {
        m_renderer->RemoveActor(m_vtkBaseComplexActor.GetPointer());
        m_mouseInteractorStyle->SetData(m_vtkUnstructuredGridReader->GetOutput());
    }
    m_renderer->SetBackground(1, 1, 1);
    ui->qvtkWidget->update();
}

void MainWindow::on_singularityAction_clicked()
{
//    if (singularityAction->isChecked()) {
//        m_renderer->AddActor(m_vtkSingularityActor.GetPointer());
//        //m_mouseInteractorStyle->SetData(m_vtkSingularityReader.GetPointer()->GetOutput());
//        m_mouseInteractorStyle->SetData(((vtkPolyDataMapper*) m_vtkSingularityActor.GetPointer()->GetMapper())->GetInput());
//    } else {
//        m_renderer->RemoveActor(m_vtkSingularityActor.GetPointer());
//        m_mouseInteractorStyle->SetData(m_vtkUnstructuredGridReader->GetOutput());
//    }
    if (singularityAction->isChecked()) {
        m_renderer->AddActor(m_vtkSingularityVActor.GetPointer());
        m_renderer->AddActor(m_vtkSingularityEActor.GetPointer());
        m_renderer->AddActor2D(scalarBar.GetPointer());
        //m_mouseInteractorStyle->SetData(m_vtkSingularityReader.GetPointer()->GetOutput());
        m_mouseInteractorStyle->SetData(((vtkPolyDataMapper*) m_vtkSingularityEActor.GetPointer()->GetMapper())->GetInput());
    } else {
        m_renderer->RemoveActor(m_vtkSingularityVActor.GetPointer());
        m_renderer->RemoveActor(m_vtkSingularityEActor.GetPointer());
        m_renderer->RemoveActor2D(scalarBar.GetPointer());
        m_mouseInteractorStyle->SetData(m_vtkUnstructuredGridReader->GetOutput());
    }
    m_renderer->SetBackground(1, 1, 1);
    ui->qvtkWidget->update();
}

void MainWindow::on_stopLoading_clicked()
{
    if (componentThread && componentThread->isRunning()) {
        componentThread->exit();
    }
}

void MainWindow::on_opacityChanged(int val)
{
    double dval = ((double)val)/opacitySlider->maximum();
    opacitySlider->setValue(val);
    opacitySpinBox->setValue(dval);
    m_vtkActor.GetPointer()->GetProperty()->SetOpacity(dval);
    ui->qvtkWidget->update();
}
void MainWindow::on_opacityChanged(double val)
{
    int ival = val*opacitySlider->maximum();
    opacitySpinBox->setValue(val);
    opacitySlider->setValue(ival);
    m_vtkActor.GetPointer()->GetProperty()->SetOpacity(val);
    ui->qvtkWidget->update();
}

void MainWindow::on_opacityBaseComplexChanged(int val)
{
    double dval = ((double)val)/opacityBaseComplexSlider->maximum();
    opacityBaseComplexSlider->setValue(val);
    opacityBaseComplexSpinBox->setValue(dval);
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetOpacity(dval);
    ui->qvtkWidget->update();
}

void MainWindow::on_opacityBaseComplexChanged(double val)
{
    int ival = val*opacityBaseComplexSlider->maximum();
    opacityBaseComplexSpinBox->setValue(val);
    opacityBaseComplexSlider->setValue(ival);
    m_vtkBaseComplexActor.GetPointer()->GetProperty()->SetOpacity(val);
    ui->qvtkWidget->update();
}

void MainWindow::on_opacitySingularityChanged(int val)
{
    double dval = ((double)val)/opacitySingularitySlider->maximum();
    opacitySingularitySlider->setValue(val);
    opacitySingularitySpinBox->setValue(dval);
    m_vtkSingularityActor.GetPointer()->GetProperty()->SetOpacity(dval);
    //m_vtkSingularityVActor.GetPointer()->GetProperty()->SetOpacity(dval);
    m_vtkSingularityEActor.GetPointer()->GetProperty()->SetOpacity(dval);
    ui->qvtkWidget->update();
}

void MainWindow::on_opacitySingularityChanged(double val)
{
    int ival = val*opacitySingularitySlider->maximum();
    opacitySingularitySpinBox->setValue(val);
    opacitySingularitySlider->setValue(ival);
    m_vtkSingularityActor.GetPointer()->GetProperty()->SetOpacity(val);
    //m_vtkSingularityVActor.GetPointer()->GetProperty()->SetOpacity(val);
    m_vtkSingularityEActor.GetPointer()->GetProperty()->SetOpacity(val);
    ui->qvtkWidget->update();
}

//
// routine to convert HSV to RGB
//
// Reference:  Foley, van Dam, Feiner, Hughes,
//		"Computer Graphics Principles and Practices,"
//		Additon-Wesley, 1990, pp592-593.
void Hsv2Rgb(float hsv[3], float rgb[3])
{
    float h, s, v;			// hue, sat, value
    float r, g, b;			// red, green, blue
    float i, f, p, q, t;		// interim values

    // guarantee valid input:
    h = hsv[0] / 60.;
    while( h >= 6. )	h -= 6.;
    while( h <  0. ) 	h += 6.;

    s = hsv[1];
    if( s < 0. )
        s = 0.;
    if( s > 1. )
        s = 1.;

    v = hsv[2];
    if( v < 0. )
        v = 0.;
    if( v > 1. )
        v = 1.;

    // if sat==0, then is a gray:
    if( s == 0.0 )
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }

    // get an rgb from the hue itself:
    i = floor( h );
    f = h - i;
    p = v * ( 1. - s );
    q = v * ( 1. - s*f );
    t = v * ( 1. - ( s * (1.-f) ) );

    switch( (int) i )
    {
    case 0:
        r = v;	g = t;	b = p;
        break;

    case 1:
        r = q;	g = v;	b = p;
        break;

    case 2:
        r = p;	g = v;	b = t;
        break;

    case 3:
        r = p;	g = q;	b = v;
        break;

    case 4:
        r = t;	g = p;	b = v;
        break;

    case 5:
        r = v;	g = p;	b = q;
        break;
    }

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

void getRainbowBlendedWhite(double scalar, double min, double max, double color[3])
{
    const int segs = 4; // white-blue-green-red-purple
    const int seg = max/scalar;
    double t = max/scalar - seg;
    switch (seg)
    {
    case 0: // white-blue
    {
        float hsv[3] = {0.0f, 0.0f, 0.0f};
        hsv[0] = 240.0f;
        hsv[1] = t;
        hsv[2] = 1.0f;
        //Hsv2Rgb(hsv, rgb);
        break;
    }
    }
}

void MainWindow::ModifyColorMap(HexDockWidget* dockWidget, vtkSmartPointer<vtkLookupTable> colorLookupTable,
                                double tableMaxRange, int fieldId, const char* iconFilename) {
    colorLookupTable->SetTableRange(0, tableMaxRange);
    colorLookupTable->Build();
    for (int i = 0; i < dockWidget->numOfListItems; i++) {
        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors-");
        double dcolor[3];
        unsigned char ccolor[3];
        if (fieldId != INT_MAX) {
            int colorid = dockWidget->cellDataFields[i][fieldId][0];
            colorLookupTable->GetColor(colorid, dcolor);
        } else colorLookupTable->GetColor(i, dcolor);
        for (unsigned int j = 0; j < 3; j++)
            ccolor[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
        //////////////////////////////////////
        QPixmap pixmap = QPixmap(iconFilename);
        GetColorPixmap(ccolor, pixmap);
        QIcon icon = QIcon(pixmap);
        dockWidget->listWidget->item(i+1)->setIcon(icon);
        for(int k = 0; k < dockWidget->m_vtkActors.at(i)->GetMapper()->GetInput()->GetNumberOfCells(); k++)
            // colors->InsertNextTupleValue(ccolor);
            colors->InsertNextTypedTuple(ccolor);
        dockWidget->m_vtkActors.at(i)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
        /////////////////////////////////////
    }
}

void MainWindow::on_sheetCellDataFieldCurrentIndexChanged(int index)
{
    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    if (index == COMPONENT_COLOR) {
        if (ui->componentDockWidget != NULL) {
        ModifyColorMap(ui->componentDockWidget, colorLookupTable, 6, 1, "../icon/volume.png");
        for (int i = 0; i < ui->sheetDockWidget->numOfListItems; i++)
            ui->sheetDockWidget->listWidget->item(i+1)->setIcon(QIcon("../icon/sheets.png"));
        }
    } else if (index == RAW_COMPONENT_ID) {
        if (ui->componentDockWidget != NULL) {
        ModifyColorMap(ui->componentDockWidget, colorLookupTable, ui->componentDockWidget->numOfListItems, INT_MAX, "../icon/volume.png");
        for (int i = 0; i < ui->sheetDockWidget->numOfListItems; i++)
            ui->sheetDockWidget->listWidget->item(i+1)->setIcon(QIcon("../icon/sheets.png"));
        }
    } else if (index == COMPONENT_ID) {
        ;
    } else if (index == CHORD_ID) {
        if (ui->chordDockWidget != NULL)
        ModifyColorMap(ui->chordDockWidget, colorLookupTable, ui->chordDockWidget->numOfListItems, INT_MAX, "../icon/chord.png");
        if (ui->chordFaceAndEdgeDockWidget != NULL)
        ModifyColorMap(ui->chordFaceAndEdgeDockWidget, colorLookupTable, ui->chordFaceAndEdgeDockWidget->numOfListItems, INT_MAX, "../icon/chord1.png");
    } else if (index == SHEET_ID) {
        ModifyColorMap(ui->sheetDockWidget, colorLookupTable, ui->sheetDockWidget->numOfListItems, INT_MAX, "../icon/sheets.png");
        if (ui->sheetFaceAndEdgeDockWidget != NULL)
        ModifyColorMap(ui->sheetFaceAndEdgeDockWidget, colorLookupTable, ui->sheetFaceAndEdgeDockWidget->numOfListItems, INT_MAX, "../icon/sheet.png");
    }
    if (index != CHORD_LINK)
    for (int id = 0; id < ui->sheetDockWidget->numOfListItems; id++) {
        vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
        if (index == COMPONENT_COLOR) colorLookupTable->SetTableRange(0, 6);
        else if (index == RAW_COMPONENT_ID) {
            if (ui->componentDockWidget != NULL) colorLookupTable->SetTableRange(0, ui->componentDockWidget->numOfListItems);
        }
        else if (index == COMPONENT_ID) {
            std::vector<int> fields = ui->sheetDockWidget->cellDataFields[id][index];
            std::unordered_set<int> s(fields.begin(), fields.end());
            colorLookupTable->SetTableRange(0, s.size());
        }
        else if (index == SHEET_ID) colorLookupTable->SetTableRange(0, ui->sheetDockWidget->numOfListItems);
        colorLookupTable->Build();
        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors-");
        double dcolor[3];
        unsigned char color[3];
        int fieldid = index < 3 ? index : 3;
        for (int i = 0; i < ui->sheetDockWidget->m_vtkActors.at(id)->GetMapper()->GetInput()->GetNumberOfCells(); i++) {
            colorLookupTable->GetColor(ui->sheetDockWidget->cellDataFields[id][fieldid][i], dcolor);
            for(unsigned int j = 0; j < 3; j++)
                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
            //colors->InsertNextTupleValue(color);
            colors->InsertNextTypedTuple(color);
        }
        ui->sheetDockWidget->m_vtkActors.at(id)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
    }

    if (index != CHORD_LINK)
        if (ui->chordDockWidget != NULL)
    for (int id = 0; id < ui->chordDockWidget->numOfListItems; id++) {
        vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
        if (index == COMPONENT_COLOR) colorLookupTable->SetTableRange(0, 6);
        else if (index == RAW_COMPONENT_ID) colorLookupTable->SetTableRange(0, ui->componentDockWidget->numOfListItems);
        else if (index == COMPONENT_ID) {
            std::vector<int> fields = ui->chordDockWidget->cellDataFields[id][index];
            std::unordered_set<int> s(fields.begin(), fields.end());
            colorLookupTable->SetTableRange(0, s.size());
        }
        else if (index == SHEET_ID) colorLookupTable->SetTableRange(0, ui->chordDockWidget->numOfListItems);
        colorLookupTable->Build();
        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors-");
        double dcolor[3];
        unsigned char color[3];
        int fieldid = index < 3 ? index : 3;
        for (int i = 0; i < ui->chordDockWidget->m_vtkActors.at(id)->GetMapper()->GetInput()->GetNumberOfCells(); i++) {
            colorLookupTable->GetColor(ui->chordDockWidget->cellDataFields[id][fieldid][i], dcolor);
            for(unsigned int j = 0; j < 3; j++)
                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
            // colors->InsertNextTupleValue(color);
            colors->InsertNextTypedTuple(color);
        }
        ui->chordDockWidget->m_vtkActors.at(id)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
    }

//    if (index == CHORD_LINK)
//    for (int id = 0; id < ui->chordFaceAndEdgeDockWidget->numOfListItems; id++) {
//        vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
//        std::vector<int> fields = ui->chordFaceAndEdgeDockWidget->cellDataFields[id][0];
//        std::unordered_set<int> s(fields.begin(), fields.end());
//        colorLookupTable->SetTableRange(0, s.size());
//        colorLookupTable->Build();
//        // Generate the colors for each point based on the color map
//        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
//        colors->SetNumberOfComponents(3);
//        colors->SetName("Colors-");
//        double dcolor[3];
//        unsigned char color[3];
//        int fieldid = 0;
//        for (int i = 0; i < ui->chordFaceAndEdgeDockWidget->m_vtkActors.at(id)->GetMapper()->GetInput()->GetNumberOfCells(); i++) {
//            colorLookupTable->GetColor(ui->chordFaceAndEdgeDockWidget->cellDataFields[id][fieldid][i], dcolor);
//            for(unsigned int j = 0; j < 3; j++)
//                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
//            colors->InsertNextTupleValue(color);
//        }
//        ui->chordFaceAndEdgeDockWidget->m_vtkActors.at(id)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
//    }

    if (index == MAIN_SHEET && ui->sheetDecompositionsDockWidget != NULL && ui->sheetDecompositionsDockWidget->currentDecompositionItem != NULL) {
        const auto& sheets_coverSheetIds = ui->sheetDecompositionsDockWidget->sheets_coverSheetIds[ui->sheetDecompositionsDockWidget->currentDecompositionId];
        int colorid = 0;
        for (auto sheetid : sheets_coverSheetIds) {
            vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
            colorLookupTable->SetTableRange(0, sheets_coverSheetIds.size());
            colorLookupTable->Build();
            vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
            colors->SetNumberOfComponents(3);
            colors->SetName("Colors-");
            double dcolor[3];
            unsigned char color[3];
            int fieldid = index < 3 ? index : 3;
            for (int i = 0; i < ui->sheetDecompositionsDockWidget->m_vtkActors->at(sheetid)->GetMapper()->GetInput()->GetNumberOfCells(); i++) {
                colorLookupTable->GetColor(colorid, dcolor);
                for(unsigned int j = 0; j < 3; j++)
                    color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
                // colors->InsertNextTupleValue(color);
                colors->InsertNextTypedTuple(color);
            }
            ui->sheetDecompositionsDockWidget->m_vtkActors->at(sheetid)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
            ui->sheetDockWidget->m_vtkActors.at(sheetid)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
            ui->quadDualDockWidget->m_vtkActors.at(sheetid)->GetMapper()->GetInput()->GetCellData()->SetScalars(colors);
            colorid++;
        }
    }

    ui->qvtkWidget->update();
}

//void MainWindow::on_treeSheetItemClicked(QTreeWidgetItem * item, int column)
//{
//    QVariant variant = item->data(0, Qt::UserRole);
//    int data = variant.toInt();
//    //QString text = QString("tree %1:%2 is clicked.").arg(item->text(column)).arg(data);
//    //qDebug() << text;
//    std::string str = item->text(column).toStdString();
//    int pos = str.find_first_of("sheet_");
//    if (pos == str.npos)
//    {
//        pos = str.find_first_of("all");
//        if (pos != str.npos){
//        for (int id = 0; id < sheetsNum; id++)
//            if (item->checkState(column) == Qt::Checked) {
//                m_renderer->AddActor(m_vtkSheetActors.at(id));
//                for (int i = 0; i < ui->sheetListWidget->count(); i++)
//                    ui->sheetListWidget->item(i)->setCheckState(Qt::Checked);
//            }
//            else if (item->checkState(column) == Qt::Unchecked) {
//                m_renderer->RemoveActor(m_vtkSheetActors.at(id));
//                for (int i = 0; i < ui->sheetListWidget->count(); i++)
//                    ui->sheetListWidget->item(i)->setCheckState(Qt::Unchecked);
//            }
//        }
//    }
//    else
//    {
//        if (pos == 0) {  // sheet
//            pos = str.find_first_of('_');
//            int id = QString(str.substr(pos+1).c_str()).toInt();
//            //qDebug() << QString("id: ") << id;
//            if (item->checkState(column) == Qt::Checked) {
//                m_renderer->AddActor(m_vtkSheetActors.at(id));
//                //qDebug() << QString("components: ") << item->childCount();
//                for (int i = 0; i < item->childCount(); i++)
//                    item->child(i)->setCheckState(column, Qt::Checked);
//            }
//            else if (item->checkState(column) == Qt::Unchecked) {
//                m_renderer->RemoveActor(m_vtkSheetActors.at(id));
//                //qDebug() << QString("components: ") << item->childCount();
//                for (int i = 0; i < item->childCount(); i++)
//                    item->child(i)->setCheckState(column, Qt::Unchecked);
//            }
//        } else { // component
//            pos = str.find_first_of('_');
//            int id = QString(str.substr(pos+1).c_str()).toInt();
//            // qDebug() << QString("id: ") << id;
//            if (item->checkState(column) == Qt::Checked)
//                m_renderer->AddActor(m_vtkComponentActors.at(id));
//            else if (item->checkState(column) == Qt::Unchecked)
//                m_renderer->RemoveActor(m_vtkComponentActors.at(id));
//        }
//    }
//    ui->qvtkWidget->update();
//}

void MainWindow::on_sheetDisplyTypeCurrentIndexChanged(int index) {
//    if (ui->sheetDecompositionsDockWidget && ui->sheetDockWidget)
//        for (int id = 0; id < ui->sheetDockWidget->numOfListItems; id++)
//            ui->sheetDockWidget->m_renderer->RemoveActor(ui->sheetDockWidget->m_vtkActors.at(id));
//    if (ui->sheetDecompositionsDockWidget && ui->sheetDualDockWidget)
//        for (int id = 0; id < ui->sheetDualDockWidget->numOfListItems; id++)
//            ui->sheetDualDockWidget->m_renderer->RemoveActor(ui->sheetDualDockWidget->m_vtkActors.at(id));
//    if (ui->sheetDecompositionsDockWidget && ui->quadDualDockWidget)
//        for (int id = 0; id < ui->quadDualDockWidget->numOfListItems; id++)
//            ui->quadDualDockWidget->m_renderer->RemoveActor(ui->quadDualDockWidget->m_vtkActors.at(id));
    if (index == 0) { // hex
        if (ui->sheetDecompositionsDockWidget && ui->sheetDockWidget)
            ui->sheetDecompositionsDockWidget->setActors(&ui->sheetDockWidget->m_vtkActors);
    } else if (index == 1) { // quad
        if (ui->sheetDecompositionsDockWidget && ui->sheetDualDockWidget)
            ui->sheetDecompositionsDockWidget->setActors(&ui->sheetDualDockWidget->m_vtkActors);
    } else if (index == 2) { // edge
        if (ui->sheetDecompositionsDockWidget && ui->quadDualDockWidget)
            ui->sheetDecompositionsDockWidget->setActors(&ui->quadDualDockWidget->m_vtkActors);
    }
}

void MainWindow::on_singularityDisplyTypeCurrentIndexChanged(int index) {
    AddSingularityVActor((QString::fromStdString(strFolderPath) + QString("/singularV.vtk")).toStdString().c_str());
    AddSingularityEActor((QString::fromStdString(strFolderPath) + QString("/singularE.vtk")).toStdString().c_str());
    ui->qvtkWidget->update();
}

void MainWindow::on_ModelClicked(QListWidgetItem * item) {
    if (ui->componentDockWidget != NULL && !ui->componentDockWidget->isLoadingFinished)  return; // componentThread->wait();
    if (ui->chordDockWidget != NULL && !ui->chordDockWidget->isLoadingFinished) return; // chordThread->wait();
    if (ui->sheetDockWidget != NULL && !ui->sheetDockWidget->isLoadingFinished) return; // sheetThread->wait();
    if (ui->chordFaceAndEdgeDockWidget != NULL && !ui->chordFaceAndEdgeDockWidget->isLoadingFinished) return; // chordFaceAndEdgeThread->wait();
    if (ui->chordCurveDockWidget != NULL && !ui->chordCurveDockWidget->isLoadingFinished) return; // chordCurveThread->wait();
    if (ui->sheetFaceAndEdgeDockWidget != NULL && !ui->sheetFaceAndEdgeDockWidget->isLoadingFinished) return; // sheetFaceAndEdgeThread->wait();
    if (ui->sheetDualDockWidget != NULL && !ui->sheetDualDockWidget->isLoadingFinished) return; // sheetDualThread->wait();
    if (ui->quadDualDockWidget != NULL && !ui->quadDualDockWidget->isLoadingFinished) return; // quadDualThread->wait();
    if (ui->faceSegmentDockWidget != NULL && !ui->faceSegmentDockWidget->isLoadingFinished) return; // faceSegmentThread->wait();
    if (ui->singularFacesDockWidget != NULL && !ui->singularFacesDockWidget->isLoadingFinished) return; // singularFacesThread->wait();
    if (ui->sliceDockWidget != NULL && !ui->sliceDockWidget->isLoadingFinished) return; // sliceThread->wait();
//    if (ui->componentDockWidget != NULL && componentThread != NULL && componentThread->isRunning())  return; // componentThread->wait();
//    if (ui->chordDockWidget != NULL && chordThread != NULL && chordThread->isRunning()) return; // chordThread->wait();
//    if (ui->sheetDockWidget != NULL && sheetThread != NULL && sheetThread->isRunning()) return; // sheetThread->wait();
//    if (ui->chordFaceAndEdgeDockWidget != NULL && chordFaceAndEdgeThread != NULL && chordFaceAndEdgeThread->isRunning()) return; // chordFaceAndEdgeThread->wait();
//    if (ui->chordCurveDockWidget != NULL && chordCurveThread != NULL && chordCurveThread->isRunning()) return; // chordCurveThread->wait();
//    if (ui->sheetFaceAndEdgeDockWidget != NULL && sheetFaceAndEdgeThread != NULL && sheetFaceAndEdgeThread->isRunning()) return; // sheetFaceAndEdgeThread->wait();
//    if (ui->sheetDualDockWidget != NULL && sheetDualThread != NULL && sheetDualThread->isRunning()) return; // sheetDualThread->wait();
//    if (ui->quadDualDockWidget != NULL && quadDualThread != NULL && quadDualThread->isRunning()) return; // quadDualThread->wait();
//    if (ui->faceSegmentDockWidget != NULL && faceSegmentThread != NULL && faceSegmentThread->isRunning()) return; // faceSegmentThread->wait();
//    if (ui->singularFacesDockWidget != NULL && singularFacesThread != NULL && singularFacesThread->isRunning()) return; // singularFacesThread->wait();
//    if (ui->sliceDockWidget != NULL && sliceThread != NULL && sliceThread->isRunning()) return; // sliceThread->wait();

    QVariant variant = item->data(Qt::UserRole);
    std::string str = item->text().toStdString();
    for (int i = 0; i < ui->modelBrowser->filenames.size(); ++i) {
        std::string& filename = ui->modelBrowser->filenames[i];
        size_t pos = filename.find_last_of('/');
        std::string modelname = filename.substr(pos + 1);
        if (str == modelname) {
            strFilename = QString::fromStdString(filename) + QString("/") + QString::fromStdString(str) + QString(".vtk");
            strFolderPath = filename;
            m_renderer->GetActors()->RemoveAllItems();
            Update();
            surfaceAction->setEnabled(true);
            surfaceWithEdgeAction->setEnabled(true);
            wireFrameAction->setEnabled(true);
            volumeAction->setEnabled(true);

            opacitySlider->setEnabled(true);
            opacitySpinBox->setEnabled(true);
            sheetListAction->setEnabled(true);
            sheetListAction->setChecked(false);
            componentListAction->setEnabled(true);
            componentListAction->setChecked(false);
            opacityBaseComplexSlider->setDisabled(true);
            opacityBaseComplexSpinBox->setDisabled(true);
            opacitySingularitySlider->setDisabled(true);
            opacitySingularitySpinBox->setDisabled(true);
            on_sheetListAction_clicked();
            break;
        }
    }
//    int data = variant.toInt();
//    QString text = QString("%1:%2 is clicked.").arg(item->text()).arg(data);
//    qDebug() << text;
//    std::string str = item->text().toStdString();
//    qDebug() << QString::fromStdString(str);
}

void MainWindow::TimerHandlerFunction()
{
     if (ui->sheetDecompositionsDockWidget && ui->quadDualDockWidget && ui->quadDualDockWidget->isLoadingFinished)
         ui->sheetDecompositionsDockWidget->widget->setEnabled(true);
}
