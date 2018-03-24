#include "signaturelist.h"
#include "signaturedefinterface.h"

#include <QFile>
#include <QMetaClassInfo>

const QPluginLoader *SignatureList::instanceByInterface(const SignatureDefInterface *det)
{
    return std::find_if(vDet.begin(), vDet.end(), [det] (const vDetectors::value_type& cur)
    {
        return qobject_cast<SignatureDefInterface*>(cur.first->instance()) == det;
    }
    )->first;
}

SignatureList::SignatureList(QFile *parent, const vDetectors &vDet):QAbstractTableModel(parent),vDet(vDet)
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

size_t SignatureList::countOfFindings(const QPluginLoader *det)
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

    const auto inst=instanceByInterface(def->parent);

    const auto it=findings.equal_range(inst);

    if(std::find_if(it.first ,it.second,[offset](const Findings::value_type& cur)
                                            {
                                                 return offset==cur.second;
                                            }

                   ) != it.second
      )
    {
        throw excDuplicateAddress();
    }

    findings.insert(std::make_pair(inst,offset));
    emit findinsUpdated(def->parent);
}

QDataStream& operator <<(QDataStream &dev, const SignatureList &lst)
{
    for(const auto& i:lst.findings)
    {
        QPluginLoader* plg= const_cast<QPluginLoader*>(i.first);
        dev << lst.vDet.find(plg)->second << reinterpret_cast<qulonglong>(i.second);
    }
    return dev;
}
