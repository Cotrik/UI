#ifndef RECTWIDGET_H
#define RECTWIDGET_H

#include <QWidget>
#include <QMouseEvent>

class RectWidget : public QWidget {
    Q_OBJECT
    public: 
    
        RectWidget(int x, int y, double factor, QWidget *parent = 0);
        virtual void paintEvent(QPaintEvent* e);
        virtual void mousePressEvent(QMouseEvent* e);

    private:
        bool _fill = true;
        double _x, _y, _width, _height, _opacity;
};

#endif // RECTWIDGET_H