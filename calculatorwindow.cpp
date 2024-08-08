#include "calculatorwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>
#include <QFile>
#include <QStack>
#include <QStyle>
#include <QFont>
#include <QPropertyAnimation>
#include <QMenu>
#include <QDebug>

// 构造函数，初始化UI和连接信号与槽
CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QWidget(parent),
    mDisplay(nullptr),
    mCalculator(new Calculator()),
    mValidator(new ExpressionValidator()){
    initUi(); // 初始化UI
    setupConnections(); // 设置信号与槽的连接
}

// 析构函数
CalculatorWindow::~CalculatorWindow() {

}

// 重写resizeEvent，调整窗口大小时调整侧边栏的宽度
void CalculatorWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    int sidebarWidth = this->width() * 0.74;
    mHistorySidebar->setFixedWidth(sidebarWidth);

    if (mHistorySidebar->isVisible()) {
        mHistorySidebar->setGeometry(0, 0, sidebarWidth, height());
    }
}

// 处理按钮点击事件
void CalculatorWindow::handleButtonClick(const QString &text) {
    QString expression = mDisplay->text();

    if (text == "AC") {
        mDisplay->clear(); // 清空显示
    }
    else if (text == "DEL") {
        if (!expression.isEmpty()) {
            expression.chop(1); // 删除最后一个字符
            mDisplay->setText(expression);
        }
    }
    else if (text == "+/-") {
        mDisplay->clear();
        addToDisplay(toggleExpressionSign(expression)); // 切换表达式的符号
    }
    else if (text == ".") {
        if (mValidator->isPointValid(expression)) {
            addToDisplay(text); // 添加小数点
        }
    }
    else if (text == "+" || text == "-" || text == "×" || text == "÷" || text == "x^y") {
        if (mValidator->isOperatorValid(expression)) {
            if (text == "x^y") {
                addToDisplay("^"); // 添加指数符号
            }
            else {
                addToDisplay(text); // 添加运算符
            }
        }
    }
    else if (text == "x!") {
        addToDisplay("!"); // 添加阶乘符号
    }
    else if (text == "=") {
        QString result = mCalculator->getStringResult(expression); // 计算结果
        if (result != "ERROR") {
            HistoryManager& historyManager = HistoryManager::getInstance();
            historyManager.addRecord(expression, result, QDateTime::currentDateTime()); // 添加历史记录
        }
        mDisplay->setText(result); // 显示结果
    }
    else {
        addToDisplay(text); // 添加其他字符
    }
}

// 切换历史记录侧边栏的显示状态
void CalculatorWindow::toggleHistorySidebar() {
    int sidebarWidth = mHistorySidebar->width();

    if (!mHistorySidebar->isVisible()) {    // 若侧边栏不可见，加载历史记录并显示
        mHistoryButton->setChecked(true);
        HistoryManager& historyManager = HistoryManager::getInstance();
        QVector<HistoryRecord> records = historyManager.getRecords();
        mHistoryList->clear();
        for (const auto& record : records) {
            // 创建历史记录项并设置数据
            QListWidgetItem* item = new QListWidgetItem(
                "表达式: " + record.expression + "\n结果: " + record.result + "\n时间: " + record.timestamp.toString(Qt::ISODate),
                mHistoryList);

            // 自定义存储数据
            item->setData(Qt::UserRole, record.id); // 设置id
            item->setData(Qt::UserRole + 1, record.expression); // 设置表达式
            item->setData(Qt::UserRole + 2, record.result); // 设置结果
            item->setData(Qt::UserRole + 3, record.timestamp.toString(Qt::ISODate)); // 设置时间

            mHistoryList->addItem(item); // 添加到列表中
        }
        mHistorySidebar->setVisible(true);
        QPropertyAnimation* animation = new QPropertyAnimation(mHistorySidebar, "geometry");
        animation->setDuration(300);
        animation->setStartValue(QRect(-sidebarWidth, 0, sidebarWidth, height()));
        animation->setEndValue(QRect(0, 0, sidebarWidth, height()));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else {  // 若侧边栏可见，隐藏侧边栏
        mHistoryButton->setChecked(false);
        QPropertyAnimation* animation = new QPropertyAnimation(mHistorySidebar, "geometry");
        animation->setDuration(300);
        animation->setStartValue(QRect(0, 0, sidebarWidth, height()));
        animation->setEndValue(QRect(-sidebarWidth, 0, sidebarWidth, height()));
        connect(animation, &QPropertyAnimation::finished, this, [this]() {
            mHistorySidebar->setVisible(false);
        });
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

// 处理显示文本变化事件，当显示面板空间不足则缩小字体
void CalculatorWindow::onTextChanged(const QString &text) {
    QFont font = mDisplay->font();
    int length = text.length();
    int maxLength = mDisplay->maxLength();

    static QStack<QFont> originalFont;
    static QStack<int> originalMaxLength;

    if (length == maxLength && font.pointSize() > 15) {
        // 备份原本的字体与最大长度
        originalFont.push(font);
        originalMaxLength.push(maxLength);

        // 当字符数量达到最大长度且字体大小大于15时，缩小字体大小并增加最大长度
        font.setPointSize(font.pointSize() * 0.6); // 缩小字体大小
        mDisplay->setFont(font);
        mDisplay->setMaxLength(mDisplay->maxLength() * 1.8);
    }
    else if (!originalFont.empty() && !originalMaxLength.empty() && length < originalMaxLength.top()) {
        // 当字符数量减少且字体大小小于原始字体大小时，恢复原始字体大小和最大长度
        mDisplay->setFont(originalFont.pop());
        mDisplay->setMaxLength(originalMaxLength.pop());
    }
}

// 处理右键点击历史记录项事件
void CalculatorWindow::onItemClickedRight(QListWidgetItem *item) {
    if (item) {
        // 创建一个 QMenu 对象
        QMenu menu(this);

        // 创建 QAction 对象，用于删除操作
        QAction* deleteAction = new QAction("删除", this);
        QAction* deleteAllAction = new QAction("删除所有", this);

        // 将 QAction 添加到 QMenu 中
        menu.addAction(deleteAction);
        menu.addAction(deleteAllAction);

        // 连接 QAction 的触发信号到删除操作
        connect(deleteAction, &QAction::triggered, this, [this, item]() {
            // 从 mHistoryList 中移除该项
            mHistoryList->removeItemWidget(item);
            // 从 mHistoryManager 中删除该记录
            HistoryManager& historyManager = HistoryManager::getInstance();
            historyManager.removeRecord(item->data(Qt::UserRole).toInt());
            // 手动删除项
            delete item;
        });

        connect(deleteAllAction, &QAction::triggered, this, [this, item]() {
            // 遍历 mHistoryList 中的所有项并删除
            while (mHistoryList->count() > 0) {
                QListWidgetItem* item = mHistoryList->takeItem(0);
                // 手动删除项
                delete item;
            }
            // 从 mHistoryManager 中删除所有记录
            HistoryManager& historyManager = HistoryManager::getInstance();
            historyManager.clearAllRecords();
        });

        // 显示 QMenu 并跟踪鼠标位置
        menu.exec(QCursor::pos());
    }
}

// 处理左键双击历史记录项事件
void CalculatorWindow::onItemDoubleClickedLeft(QListWidgetItem* item) {
    if (item) {
        QString result = item->data(Qt::UserRole + 2).toString(); // 获取结果
        addToDisplay(result);
        toggleHistorySidebar();
    }
}

// 初始化UI
void CalculatorWindow::initUi() {
    // 设置窗口标题和图标
    setWindowTitle("Calculator");
    setWindowIcon(QIcon(":/icon.svg"));

    // 设置窗口的初始大小、最小尺寸和最大尺寸
    resize(350, 500);
    setMinimumSize(300, 420);
    setMaximumSize(500, 600);

    // 创建网格布局管理器
    QGridLayout* gridLayout = new QGridLayout(this);

    // 创建历史记录按钮，并设置其图标和样式
    mHistoryButton = new QPushButton();
    mHistoryButton->setIcon(QIcon(":/history.svg"));
    mHistoryButton->setProperty("class", "historyBtn");
    mHistoryButton->setCheckable(true);
    gridLayout->addWidget(mHistoryButton, 0, 3);

    // 创建显示框，设置为只读，右对齐，并设置字体和最大长度
    mDisplay = new QLineEdit();
    mDisplay->setReadOnly(true);
    mDisplay->setAlignment(Qt::AlignRight);
    mDisplay->setProperty("class", "displayLine");
    QFont font = mDisplay->font();
    font.setPointSize(32);
    mDisplay->setFont(font);
    mDisplay->setMaxLength(9);
    gridLayout->addWidget(mDisplay, 1, 0, 1, 4);

    // 定义按钮的布局
    QString buttons[6][4] = {
                             {"(",   ")",    "x!",   "x^y"},
                             {"AC",  "DEL",  "+/-",  "÷"},
                             {"7",   "8",    "9",    "×"},
                             {"4",   "5",    "6",    "-"},
                             {"1",   "2",    "3",    "+"},
                             {"%",   "0",    ".",    "="},
                             };

    // 遍历按钮布局，创建并添加按钮到网格布局
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            QPushButton* btn = new QPushButton(buttons[i][j]);
            if (mValidator->isNumeric(buttons[i][j])) {
                btn->setProperty("class", "numericBtn calculateBtn");
            }
            else {
                if (buttons[i][j] == "=") {
                    btn->setProperty("class", "equalBtn calculateBtn");
                }
                else if (buttons[i][j] == "+" || buttons[i][j] == "-" || buttons[i][j] == "×" || buttons[i][j] == "÷") {
                    btn->setProperty("class", "mathOperatorsBtn calculateBtn");
                }
                else if (buttons[i][j] == "AC" || buttons[i][j] == "DEL" || buttons[i][j] == "+/-") {
                    btn->setProperty("class", "FunctionsBtn calculateBtn");
                }
                else if (buttons[i][j] == "%" || buttons[i][j] == ".") {
                    btn->setProperty("class", "specialSymbolsBtn calculateBtn");
                }
                else {
                    btn->setProperty("class", "otherBtn calculateBtn");
                }
            }
            gridLayout->addWidget(btn, i + 2, j);
            mButtons.append(btn);

            QFont font = btn->font();
            font.setPointSize(16);
            btn->setFont(font);
        }
    }

    // 初始化侧边栏，设置其样式并隐藏
    mHistorySidebar = new QWidget(this);
    mHistorySidebar->setProperty("class", "historySidebar");
    mHistorySidebar->setVisible(false);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(mHistorySidebar);

    // 初始化侧边栏中的列表，设置其样式
    mHistoryList = new CustomListWidget(mHistorySidebar);
    mHistoryList->setProperty("class", "historyList");
    sidebarLayout->addWidget(mHistoryList);

    // 添加样式表文件路径并加载样式表
    mStyleSheetFilePaths.append(":/style.qss");
    loadStyleSheets();
}

// 设置信号与槽的连接
void CalculatorWindow::setupConnections() {

    connect(mDisplay, &QLineEdit::textChanged, this, [this]() {
        onTextChanged(mDisplay->text());
    });

    for (QPushButton* btn : mButtons) {
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            handleButtonClick(btn->text());
        });
    }

    connect(mHistoryButton, &QPushButton::clicked, this, [this]() {
        toggleHistorySidebar();
    });

    connect(mHistoryList, &CustomListWidget::itemDoubleClickedLeft, this, [this](QListWidgetItem* item) {
        onItemDoubleClickedLeft(item);
    });

    connect(mHistoryList, &CustomListWidget::itemClickedRight, this, [this](QListWidgetItem* item) {
        onItemClickedRight(item);
    });
}

// 加载样式表
void CalculatorWindow::loadStyleSheets() {
    static QString allStyleSheets;

    for (const QString& filePath : mStyleSheetFilePaths) {
        QFile file(filePath);
        if (!file.exists()) {
            qDebug() << "File does not exist:" << file.fileName();
        }
        else if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Failed to open file:" << file.errorString();
        }
        else {
            QTextStream stream(&file);
            QString styleSheet = stream.readAll();
            allStyleSheets.append(styleSheet);
        }
    }

    setStyleSheet(allStyleSheets);
}

// 向显示面板中添加字符
void CalculatorWindow::addToDisplay(const QString& str) {
    for (const QChar& ch : str) {
        QString originalText = mDisplay->text();
        mDisplay->setText(originalText + ch);
    }
}

// 切换表达式的符号
QString CalculatorWindow::toggleExpressionSign(const QString &expression) const {
    QString result;
    if (!expression.isEmpty()) {
        result = expression;
        if (result.startsWith("-(") && result.endsWith(")")) {
            // 如果表达式以负号和括号开头和结尾，去掉负号和括号
            result.remove(0, 2); // 去掉 "-("
            result.chop(1); // 去掉 ")"
        } else {
            // 如果表达式不以负号和括号开头和结尾，添加负号并加上括号
            result.prepend("-(");
            result.append(")");
        }
    }
    return result;
}
