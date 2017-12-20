/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>
#include <QXmlStreamReader>
#include <QDebug>
#include <QProgressBar>
#include "QVTKWidget.h"
#include "HexDockWidget.h"
#include "ModelBrowser.h"
#include "SheetDecompositionsDockWidget.h"

QT_BEGIN_NAMESPACE
struct HexDockWidgetInfo {
    QString name;
    Qt::DockWidgetArea dockArea = Qt::LeftDockWidgetArea;
    bool isPolyData = false;
    QString prefix;
    QString iconName;
};

class Ui_MainWindow
{
public:
    vtkRenderer* m_renderer;
    QWidget *centralwidget;
    QVTKWidget *qvtkWidget;
    QTreeWidget *sheetTreeWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QXmlStreamReader* config;

    HexDockWidget* componentDockWidget = NULL;
    HexDockWidget* chordDockWidget = NULL;
    HexDockWidget* sheetDockWidget = NULL;
    HexDockWidget* sheetFaceAndEdgeDockWidget = NULL;
    HexDockWidget* sheetDualDockWidget = NULL;
    HexDockWidget* chordFaceAndEdgeDockWidget = NULL;
    HexDockWidget* chordCurveDockWidget = NULL;
    HexDockWidget* faceSegmentDockWidget = NULL;
    HexDockWidget* singularFacesDockWidget = NULL;
    HexDockWidget* sliceDockWidget = NULL;
    //HexDockWidget* componentFaceAndEdgeDockWidget;
    std::vector<HexDockWidget*> docWidgets;
    std::vector<HexDockWidgetInfo> docWidgetInfo;
    ModelBrowser* modelBrowser;
    SheetDecompositionsDockWidget* sheetDecompositionsDockWidget = NULL;
    //QProgressBar* loadingBar;
//    QComboBox* sheetCellDataFieldComboBox;
    void setupUi(QMainWindow *MainWindow) {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1920, 1080);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../icon/hex.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setIconSize(QSize(22, 22));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        qvtkWidget = new QVTKWidget(centralwidget);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
        qvtkWidget->setGeometry(QRect(10, 0, 1460, 1000));
        MainWindow->setCentralWidget(centralwidget);

        m_renderer = vtkRenderer::New();

        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1920, 20));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        modelBrowser = new ModelBrowser(MainWindow);
        readConfigXml();
        for (auto& info : docWidgetInfo) {
            if (info.name == "Component")
        componentDockWidget = new HexDockWidget(MainWindow, QString("Component"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer);
            else if (info.name == "Chord")
        chordDockWidget = new HexDockWidget(MainWindow, QString("Chord"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer);
            else if (info.name == "Sheet")
        sheetDockWidget = new HexDockWidget(MainWindow, QString("Sheet"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer);
        //componentFaceAndEdgeDockWidget = new HexDockWidget(MainWindow, QString("ComponentFaceAndEdge"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "ChordFaceAndEdge")
        chordFaceAndEdgeDockWidget = new HexDockWidget(MainWindow, QString("ChordFaceAndEdge"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "ChordCurve")
        chordCurveDockWidget = new HexDockWidget(MainWindow, QString("ChordCurve"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "SheetFaceAndEdge")
        sheetFaceAndEdgeDockWidget = new HexDockWidget(MainWindow, QString("SheetFaceAndEdge"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "SheetDual")
        sheetDualDockWidget = new HexDockWidget(MainWindow, QString("SheetDual"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "FaceSegment")
        faceSegmentDockWidget = new HexDockWidget(MainWindow, QString("FaceSegment"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "SingularFaces")
        singularFacesDockWidget = new HexDockWidget(MainWindow, QString("SingularFaces"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
            else if (info.name == "Slice")
        sliceDockWidget = new HexDockWidget(MainWindow, QString("Slice"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
        }
        sheetDecompositionsDockWidget = new SheetDecompositionsDockWidget(MainWindow, QString("Sheet Decompositions"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer,
                                                                          false, &sheetDockWidget->m_vtkActors);
//        sheetCellDataFieldComboBox = new QComboBox(sheetWidget);
//        sheetCellDataFieldComboBox->setObjectName(QString::fromUtf8("sheetCellDataFieldComboBox"));
//        sheetCellDataFieldComboBox->setGeometry(QRect(0, 0, 200, 27));

//        sheetTreeWidget = new QTreeWidget(sheetWidget);
//        QTreeWidgetItem * sheetTreewidgetItem = new QTreeWidgetItem();
//        sheetTreewidgetItem->setText(0, QString::fromUtf8("sheetTreewidgetItem"));
//        sheetTreeWidget->setHeaderItem(sheetTreewidgetItem);
//        sheetTreeWidget->setObjectName(QString::fromUtf8("sheetTreeWidget"));
//        sheetTreeWidget->setGeometry(QRect(5, 550, 200, 450));
        // -----------------------------------------------------------
        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow) {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    }

    void readConfigXml() {
        config = new QXmlStreamReader;
        QFile xmlFile("../config.xml");
        xmlFile.open(QIODevice::ReadOnly);
        config->setDevice(&xmlFile);
        if (config->readNextStartElement() && config->name() == "HexDockWidgets")
            processHexDockWidgets();

        if (config->tokenType() == QXmlStreamReader::Invalid) config->readNext();
        if (config->hasError()) {
            config->raiseError();
            //qDebug() << errorString();
            qDebug() << "Error in readConfigXml";
        }
    }
    void processHexDockWidgets() {
        if (!config->isStartElement() || config->name() != "HexDockWidgets")
            return;
        while (config->readNextStartElement()) {
            if (config->name() == "HexDockWidget")
                processHexDockWidget();
            else
                config->skipCurrentElement();
        }
    }
    void processHexDockWidget() {
        if (!config->isStartElement() || config->name() != "HexDockWidget")
            return;
        HexDockWidgetInfo info;
        while (config->readNextStartElement()) {
            if (config->name() == "Name")
                info.name = readNextText();
            else if (config->name() == "DockWidgetArea")
                info.dockArea = readNextText() == "left" ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea;
            else if (config->name() == "IsPolyData")
                info.isPolyData = readNextText() == "true" ? true : false;
            else if (config->name() == "Prefix")
                info.prefix = readNextText();
            else if (config->name() == "Icon")
                info.iconName = readNextText();
            config->skipCurrentElement();
        }
        docWidgetInfo.push_back(info);
    }

    QString readNextText() {
    #ifndef USE_READ_ELEMENT_TEXT
        config->readNext();
        return config->text().toString();
    #else
        return config->readElementText();
    #endif
    }
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
