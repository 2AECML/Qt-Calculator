#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QMouseEvent>

// 自定义列表部件类，继承自QListWidget，用于处理鼠标事件
class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    // 构造函数，接受一个父部件指针
    CustomListWidget(QWidget* parent = nullptr);

protected:
    // 重写鼠标按下事件处理函数
    void mousePressEvent(QMouseEvent* event) override;
    // 重写鼠标双击事件处理函数
    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:
    // 当左键点击列表项时发出的信号
    void itemClickedLeft(QListWidgetItem* item);
    // 当右键点击列表项时发出的信号
    void itemClickedRight(QListWidgetItem* item);
    // 当左键双击列表项时发出的信号
    void itemDoubleClickedLeft(QListWidgetItem* item);
    // 当右键双击列表项时发出的信号
    void itemDoubleClickedRight(QListWidgetItem* item);
};

#endif // CUSTOMLISTWIDGET_H
