#include "signaturelist.h"
#include "signaturedefinterface.h"

#include <QFile>
#include <QMetaClassInfo>

SignatureList::SignatureList(QFile *parent):QAbstractTableModel(parent)
{

}


int SignatureList::rowCount(const QModelIndex &) const
{
    return findings.size();
}

int SignatureList::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant SignatureList::data(const QModelIndex&, int ) const
{
    return QVariant();
}

size_t SignatureList::countOfFindings(const SignatureDefInterface *det)
{
    return findings.count(det);
}


void SignatureList::registerSignature(const uchar *offset)
{
    const auto s = sender();

    const auto def=dynamic_cast<SignatureDetector*>(s);

    if(!def)
    {
        throw excWrongSender();
    }

    const auto it=findings.equal_range(def->parent);

    if(std::find_if(it.first ,it.second,[offset](const Findings::value_type& cur)
                                            {
                                                 return offset==cur.second;
                                            }

                   ) != it.second
      )
    {
        throw excDuplicateAddress();
    }

    findings.insert(std::make_pair(def->parent,offset));
    emit findinsUpdated(def->parent);
}

QDataStream &operator <<(QDataStream &dev, const SignatureList &lst)
{
    for(const auto& i:lst.findings)
    {
        const auto obj=reinterpret_cast<const QObject*>(i.first);
        const auto count = obj->metaObject()->classInfoCount();
        for (int i = 0; i < count; ++i)
        {
            const auto& info = obj->metaObject()->classInfo(i);
            if (strcmp(info.name(), "Interface") == 0)
            {
                dev << QString::fromLatin1(info.value());
            }
        }
    }
    return dev;
}
