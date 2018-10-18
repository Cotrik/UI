#include "matrixWidgetComponent.h"
#include <QPainter>

matrixWidgetComponent::matrixWidgetComponent(int row, int col, double ratio, QString type, QWidget *parent, Qt::GlobalColor color, QString text) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _row = row;
    _col = col;
    _size = max_size * ratio;
    _opacity = _opacity * ratio;
    _color = color;
    _type = type;
    _text = text;
    double x = (bounding_rect_size * _row) + offset;
    double y = (bounding_rect_size * _col) + offset;
    this->setGeometry(QRect(x, y, bounding_rect_size, bounding_rect_size));
}

void matrixWidgetComponent::paintEvent(QPaintEvent* e) {
    e->accept();
    QPainter p(this);
    p.setPen(_color);
    p.setOpacity(_opacity);
    double x = (bounding_rect_size - _size) / 2;
    double y = x;
    if (_type != "Label") {
        if (_fill) p.setBrush(_color);
        if (_type == "Circle") {
            p.drawEllipse(x, y, _size, _size);
        } else {
            p.drawRect(QRect(x, y, _size, _size));
        }
    } else {
        p.drawText(QRect(x, y, _size, _size), Qt::AlignCenter, _text);
    }
    
}

void matrixWidgetComponent::mousePressEvent(QMouseEvent* e) {
    _fill = !_fill;
    update();
    e->accept();
}
