#include "rectWidget.h"
#include <QPainter>

RectWidget::RectWidget(int x, int y, double factor, QWidget *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // factor = 1;
    _x = x;
    _y = y;
    _width = factor * 20;
    _height = factor * 20;
    _opacity = factor * 1;
    this->setGeometry(QRect(_x, _y, 25, 25));
}

void RectWidget::paintEvent(QPaintEvent* e) {
    e->accept();
    QPainter p(this);
    p.setPen(Qt::black);
    p.setOpacity(_opacity);
    if (_fill) p.setBrush(Qt::black);
    p.drawRect(QRect(2.5, 2.5, _width, _height));
}

void RectWidget::mousePressEvent(QMouseEvent* e) {
    _fill = !_fill;
    update();
    e->accept();
}
