#ifndef MATRIXWIDGETCOMPONENT_H
#define MATRIXWIDGETCOMPONENT_H

#include <QMainWindow>
#include <QWidget>
#include <QMouseEvent>

class matrixWidgetComponent : public QWidget {
    Q_OBJECT
    public: 
    
        matrixWidgetComponent(int row, int col, double ratio, QString type, QMainWindow *MainWindow, QWidget *parent = 0, Qt::GlobalColor color = Qt::black, QString text = NULL);
        virtual void paintEvent(QPaintEvent* e);
        virtual void mousePressEvent(QMouseEvent* e);

    private:
        QMainWindow *mainWindow;
        bool _fill = true;
        Qt::GlobalColor _color;
        QString _type, _text;
        double max_size = 15, offset = 5, bounding_rect_size = 25, _opacity = 0.6;
        double _size, _row, _col;
};

#endif // MATRIXWIDGETCOMPONENT_H