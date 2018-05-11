#ifndef XFSDEF_H
#define XFSDEF_H

#include "signaturedefinterface.h"

#include <QObject>
#include <QtPlugin>

class ZeroBlocksDef:public QObject,SignatureDefInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SignatureDefInterface_iid FILE "zeroblocksdef.json")
    Q_INTERFACES(SignatureDefInterface)
public:
    SignatureDetector *make();
    void configure(QWidget *parent,QSettings*);
};

#endif // XFSDEF_H
