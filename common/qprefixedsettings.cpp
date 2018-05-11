#include "qprefixedsettings.h"
#include <QSettings>

QPrefixedSettings::QPrefixedSettings(QSettings *base, const QString &prefix):set(base),prefix(prefix)
{

}

QVariant QPrefixedSettings::value(const QString &name, const QVariant &def)
{
    return set->value(prefix+name,def);
}

void QPrefixedSettings::setValue(const QString &name, const QVariant &var)
{
    set->setValue(prefix+name,var);
}
