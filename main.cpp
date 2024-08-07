#include "calculatorwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    QApplication a(argc, argv);
    CalculatorWindow w;
    w.show();
    return a.exec();
}
