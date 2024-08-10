#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include "calculator.h"
#include "expressionvalidator.h"
#include "customlistwidget.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QList>
#include <QListWidget>
#include <QScopedPointer>
#include <QResizeEvent>

// 计算器主窗口类，继承自QWidget
class CalculatorWindow : public QWidget
{
    Q_OBJECT

public:
    // 构造函数，接受一个父部件指针
    CalculatorWindow(QWidget *parent = nullptr);
    // 析构函数
    ~CalculatorWindow();

protected:
    // 重写窗口大小调整事件处理函数
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // 处理按钮点击事件
    void handleButtonClick(const QString& text);
    // 切换历史记录侧边栏的显示状态
    void toggleHistorySidebar();
    // 处理显示文本变化事件
    void onTextChanged(const QString& text);
    // 处理列表项左键双击事件
    void onItemDoubleClickedLeft(QListWidgetItem* item);
    // 处理列表项右键点击事件
    void onItemClickedRight(QListWidgetItem* item);

private:
    // 初始化UI组件
    void initUi();
    // 设置信号和槽的连接
    void setupConnections();
    // 加载样式表
    void loadStyleSheets();
    // 向显示区域添加文本
    void addToDisplay(const QString& str);
    // 切换表达式的符号
    QString toggleExpressionSign(const QString& expression) const;

private:
    QPushButton* mHistoryButton; // 历史记录按钮
    QLineEdit* mDisplay; // 显示区域
    QList<QPushButton*> mButtons; // 按钮列表
    QWidget* mHistorySidebar; // 历史记录侧边栏
    CustomListWidget* mHistoryList; // 自定义列表部件，用于显示历史记录

    QList<QString> mStyleSheetFilePaths; // 样式表文件路径列表
    QScopedPointer<Calculator> mCalculator; // 计算器对象
    QScopedPointer<ExpressionValidator> mValidator; // 表达式验证器对象
};
#endif // CALCULATORWINDOW_H
