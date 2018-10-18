#include "circleWidget.h"
#include <QPainter>

CircleWidget::CircleWidget(int x, int y, double factor, Qt::GlobalColor color, QWidget *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // factor = 1;
    _diameter = factor * 20;
    _opacity = factor * 1;
    _color = color;
    _x = x;
    _y = y;
    this->setGeometry(QRect(_x, _y, 25, 25));
}

void CircleWidget::paintEvent(QPaintEvent* e) {
    e->accept();
    QPainter p(this);
    p.setPen(_color);
    p.setOpacity(_opacity);
    if (_fill) p.setBrush(_color);
    p.drawEllipse(12.5 - (_diameter/2), 12.5 - (_diameter/2), _diameter, _diameter);
}

void CircleWidget::mousePressEvent(QMouseEvent* e) {
    _fill = !_fill;
    update();
    e->accept();
}
