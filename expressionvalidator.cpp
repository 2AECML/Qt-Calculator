#include "expressionvalidator.h"
#include <QStack>

// 判断表达式是否合法
bool ExpressionValidator::isExpressionValid(const QVector<QString> &tokens) const {
    // 检查表达式是否以运算符开头或结尾
    if (tokens.startsWith("+") || tokens.startsWith("-") ||
        tokens.startsWith("×") || tokens.startsWith("÷") ||
        tokens.endsWith("+") || tokens.endsWith("-") ||
        tokens.endsWith("×") || tokens.endsWith("÷")) {
        return false;
    }

    QStack<QString> stack;  // 用于存放左括号的栈

    for (int i = 0; i < tokens.length(); ++i) {
        QString token = tokens[i];

        // 检查括号匹配
        if (token == "(") {
            stack.push(token);
        }
        else if (token == ")") {
            if (stack.isEmpty()) {
                return false;
            }
            stack.pop();
        }

        // 检查连续的运算符
        if (i > 0) {
            QString prevToken = tokens[i - 1];
            if (isOperator(prevToken) && isOperator(token)) {
                return false;   // 连续的运算符
            }

            // 检查连续的小数点
            if (prevToken == "." && token == ".") {
                return false;   // 连续的小数点
            }
        }

        // 检查是否出现不合法的字符
        if (!isTokenValid(token)) {
            return false;
        }

        // 检查%与!的使用是否合理
        if (token == "%" || token == "!") {
            bool prevNonDigitAndParen = (i == 0 || (!isNumeric(tokens[i - 1]) && tokens[i - 1] != ")"));
            bool isNextDigit = (i != tokens.length() - 1 && isNumeric(tokens[i + 1]));
            if (prevNonDigitAndParen || isNextDigit) {
                return false;
            }
        }

        // 检查^的使用是否合理
        if (tokens[i] == "^") {
            bool prevNonDigitAndParen = (i == 0 || (!isNumeric(tokens[i - 1]) && tokens[i - 1] != ")"));
            bool nextNonDigitAndParen = (i == tokens.length() - 1 || (!isNumeric(tokens[i + 1]) && tokens[i + 1] != '('));
            if (prevNonDigitAndParen || nextNonDigitAndParen) {
                return false;
            }
        }
    }

    // 检查括号是否完全匹配
    if (!stack.isEmpty()) {
        return false;
    }

    return true;
}

// 判断字符是否为运算符
bool ExpressionValidator::isOperator(const QChar& ch) const {
    return ch == '+' ||
           ch == '-' ||
           ch == QChar(0x00D7) ||
           ch == QChar(0x00F7) ||
           ch == '%' ||
           ch == '!' ||
           ch == '^';
}

// 判断字符串是否为运算符
bool ExpressionValidator::isOperator(const QString& str) const {
    return str == "+" ||
           str == "-" ||
           str == "×" ||
           str == "÷" ||
           str == "%" ||
           str == "!" ||
           str == "^";
}

// 判断字符串是否为数字
bool ExpressionValidator::isNumeric(const QString& str) const {
    bool ok = false;
    str.toDouble(&ok);
    return ok;
}

// 判断字符串是否为合法的字符
bool ExpressionValidator::isTokenValid(const QString& token) const {
    return isNumeric(token) ||
           isOperator(token) ||
           token == "." ||
           token == "(" ||
           token == ")";
}

// 判断点的位置是否合法
bool ExpressionValidator::isPointValid(const QString& expression) const {
    QString lastNumber = getLastNumberOfExpression(expression);
    if (lastNumber.contains('.') || expression.endsWith('.')) {
        return false;
    }
    return true;
}

// 判断运算符的位置是否合法
bool ExpressionValidator::isOperatorValid(const QString& expression) const {
    if (expression.endsWith("+") || expression.endsWith("-") ||
        expression.endsWith("×") || expression.endsWith("÷") ||
        expression.endsWith("^")) {
        return false;
    }
    return true;
}

// 获取表达式的最后一个数字
QString ExpressionValidator::getLastNumberOfExpression(const QString& expression) const {
    unsigned int start = 0; // 记录最后一个数字的开始位置
    unsigned int length = 0;    // 记录最后一个数字的长度
    if (!expression.isEmpty()) {
        // 遍历表达式的每个字符
        for (int i = expression.size() - 1; i >= 0; --i) {
            QChar ch = expression[i];
            if (ch.isDigit()) {
                length++;
            }
            // 遇到运算符或括号，记录位置并跳出循环
            if (ch == '+' || ch == '-' || ch == QChar(0x00D7) || ch == QChar(0x00F7) || ch == '(' || ch == ')') {
                start = i + 1;
                break;
            }
        }
    }
    QString result = expression.mid(start, length); // 截取表达式的最后一个数字
    return result;
}
