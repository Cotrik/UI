#ifndef MODELBROWSER_H
#define MODELBROWSER_H

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
//#include <QtGui/QAction>
//#include <QtGui/QApplication>
//#include <QtGui/QButtonGroup>
//#include <QtGui/QComboBox>
//#include <QtGui/QDockWidget>
//#include <QtGui/QDoubleSpinBox>
//#include <QtGui/QHeaderView>
//#include <QtGui/QLabel>
//#include <QtGui/QListWidget>
//#include <QtGui/QMainWindow>
//#include <QtGui/QMenuBar>
//#include <QtGui/QSlider>
//#include <QtGui/QStatusBar>
//#include <QtGui/QToolBar>
//#include <QtGui/QTreeWidget>

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

class ModelBrowser : public QDockWidget
{
    Q_OBJECT
public:
    explicit ModelBrowser(QMainWindow* &MainWindow);
public:
    void loadModels(const char* path);

signals:

public slots:
    virtual void resizeEvent(QResizeEvent *event);
public:
    QWidget *widget;
    QLabel *label;
    QListWidget *listWidget;
    std::vector<std::string> filenames;
};

#endif // MODELBROWSER_H
