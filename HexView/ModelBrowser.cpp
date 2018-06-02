#include "ModelBrowser.h"
#include <QDirIterator>

ModelBrowser::ModelBrowser(QMainWindow* &MainWindow)
    : QDockWidget(QString("Models"), MainWindow)
{
    widget = new QWidget;
    listWidget = new QListWidget(widget);
    listWidget->setObjectName(QString("Models") + QString::fromUtf8("ListWidget"));
    listWidget->setGeometry(QRect(5, 0, 220, 450));
    listWidget->setResizeMode(QListView::Adjust);
    QWidget* dockWidgetContents = new QWidget();
    dockWidgetContents->setObjectName(QString("Models") + QString::fromUtf8("DockWidgetContents"));
    this->setWidget(widget);
    MainWindow->addDockWidget(Qt::LeftDockWidgetArea, this);
}

void ModelBrowser::resizeEvent(QResizeEvent *event)
{
//    QDockWidget:resizeEvent(event);
    const QSize &newsize = event->size();
    listWidget->setMinimumWidth(newsize.width() - 15);
    listWidget->setMinimumHeight(newsize.height() > 40 ? newsize.height() - 40 : 10);
    listWidget->setMaximumWidth(newsize.width());
    listWidget->setMaximumHeight(newsize.height() > 50 ? newsize.height() - 50 : 20);
}

void ModelBrowser::loadModels(const char* path) {
    listWidget->clear();
    filenames.clear();
    QDirIterator directories(path, QDir::Dirs | /*QDir::NoSymLinks |*/ QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags/*QDirIterator::Subdirectories*/);
    while (directories.hasNext()) {
        directories.next();
        std::string filename = directories.filePath().toStdString();
        filenames.push_back(filename);
        size_t pos = filename.find_last_of('/');
        std::string modelname = filename.substr(pos + 1);
        QListWidgetItem* item = new QListWidgetItem(QIcon("../icon/model.png"), QString::fromStdString(modelname));
        //item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        //item->setCheckState(Qt::Unchecked); // AND initialize check state
        listWidget->addItem(item);
        //listWidget->setItemWidget(it, new QRadioButton(tr("Item 2")));
    }
    listWidget->sortItems(Qt::AscendingOrder);
}
