#ifndef EXPRESSIONVALIDATOR_H
#define EXPRESSIONVALIDATOR_H

#include <QVector>
#include <QString>

// 表达式验证器类，用于验证数学表达式的有效性
class ExpressionValidator
{
public:
    // 验证整个表达式是否有效
    bool isExpressionValid(const QVector<QString>& tokens) const;
    // 验证单个字符是否为操作符
    bool isOperator(const QChar& ch) const;
    // 验证字符串是否为操作符
    bool isOperator(const QString& str) const;
    // 验证字符串是否为数字
    bool isNumeric(const QString &str) const;
    // 验证单个token是否有效
    bool isTokenValid(const QString& token) const;
    // 验证表达式中的小数点是否有效
    bool isPointValid(const QString& expression) const;
    // 验证表达式中的操作符是否有效
    bool isOperatorValid(const QString& expression) const;

private:
    // 获取表达式中最后一个数字部分
    QString getLastNumberOfExpression(const QString& expression) const;
};

#endif // EXPRESSIONVALIDATOR_H
