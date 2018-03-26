#include "raid0recoveryapp.h"

#include <QMessageBox>

Raid0RecoveryApp::Raid0RecoveryApp(int argc, char* argv[]):QApplication(argc,argv)
{

}


bool Raid0RecoveryApp::notify(QObject * obj, QEvent * ev)
{
    try
    {
        return QApplication::notify(obj,ev);
    }
    catch(...)
    {
        QString msg(tr("on notifying %1 with event %2").arg(obj->objectName()).arg(ev->staticMetaObject.className()));
        QMessageBox::information(nullptr, tr("Exception %-("), msg  );
        quit();
    }
    return false;
}
