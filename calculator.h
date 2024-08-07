#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "expressionvalidator.h"
#include <QString>
#include <QVector>
#include <QChar>
#include <QScopedPointer>

// 计算器类，用于处理数学表达式的计算
class Calculator
{
public:
    Calculator(); // 构造函数
    ~Calculator(); // 析构函数

    // 获取表达式的字符串结果，默认精度为6位小数
    QString getStringResult(const QString& expression, int precision = 6) const;
    // 获取表达式的双精度结果
    long double getDoubleResult(const QString& expression) const;

private:
    // 将字符串表达式分割为字符串向量
    QVector<QString> stringToVector(const QString& expression) const;
    // 补全表达式，例如处理负数的情况
    QVector<QString> completeExpression(const QVector<QString>& tokens) const;
    // 将中缀表达式转换为后缀表达式（逆波兰表示法）
    QVector<QString> infixToPostfix(const QVector<QString>& tokens) const;
    // 计算后缀表达式的结果
    long double evaluatePostfix(const QVector<QString>& postfix) const;
    // 获取操作符的优先级
    int precedence(const QChar& ch) const;
    // 计算阶乘
    long double factorial(const int val) const;

private:
    // 表达式验证器对象
    QScopedPointer<ExpressionValidator> mValidator;
};

#endif // CALCULATOR_H
