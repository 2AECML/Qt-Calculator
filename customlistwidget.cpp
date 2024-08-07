#include "customlistwidget.h"

CustomListWidget::CustomListWidget(QWidget* parent) : QListWidget(parent) {}

// 重写鼠标点击事件
void CustomListWidget::mousePressEvent(QMouseEvent* event) {
    // 执行默认左键双击事件
    QListWidget::mousePressEvent(event);

    // 判断点击的是左键还是右键
    if (event->button() == Qt::LeftButton) {
        emit itemClickedLeft(itemAt(event->pos()));
    }
    else if (event->button() == Qt::RightButton) {
        emit itemClickedRight(itemAt(event->pos()));
    }
}

// 重写鼠标双击事件
void CustomListWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    // 执行默认左键双击事件
    QListWidget::mouseDoubleClickEvent(event);

    // 判断双击的是左键还是右键
    if (event->button() == Qt::LeftButton) {
        emit itemDoubleClickedLeft(itemAt(event->pos()));
    }
    else if (event->button() == Qt::RightButton) {
        emit itemDoubleClickedRight(itemAt(event->pos()));
    }
}
