#include <QApplication>
#include "control.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Control w;
    QApplication::setQuitOnLastWindowClosed(true);
    return a.exec();
}
