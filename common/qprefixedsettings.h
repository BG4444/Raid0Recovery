#ifndef QPREFIXEDSETTINGS_H
#define QPREFIXEDSETTINGS_H

#include <QVariant>


class QSettings;

class QPrefixedSettings
{
    QSettings* set;
    const QString prefix;
public:
    QPrefixedSettings(QSettings* base,const QString& prefix);
    QVariant value(const QString& name,const QVariant& def);
    void setValue(const QString& name, const QVariant &var);
};

#endif // QPREFIXEDSETTINGS_H
