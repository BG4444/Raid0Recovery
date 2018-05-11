#ifndef XFSDEF_H
#define XFSDEF_H

#include "signaturedefinterface.h"

#include <QObject>
#include <QtPlugin>

class XFSDef:public QObject,SignatureDefInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SignatureDefInterface_iid FILE "xfsdef.json")
    Q_INTERFACES(SignatureDefInterface)
public:
    SignatureDetector *make();
    void configure(QWidget *parent, QSettings *set);
};

#endif // XFSDEF_H
