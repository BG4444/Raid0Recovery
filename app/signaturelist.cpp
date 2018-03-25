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

SignatureList::SignatureList(QObject *parent, const vDetectors &vDet, const uchar* base, const u_int64_t size):QAbstractTableModel(parent),vDet(vDet),base(base),size(size)
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
    for(size_t i=0;i<index.row();++i)
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
            if(index.row()<0 || index.row()>findings.size())
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
                    const qulonglong ptr = pos->second-base;
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

void SignatureList::build( const int rowFindingsList, QWidget* buildTab) const
{
    auto pos=roll(index(rowFindingsList,0));

    QPluginLoader* ldr=const_cast<QPluginLoader*>(pos->first);

    const auto face=qobject_cast<SignatureDefInterface*> (ldr->instance());

    const auto imgs=qobject_cast<ImagesList*>(parent());

    FileGlue glue(imgs->size(),imgs->findSignatureList(this));
    const auto g=glue.insertPersistent();

    const auto& gluedData= imgs->glue(g, 8192*4, pos->second - base);

    const auto oper=face->make(reinterpret_cast<const uchar*>(gluedData.data()), gluedData.size() );

    oper->build(buildTab);
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

    registerSignature(inst,offset - base);

    emit findinsUpdated(def->parent);
}

void SignatureList::registerSignature(const QPluginLoader *plg, qulonglong ptr)
{
    const auto it=findings.equal_range(plg);
    const uchar* offset=ptr+base;

    if(std::find_if(it.first ,it.second,[offset](const Findings::value_type& cur)
                                            {
                                                 return offset==cur.second;
                                            }

                   ) != it.second
      )
    {
        throw excDuplicateAddress();
    }

    findings.insert(std::make_pair(plg,offset));
}

QDataStream& operator <<(QDataStream &dev, const SignatureList &lst)
{
    dev << static_cast<qulonglong>(lst.findings.size());
    for(const auto& i:lst.findings)
    {
        QPluginLoader* plg= const_cast<QPluginLoader*>(i.first);
        dev << lst.vDet.find(plg)->second << static_cast<qulonglong>(i.second - lst.base);
    }
    return dev;
}
