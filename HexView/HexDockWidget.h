#ifndef HEXDOCKWIDGET_H
#define HEXDOCKWIDGET_H

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
//#include <QtGui/QWidget>
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

class HexDockWidget : public QDockWidget {
	Q_OBJECT
public:
	HexDockWidget(QMainWindow* &MainWindow, QString name, Qt::DockWidgetArea dockWidgetArea,
		QVTKWidget* &qvtkWidget, vtkRenderer* &m_renderer, const bool isPolyData = false);

public:
	//void setupUi(QMainWindow *MainWindow);
	void setupDockWidget(QMainWindow* &MainWindow, QString name,
		QWidget* &sheetWidget,
		QLabel* &sheetLabel,
		QSlider* &sheetOpacitySlider,
		QDoubleSpinBox* &sheetOpacitySpinBox,
		QListWidget* &sheetListWidget,
		QComboBox* &sheetDisplayComboBox,
		QWidget* &sheetDockWidgetContents,
		Qt::DockWidgetArea dockWidgetArea = Qt::LeftDockWidgetArea);
public:
	void setIconFilename(QString filename);
	void setListItemFileNamePrefix(const std::string prefix);
	void setFolderPath(const std::string path);

	virtual void AddActor(const char* filename, const int id, std::vector<vtkSmartPointer<vtkActor> > &actors, bool isPolyData = false);
	virtual void loadListWidgetItems(const std::string path, const std::string prefix, QString iconFilename);
private:
	virtual void AddUnstructedGridActor(const char* filename, const int id, std::vector<vtkSmartPointer<vtkActor> > &actors);
	virtual void AddPolyDataActor(const char* filename, const int id, std::vector<vtkSmartPointer<vtkActor> > &actors);
	virtual std::vector<std::vector<int> > GetScalarFields(vtkSmartPointer<vtkUnstructuredGridReader> pReader, int numOfFields = 4);
	virtual std::vector<std::vector<int> > GetScalarFields(vtkSmartPointer<vtkPolyDataReader> pReader, int numOfFields = 1);
	public slots:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void on_Clicked(QListWidgetItem* item);
	virtual void on_OpacityChanged(int val);
	virtual void on_OpacityChanged(double val);
	virtual void on_DisplayCurrentIndexChanged(int index);
	virtual void on_CurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
signals:
	void finishLoading();

public:
	QWidget * widget;
	QLabel *label;
	QSlider *opacitySlider;
	QDoubleSpinBox *opacitySpinBox;
	QListWidget *listWidget;
	QComboBox *displayComboBox;
	QDockWidget *dockWidget;
	QWidget *dockWidgetContents;
	QProgressBar* loadingBar;
	bool isPolyData;
	std::unordered_map<int, int> listid_id;
	int displayCurrentIndex = 0; // Display Surface
	bool isQuadDual = false;
	bool isLoadingFinished = true;

public:
	QString iconFilename;
	std::string strListItemFileNamePrefix;
	std::string strFolderPath;
	size_t numOfListItems;
	std::vector<std::vector<std::vector<int > > > cellDataFields;
	QVTKWidget *qvtkWidget;
	vtkRenderer* m_renderer;
	std::vector<vtkSmartPointer<vtkActor> > m_vtkActors;
};

#endif // HEXDOCKWIDGET_H
