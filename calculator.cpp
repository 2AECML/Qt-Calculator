#include "calculator.h"
#include <QStack>
#include <QtMath>


Calculator::Calculator() : mValidator(new ExpressionValidator()){

}

Calculator::~Calculator() {

}

// 计算后缀表达式的值，返回字符串类型结果
QString Calculator::getStringResult(const QString& expression, int precision) const {
    if (expression.isEmpty()) {
        return QString();
    }
    QVector<QString> infix = stringToVector(expression);
    QVector<QString> completeInfix = completeExpression(infix);
    if (!mValidator->isExpressionValid(completeInfix)) {
        return QString("ERROR");
    }
    QVector<QString> postfix = infixToPostfix(completeInfix);  // 将中缀表达式转换为后缀表达式
    long double doubleResult = evaluatePostfix(postfix);   // 计算后缀表达式的值
    QString stringResult = QString::number(doubleResult, 'g', precision);
    return stringResult;  // 返回计算结果
}

// 计算后缀表达式的值，返回 double 类型结果
long double Calculator::getDoubleResult(const QString& expression) const {
    if (expression.isEmpty()) {
        return 0;
    }
    QVector<QString> infix = stringToVector(expression);
    QVector<QString> completeInfix = completeExpression(infix);
    if (!mValidator->isExpressionValid(completeInfix)) {
        return 0;
    }
    QVector<QString> postfix = infixToPostfix(infix);  // 将中缀表达式转换为后缀表达式
    long double doubleResult = evaluatePostfix(postfix);   // 计算后缀表达式的值
    return doubleResult;  // 返回计算结果
}

// 将字符串中的内容分割为 token 并返回 token 列表
QVector<QString> Calculator::stringToVector(const QString& expression) const {
    QVector<QString> tokens;
    QString number; // 用于存储当前读取的数字

    for (const QChar& ch : expression) {
        // 如果当前字符是数字或小数点
        if (ch.isDigit() || ch == '.') {
            number.append(ch); // 将字符添加到数字字符串中
        } else {
            // 如果当前字符不是数字或小数点，且数字字符串不为空
            if (!number.isEmpty()) {
                tokens.append(number); // 将数字添加到输出列表
                number.clear(); // 清空数字字符串
            }
            // 将当前字符作为单独的 token 添加到输出列表
            tokens.append(QString(ch));
        }
    }

    // 如果数字字符串不为空，将其添加到输出列表
    if (!number.isEmpty()) {
        tokens.append(number);
    }

    return tokens; // 返回 token 列表
}

// 补全表达式，使其符合表达式语法
QVector<QString> Calculator::completeExpression(const QVector<QString>& tokens) const {
    QVector<QString> result;

    // 遍历输入的tokens
    for (int i = 0; i < tokens.length(); ++i) {
        // 判断当前token的前一个和后一个是否是非数字和非右括号
        bool prevNonNumberAndParen = (i == 0 || (!mValidator->isNumeric(tokens[i - 1]) && tokens[i - 1] != ")"));
        bool nextNonNumberAndParen = (i == tokens.length() - 1 || (!mValidator->isNumeric(tokens[i + 1]) && tokens[i + 1] != "("));

        // 处理加号
        if (tokens[i] == "+") {
            if (prevNonNumberAndParen && nextNonNumberAndParen) {
                // 前后都是非数字和非右括号，插入0 + 0
                result.append("0");
                result.append("+");
                result.append("0");
            } else if (prevNonNumberAndParen) {
                // 前是非数字和非右括号，插入0 +
                result.append("0");
                result.append("+");
            } else if (nextNonNumberAndParen) {
                // 后是非数字和非右括号，插入+ 0
                result.append("+");
                result.append("0");
            } else {
                // 前后都不是非数字和非右括号，直接插入+
                result.append("+");
            }
        }
        // 处理减号
        else if (tokens[i] == "-") {
            if (prevNonNumberAndParen && nextNonNumberAndParen) {
                // 前后都是非数字和非右括号，插入0 - 0
                result.append("0");
                result.append("-");
                result.append("0");
            } else if (prevNonNumberAndParen) {
                // 前是非数字和非右括号，插入0 -
                result.append("0");
                result.append("-");
            } else if (nextNonNumberAndParen) {
                // 后是非数字和非右括号，插入- 0
                result.append("-");
                result.append("0");
            } else {
                // 前后都不是非数字和非右括号，直接插入-
                result.append("-");
            }
        }
        // 处理乘号
        else if (tokens[i] == "×") {
            if (prevNonNumberAndParen && nextNonNumberAndParen) {
                // 前后都是非数字和非右括号，插入0 × 0
                result.append("0");
                result.append("×");
                result.append("0");
            } else if (prevNonNumberAndParen) {
                // 前是非数字和非右括号，插入1 ×
                result.append("1");
                result.append("×");
            } else if (nextNonNumberAndParen) {
                // 后是非数字和非右括号，插入× 1
                result.append("×");
                result.append("1");
            } else {
                // 前后都不是非数字和非右括号，直接插入×
                result.append("×");
            }
        }
        // 处理除号
        else if (tokens[i] == "÷") {
            if (prevNonNumberAndParen && nextNonNumberAndParen) {
                // 前后都是非数字和非右括号，插入0 ÷ 1
                result.append("0");
                result.append("÷");
                result.append("1");
            } else if (prevNonNumberAndParen) {
                // 前是非数字和非右括号，插入0 ÷
                result.append("0");
                result.append("÷");
            } else if (nextNonNumberAndParen) {
                // 后是非数字和非右括号，插入÷ 1
                result.append("÷");
                result.append("1");
            } else {
                // 前后都不是非数字和非右括号，直接插入÷
                result.append("÷");
            }
        }
        // 处理左括号
        else if (tokens[i] == "(") {
            bool isPrevNumber = (i != 0 && mValidator->isNumeric(tokens[i - 1]));
            if (isPrevNumber) {
                // 前一个token是数字，插入× (
                result.append("×");
                result.append("(");
            }
            else {
                // 前一个token不是数字，直接插入(
                result.append("(");
            }
        }
        // 处理右括号
        else if (tokens[i] == ")") {
            bool isNextNumberOrLeftParen = (i != tokens.length() - 1 &&
                                            (mValidator->isNumeric(tokens[i + 1]) ||tokens[i + 1] == "("));
            if (isNextNumberOrLeftParen) {
                // 后一个token是数字或左括号，插入) ×
                result.append(")");
                result.append("×");
            }
            else {
                // 后一个token不是数字或左括号，直接插入)
                result.append(")");
            }
        }
        // 处理包含小数点的数字
        else if (tokens[i].contains(".")) {
            bool lackIntegerPart = tokens[i].startsWith('.');
            bool lackFractionalPart = tokens[i].endsWith('.');
            if (lackIntegerPart && lackFractionalPart) {
                // 缺少整数部分和小数部分，插入0.0
                result.append('0' + tokens[i] + '0');
            }
            else if (lackIntegerPart) {
                // 缺少整数部分，插入0
                result.append('0' + tokens[i]);
            }
            else if (lackFractionalPart) {
                // 缺少小数部分，插入0
                result.append(tokens[i] + '0');
            }
            else {
                // 整数和小数部分都不缺，直接插入
                result.append(tokens[i]);
            }
        }
        // 处理其他token
        else {
            result.append(tokens[i]);
        }
    }
    return result;
}


// 将中缀表达式转换为后缀表达式
QVector<QString> Calculator::infixToPostfix(const QVector<QString>& tokens) const {
    QVector<QString> output; // 用于存储后缀表达式
    QStack<QString> operators; // 用于存储运算符的栈

    for (const QString& token : tokens) {
        // 如果 token 是数字
        bool isNumber;
        token.toDouble(&isNumber);
        if (isNumber) {
            output.append(token); // 将数字添加到输出列表
        } else {
            // 如果 token 是运算符
            if (mValidator->isOperator(token[0])) {
                // 如果栈顶运算符优先级高于或等于当前运算符，则弹出并添加到输出列表
                while (!operators.isEmpty() && mValidator->isOperator(operators.top()[0]) && precedence(token[0]) <= precedence(operators.top()[0])) {
                    output.append(operators.top());
                    operators.pop();
                }
                operators.push(token); // 将当前运算符压入栈
            }
            // 如果 token 是左括号
            else if (token == "(") {
                operators.push(token); // 将左括号压入栈
            }
            // 如果 token 是右括号
            else if (token == ")") {
                // 弹出运算符直到遇到左括号
                while (!operators.isEmpty() && operators.top() != "(") {
                    output.append(operators.top());
                    operators.pop();
                }
                operators.pop(); // 弹出左括号
            }
        }
    }

    // 将栈中剩余的运算符全部弹出并添加到输出列表
    while (!operators.isEmpty()) {
        output.append(operators.top());
        operators.pop();
    }

    return output; // 返回后缀表达式
}

// 计算后缀表达式的值
long double Calculator::evaluatePostfix(const QVector<QString>& postfix) const {
    QStack<long double> stack; // 用于存储操作数的栈

    // 遍历后缀表达式的每个token
    for (const QString& token : postfix) {
        // 如果token是数字或浮点数
        if (token.length() == 1 && mValidator->isOperator(token[0])) {
            if (token == "%") {
                stack.top() /= 100; // 将栈顶元素除以100
                continue;
            }

            if (token == '!') {
                stack.top() = factorial(stack.top());
                continue;
            }

            long double b = stack.pop(); // 弹出栈顶的第二个操作数
            long double a = stack.pop(); // 弹出栈顶的第一个操作数

            // 根据运算符执行相应的运算
            if (token == "+") {
                stack.push(a + b);
            }
            else if (token == "-") {
                stack.push(a - b);
            }
            else if (token == "×") {
                stack.push(a * b);
            }
            else if (token == "÷") {
                stack.push(a / b);
            }
            else if (token == '^') {
                stack.push(qPow(a, b));
            }
        }
        else {
            stack.push(token.toDouble()); // 将token转换为double并压入栈
        }
    }

    return stack.top(); // 返回栈顶元素，即最终结果
}

// 获取运算符的优先级
int Calculator::precedence(const QChar& ch) const {
    if (ch == '+' || ch == '-') return 1;
    if (ch == QChar(0x00D7) || ch == QChar(0x00F7)) return 2;
    if (ch == '%' || ch == '!' || ch == '^') return 3;    // ‘%’在此处代表的不是取余，而是对前一个数除以100
    return 0;
}

// 计算阶乘
long double Calculator::factorial(const int val) const {
    long double result = 1;
    for (int i = val; i >= 1; --i) {
        result *= i;
    }
    return result;
}


