#include "recoverwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RecoverWindow w;
    w.show();

    return a.exec();
}
