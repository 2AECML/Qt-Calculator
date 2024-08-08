#include "historymanager.h"

#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

// 初始化静态成员变量
HistoryManager HistoryManager::mInstance;
QSqlDatabase* HistoryManager::mDatabase = nullptr;
bool HistoryManager::mIsDatabaseInitialized = false;

HistoryManager& HistoryManager::getInstance() {
    if (!mIsDatabaseInitialized) {
        initDatabase();
        mIsDatabaseInitialized = true;
    }
    return mInstance;
}

// 添加记录
bool HistoryManager::addRecord(const QString& expression, const QString& result, const QDateTime& timestamp)
{
    // 准备 SQL 插入语句
    QSqlQuery query;
    query.prepare("INSERT INTO history (expression, result, timestamp) VALUES (:expression, :result, :timestamp)");

    // 绑定参数
    query.bindValue(":expression", expression);
    query.bindValue(":result", result);
    query.bindValue(":timestamp", timestamp.toString(Qt::ISODate));

    // 执行查询
    return query.exec();
}

// 获取记录
QVector<HistoryRecord> HistoryManager::getRecords() const {
    // 定义记录列表
    QVector<HistoryRecord> records;

    // 准备 SQL 查询语句
    QSqlQuery query("SELECT id, expression, result, timestamp FROM history ORDER BY timestamp DESC");

    // 遍历查询结果
    while (query.next()) {
        HistoryRecord record;
        record.id = query.value("id").toInt();
        record.expression = query.value("expression").toString();
        record.result = query.value("result").toString();
        record.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);
        records.append(record);
    }

    // 返回记录列表
    return records;
}

// 删除记录
bool HistoryManager::removeRecord(int id) {
    // 准备 SQL 删除语句
    QSqlQuery query;
    query.prepare("DELETE FROM history WHERE id = :id");
    query.bindValue(":id", id);  // 绑定参数

    // 执行查询
    return query.exec();  
}

// 清空记录
bool HistoryManager::clearAllRecords()
{
    // 准备并执行 SQL 删除语句
    QSqlQuery query;
    return query.exec("DELETE FROM history");
}

HistoryManager::HistoryManager() {

}

// 析构函数，关闭数据库连接
HistoryManager::~HistoryManager()
{
    if (mDatabase) {
        if (mDatabase->isOpen()) {
            mDatabase->close();
        }
        delete mDatabase;
        mDatabase = nullptr;
    }

}

// 初始化数据库
void HistoryManager::initDatabase()
{
    qDebug() << QSqlDatabase::drivers();

    // 创建数据库连接
    mDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    mDatabase->setDatabaseName("calculator_history.db");

    // 打开数据库连接
    if (!mDatabase->open()) {
        qDebug() << "Error: connection with database failed: " << mDatabase->lastError();
    }
    else {
        // 创建表
        qDebug() << "Database: connection ok";
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS history ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "expression TEXT, "
                   "result TEXT, "
                   "timestamp DATETIME)");
    }

}
