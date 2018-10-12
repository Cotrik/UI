#ifndef CIRCLEWIDGET_H
#define CIRCLEWIDGET_H

#include <QWidget>
#include <QMouseEvent>

class CircleWidget : public QWidget {
    Q_OBJECT
    public: 
    
        CircleWidget(int x, int y, double factor, Qt::GlobalColor color, QWidget *parent = 0);
        virtual void paintEvent(QPaintEvent* e);
        virtual void mousePressEvent(QMouseEvent* e);

    private:
        bool _fill = true;
        Qt::GlobalColor _color;
        double _x, _y, _diameter, _opacity;
};

#endif // CIRCLEWIDGET_H