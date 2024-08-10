#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QString>
#include <QDateTime>
#include <QtSql/QSqlDatabase>
#include <QVector>
#include <memory>

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
    // 获取单例对象
    static HistoryManager* getInstance();

    // 禁止拷贝构造和赋值操作
    HistoryManager(HistoryManager const&) = delete;
    void operator=(HistoryManager const&) = delete;

    // 添加一条新的历史记录
    bool addRecord(const QString& expression, const QString& result, const QDateTime& timestamp);

    // 获取所有历史记录
    QVector<HistoryRecord> getRecords() const;

    // 根据ID删除一条历史记录
    bool removeRecord(int id);

    // 清空所有历史记录
    bool clearAllRecords();

private:
    // 私有构造函数和析构函数
    HistoryManager(); // 构造函数
    ~HistoryManager(); // 析构函数

    // 初始化数据库连接
    void initDatabase();

    // 数据库指针
    std::unique_ptr<QSqlDatabase> mDatabase;

    // 自定义静态变量的删除器
    // 目的是为了使unique_ptr能够访问HistoryManager的析构函数
    class InstanceDeleter {
    public:
        void operator()(HistoryManager* instance) const {
            delete instance;
        }
    };

    // 单例实例
    static std::unique_ptr<HistoryManager, InstanceDeleter> mInstance;

};

#endif // HISTORYMANAGER_H
