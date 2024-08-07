#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QString>
#include <QDateTime>
#include <QScopedPointer>
#include <QtSql/QSqlDatabase>

// 历史记录结构体，包含记录的ID、表达式、结果和时间戳
struct HistoryRecord {
    int id; // 记录的唯一标识
    QString expression; // 表达式
    QString result; // 计算结果
    QDateTime timestamp; // 记录的时间戳
};

// 历史记录管理类，负责管理历史记录的添加、查询、删除和清空
class HistoryManager {
public:
    HistoryManager(); // 构造函数
    ~HistoryManager(); // 析构函数

    // 添加一条新的历史记录
    bool addRecord(const QString& expression, const QString& result, const QDateTime& timestamp);
    // 获取所有历史记录
    QVector<HistoryRecord> getRecords() const;
    // 根据ID删除一条历史记录
    bool removeRecord(int id);
    // 清空所有历史记录
    bool clearAllRecords();

private:
    // 初始化数据库连接
    void initDatabase();
    // 指向数据库的智能指针
    QScopedPointer<QSqlDatabase> mDatabase;
};

#endif // HISTORYMANAGER_H
