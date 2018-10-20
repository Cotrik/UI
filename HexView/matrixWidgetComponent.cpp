#include "matrixWidgetComponent.h"
#include "SheetDecompositionsDockWidget.h"

#include <QPainter>

matrixWidgetComponent::matrixWidgetComponent(int row, int col, double ratio, QString type, QMainWindow *MainWindow, QWidget *parent, Qt::GlobalColor color, QString text) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWindow = MainWindow;
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
    SheetDecompositionsDockWidget *sheetDecompositionsDockWidget = mainWindow->findChild<SheetDecompositionsDockWidget *>("Sheet DecompositionsDockWidget");
    if (!sheetDecompositionsDockWidget) {return;}
    Qt::CheckState state = Qt::Checked;
    if (_fill) {
        state = Qt::Unchecked;
    }
    int decomposition_id = sheetDecompositionsDockWidget->currentDecompositionId;
    sheetDecompositionsDockWidget->treeWidget->topLevelItem(decomposition_id)->child(_row-1)->setCheckState(0, state);
    sheetDecompositionsDockWidget->on_Clicked(sheetDecompositionsDockWidget->treeWidget->topLevelItem(decomposition_id)->child(_row-1), 0);
    
    if (_row != _col) {
        sheetDecompositionsDockWidget->treeWidget->topLevelItem(decomposition_id)->child(_col-1)->setCheckState(0, state);
        sheetDecompositionsDockWidget->on_Clicked(sheetDecompositionsDockWidget->treeWidget->topLevelItem(decomposition_id)->child(_col-1), 0);
    }
    update();
    e->accept();
}
