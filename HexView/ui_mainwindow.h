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
#include <QProgressBar>
#include "QVTKWidget.h"
#include "HexDockWidget.h"
#include "ModelBrowser.h"

QT_BEGIN_NAMESPACE

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

    HexDockWidget* componentDockWidget;
    HexDockWidget* chordDockWidget;
    HexDockWidget* sheetDockWidget;
    HexDockWidget* sheetFaceAndEdgeDockWidget;
    HexDockWidget* sheetDualDockWidget;
    HexDockWidget* chordFaceAndEdgeDockWidget;
    HexDockWidget* chordCurveDockWidget;
    //HexDockWidget* componentFaceAndEdgeDockWidget;
    ModelBrowser* modelBrowser;
    //QProgressBar* loadingBar;
//    QComboBox* sheetCellDataFieldComboBox;
    void setupUi(QMainWindow *MainWindow)
    {
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

        componentDockWidget = new HexDockWidget(MainWindow, QString("Component"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer);
        chordDockWidget = new HexDockWidget(MainWindow, QString("Chord"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer);
        sheetDockWidget = new HexDockWidget(MainWindow, QString("Sheet"), Qt::LeftDockWidgetArea, qvtkWidget, m_renderer);
        //componentFaceAndEdgeDockWidget = new HexDockWidget(MainWindow, QString("ComponentFaceAndEdge"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
        chordFaceAndEdgeDockWidget = new HexDockWidget(MainWindow, QString("ChordFaceAndEdge"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
        chordCurveDockWidget = new HexDockWidget(MainWindow, QString("ChordCurve"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
        sheetFaceAndEdgeDockWidget = new HexDockWidget(MainWindow, QString("SheetFaceAndEdge"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);
        sheetDualDockWidget = new HexDockWidget(MainWindow, QString("SheetDual"), Qt::RightDockWidgetArea, qvtkWidget, m_renderer, true);

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

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
