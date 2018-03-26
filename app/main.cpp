#include "recoverwindow.h"
#include "raid0recoveryapp.h"

int main(int argc, char *argv[])
{
    Raid0RecoveryApp a(argc, argv);
    RecoverWindow w;
    w.showMaximized();

    return a.exec();
}
