#include "signaturelist.h"
#include "signaturedefinterface.h"
#include "imageslist.h"


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

SignatureList::SignatureList(QObject *parent, const vDetectors &vDet):QAbstractTableModel(parent),vDet(vDet)
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

SignatureList::Findings::const_iterator SignatureList::roll(const QModelIndex& index) const
{
    auto pos=findings.begin();
    for(int i=0;i<index.row();++i)
    {
        ++pos;
    }

    return pos;
}

QVariant SignatureList::data(const QModelIndex& index, int role ) const
{
    switch(role)
    {
        case Qt::DisplayRole:
        {
            if(index.row()<0 || size_t(index.row()) > findings.size())
            {
                return QVariant();
            }
            auto pos = roll(index);
            switch(index.column())
            {
                case 0:
                {
                    const QPluginLoader* ldr=pos->first;
                    return ldr->metaData().find("className").value().toString();
                }
                case 1:
                {
                    const qulonglong ptr = pos->second;
                    return QString::number(ptr,16);
                }
            }
            return QVariant();
        }
        default:
            return QVariant();
    }
}

size_t SignatureList::countOfFindings(const QPluginLoader *det)
{
    return findings.count(det);
}

void SignatureList::build( const int rowFindingsList, QWidget* buildTab,const FileGlue& glue) const
{
    auto pos=roll(index(rowFindingsList,0));

    QPluginLoader* ldr=const_cast<QPluginLoader*>(pos->first);

    const auto face=qobject_cast<SignatureDefInterface*> (ldr->instance());

    const auto imgs=qobject_cast<ImagesList*>(parent());


    const auto g=glue.insertPersistent();

    constexpr size_t ssize=8192*16;

    const auto& gluedData= imgs->glue(g, ssize, pos->second);

    const auto oper=face->make();

    oper->build(buildTab,ssize);
}


void SignatureList::registerSignature(const quint64 offset)
{
    const auto s = sender();

    const auto def=dynamic_cast<SignatureDetector*>(s);

    if(!def)
    {
        throw excWrongSender();
    }

    const auto inst=instanceByInterface(def->parent);

    registerSignature(inst,offset);

    emit findinsUpdated(def->parent);
}

void SignatureList::registerSignature(const QPluginLoader *plg, quint64 ptr)
{
    const auto it=findings.equal_range(plg);


    if(std::find_if(it.first ,it.second,[ptr](const Findings::value_type& cur)
                                            {
                                                 return ptr==cur.second;
                                            }

                   ) != it.second
      )
    {
        throw excDuplicateAddress();
    }

    findings.insert(std::make_pair(plg,ptr));
}

QDataStream& operator <<(QDataStream &dev, const SignatureList &lst)
{
    dev << static_cast<qulonglong>(lst.findings.size());
    for(const auto& i:lst.findings)
    {
        QPluginLoader* plg= const_cast<QPluginLoader*>(i.first);
        dev << lst.vDet.find(plg)->second << static_cast<qulonglong>(i.second);
    }
    return dev;
}
