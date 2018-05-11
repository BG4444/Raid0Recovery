#ifndef XFSDEF_H
#define XFSDEF_H

#include "signaturedefinterface.h"

#include <QObject>
#include <QtPlugin>
#include <qprefixedsettings.h>
#include <memory>

class FindUUIDDef:public QObject,SignatureDefInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SignatureDefInterface_iid FILE "finduuiddef.json")
    Q_INTERFACES(SignatureDefInterface)
    std::vector<uchar> sign;
    std::unique_ptr <QPrefixedSettings> set;
public:
    SignatureDetector *make();
    void configure(QWidget *parent, QSettings *set);
private slots:
    void onTextChanged(const QString& text);
signals:
    void updateStatus(const QString& text);
};

#endif // XFSDEF_H
