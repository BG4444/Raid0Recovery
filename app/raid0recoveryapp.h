#ifndef RAID0RECOVERYAPP_H
#define RAID0RECOVERYAPP_H

#include <QApplication>

class Raid0RecoveryApp : public QApplication
{
public:
    Raid0RecoveryApp(int argc,  char* argv[]);

    // QCoreApplication interface
public:
    bool notify(QObject *obj, QEvent *ev);
};

#endif // RAID0RECOVERYAPP_H
