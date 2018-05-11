#include "signaturelist.h"
#include "signaturedefinterface.h"
#include "imageslist.h"
#include <macroses.h>


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

size_t SignatureList::count(const QPluginLoader *ldr) const
{
    return findings.count(ldr);
}

std::pair< SignatureList ::Findings::iterator,  SignatureList ::Findings::iterator> SignatureList::allOf(const QPluginLoader *ldr)
{
    return findings.equal_range(ldr);
}


int SignatureList::rowCount(const QModelIndex &) const
{
    return findings.size();
}

int SignatureList::columnCount(const QModelIndex &) const
{
    return 4;
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
            auto ldr=const_cast<QPluginLoader*>(pos->first);
            switch(index.column())
            {
                case 0:
                {                    
                    return ldr->metaData().find("className").value().toString();
                }
                case 1:
                {
                    const qulonglong ptr = pos->second.offset;
                    return QString::number(ptr,16);
                }
                case 2:
                {
                    return pos->second.hash.toBase64();
                }
                case 3:
                {
                     const auto def =  qobject_cast<SignatureDefInterface*> (ldr ->instance()) ->make();
                     const auto imgs=  qobject_cast<ImagesList*>(parent());
                     const auto ret =  def->getDescription(pos->second.offset,imgs->getFile(imgs->findSignatureList(this)));
                     def->deleteLater();
                     return ret;
                }
                default:
                    return QVariant();
            }            
        }
        default:
            return QVariant();
    }
}

size_t SignatureList::countOfFindings(const QPluginLoader *det)
{
    return findings.count(det);
}

void SignatureList::build( const int rowFindingsList, QWidget* buildTab,const FileGlue& glue,const quint64 chunkSize) const
{
    auto pos=roll(index(rowFindingsList,0));

    QPluginLoader* ldr=const_cast<QPluginLoader*>(pos->first);

    const auto face=qobject_cast<SignatureDefInterface*> (ldr->instance());

    const auto imgs=qobject_cast<ImagesList*>(parent());


    const auto g=glue.insertPersistent();



    const auto& gluedData= imgs->glue(g, chunkSize, pos->second.offset);

    QFile gluedDataFile("glued.data");
    gluedDataFile.open(QFile::WriteOnly);
    ASSERT(gluedDataFile.write(gluedData)==gluedData.size());
    gluedDataFile.close();

    const auto oper=face->make();

    oper->build(buildTab,chunkSize,reinterpret_cast<const uchar*>(gluedData.data()),gluedData.size());
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
                                                 return ptr==cur.second.offset;
                                            }

                   ) != it.second
      )
    {
        throw excDuplicateAddress();
    }

    findings.emplace(plg,ptr);
}

QDataStream& operator <<(QDataStream &dev, const SignatureList &lst)
{
    dev << static_cast<qulonglong>(lst.findings.size());
    for(const auto& i:lst.findings)
    {
        QPluginLoader* plg= const_cast<QPluginLoader*>(i.first);
        dev << lst.vDet.find(plg)->second.id << static_cast<qulonglong>(i.second.offset);
    }
    return dev;
}

SignatureList::findingInfo::findingInfo(const quint64 offset):offset(offset)
{

}
