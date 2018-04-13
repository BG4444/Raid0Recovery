#include "recoverwindow.h"
#include "raid0recoveryapp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RecoverWindow w;
    w.showMaximized();

    return a.exec();
}
