#include "imageslist.h"
#include "signaturelist.h"

#include <QPoint>


ImagesList::ImagesList(QObject* parent, const StorageDetector &sDetect, const vDetectors &vDetect):
    QAbstractTableModel(parent),
    sDetect(sDetect),
    vDetect(vDetect),
    threadCount(1),
    nRows(0)
{

}

SignatureList *ImagesList::operator[](int row) const
{
    return imagesIndex[row]->second->signatures.get();
}

int ImagesList::findSignatureList(const SignatureList *lst) const
{
    return std::find_if(imagesIndex.begin(),imagesIndex.end(), [lst](const Fileindex::value_type& cur)
                                                                {
                                                                    return cur->second->signatures.get()==lst;
                                                                }
                )-imagesIndex.begin();
}

QByteArray ImagesList::glue(const Glue& glue,const quint64 sector_size, const quint64 offset)
{
    QByteArray ret(glue.size()*sector_size,Qt::Uninitialized);

    auto pos=ret.begin();

    for(const auto& i:glue)
    {        
        QFile* f  = const_cast<QFile*>(&imagesIndex[i]->first);

        f->seek(offset);
        f->read(pos,sector_size);
        pos+=sector_size;
    }

    return ret;
}

void ImagesList::resetCapturedParts()
{
    for(auto& i:*this)
    {
        i.second->nUsedAlgorithms=0;
    }
}


Filemap::const_iterator ImagesList::insFile(const QString& fName)
{
    const auto insertion=emplace(fName, nullptr );
    if(insertion.second)
    {
        const auto cur=const_cast<QFile*>(&(insertion.first->first));
        if(cur->open(QFile::ReadOnly))
        {            
            const auto lst=new SignatureList(this,vDetect);
            insertion.first->second.reset(new ImageInfo
                                             (lst,
                                              sDetect.detect(cur->fileName()),
                                              threadCount,
                                              this
                                             ));
            connect(lst,&SignatureList::storeLog,this,&ImagesList::storeLog);

            connect(insertion.first->second.get(),&ImageInfo::progressChanged,this,&ImagesList::onProgressChanged);

            connect(this,&ImagesList::setThreadCount,insertion.first->second.get(),&ImageInfo::setThreadCount);

            connect(lst,&SignatureList::findinsUpdated, this, &ImagesList::onFindingsUpdated );
        }
    }
    return insertion.first;
}

void ImagesList::reindexRows()
{
    if(nRows!=size())
    {
        Fileindex t_index;

        t_index.reserve(size());

        auto indexPos=imagesIndex.cbegin();
        size_t pos=0;

        for(auto i=cbegin();i!=cend();++i,++pos)
        {
           if(indexPos < imagesIndex.cend() && i==*indexPos)
           {
               ++indexPos;
           }
           else
           {
               beginInsertRows(QModelIndex(),pos,pos);
               ++nRows;
               endInsertRows();
           }
           t_index.push_back(i);
        }
        imagesIndex.swap(t_index);
    }
}

void ImagesList::addImages(const QStringList &files)
{
    nRows=size();

    for(const auto& i:files)
    {        
        insFile(i);
    }

    reindexRows();
}

void ImagesList::onSetThreadCount(const int count)
{    
    if(threadCount!=count)
    {
        threadCount=count;
        emit setThreadCount(count);
    }
}

void ImagesList::updateCellBy(const int col,const std::function<bool(const Fileindex::value_type&)>&& func)
{
    const auto pos=std::find_if(imagesIndex.cbegin(), imagesIndex.cend(), func );

    if(pos==imagesIndex.cend())
    {
        throw InvalidSender();
    }
    else
    {
        const auto tIdx=index(pos-imagesIndex.cbegin(), col);
        emit dataChanged(tIdx,tIdx,{Qt::DisplayRole});
    }
}

void ImagesList::onProgressChanged()
{
    const auto& det=sender();
    updateCellBy(4, [det](const Fileindex::value_type& cur )
                            {
                                return cur->second.get()==det;
                            }
                        );
    int ret=0;
    for (const auto&i:*this)
    {
        for(const auto&j: i.second->getProgress())
        {
            ret+=j;
        }
    }
    emit ProgressChanged(ret);
}

void ImagesList::onFindingsUpdated(const SignatureDefInterface* det)
{
    const auto snd= dynamic_cast<SignatureList*>(sender());

    if (!snd)
    {

    }

    int col= 5;

    for(const auto& i:vDetect)
    {
        if(qobject_cast<SignatureDefInterface*>(i.first->instance())==det)
        {
            break;
        }
        ++col;
    }

    updateCellBy(col , [snd](const Fileindex::value_type& cur)
                            {
                                return cur->second->signatures.get()==snd;
                            }
                );
}


int ImagesList::rowCount(const QModelIndex &) const
{
    return nRows;
}

int ImagesList::columnCount(const QModelIndex &) const
{
    return 4 + 1 + vDetect.size();
}

QVariant ImagesList::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            if(index.row() >= 0 && (size_t(index.row())) < nRows )
            {
                const auto& cur=imagesIndex[index.row()]->first;

                const auto& info=imagesIndex[index.row()]->second;

                switch(index.column())
                {
                    case 0:
                        return cur.fileName();
                    case 1:
                        if(cur.isOpen())
                        {
                            return QString::number(cur.size());
                        }
                        return QVariant();
                    case 2:
                        if(info->signatures)
                        {
                            return "Ok";
                        }                        
                        return QVariant();
                    case 3:
                        if(info->storage!=sDetect.cend())
                        {
                            return info->storage->first.displayName();
                        }
                        return QVariant();
                    case 4:
                        return QVariant(reinterpret_cast<qulonglong>(&info->getProgress()));                    
                    default:
                        {
                            const size_t col=index.column() - 5;
                            if( col < vDetect.size() )
                            {
                                 return QVariant(static_cast<qulonglong>
                                                 (
                                                     info->signatures->countOfFindings(
                                                                vDetect[col]
                                                                                      )
                                                 )
                                                );
                            }
                            return QVariant();
                        }
                }
            }
            return QVariant();
        default:
           return QVariant();
    }
}

QVariant ImagesList::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            if(orientation==Qt::Vertical)
            {
                return 1+section;
            }
            if(section>=0 && section <5)
            {
                const auto names={
                                    tr("Image Name"),
                                    tr("Size"),
                                    tr("Mapped"),
                                    tr("Drive"),
                                    tr("Progress")
                                 };
                return *(names.begin()+section);
            }
            return vDetect[section-5]->metaData().find("className").value().toString();
        default:
            return QAbstractTableModel::headerData(section,orientation,role);
    }
}

QFile *ImagesList::getFile(int row)
{
    return const_cast<QFile*>(&imagesIndex[row]->first);
}

bool cmpFiles::operator ()(const QFile &a, const QFile &b)
{
    quint8 alpha;
    quint8 beta;
    bool aopen=false;
    bool bopen=false;

    if(!a.isOpen())
    {
        const_cast<QFile*>(&a)->open(QFile::ReadOnly);
        aopen=true;
    }
    if(!b.isOpen())
    {
        const_cast<QFile*>(&b)->open(QFile::ReadOnly);
        bopen=true;
    }


    const_cast<QFile*>(&a)->seek(99);
    const_cast<QFile*>(&a)->read(reinterpret_cast<char*>(&alpha),1);

    if(aopen)
    {
        const_cast<QFile*>(&a)->close();
    }

    const_cast<QFile*>(&b)->seek(99);
    const_cast<QFile*>(&b)->read(reinterpret_cast<char*>(&beta),1);

    if(bopen)
    {
        const_cast<QFile*>(&b)->close();
    }

    return alpha<beta;
}

QDataStream &operator <<(QDataStream &dev, const ImagesList &lst)
{
    dev << static_cast<qulonglong>(lst.vDetect.size());

    for(size_t i=0;i < lst.vDetect.size();++i)
    {
        for(const auto&j:lst.vDetect)
        {
            if(j.second.id==int(i))
            {
                const QPluginLoader* plg=j.first;
                dev << plg->metaData().find("className").value().toString();
                break;
            }
        }
    }

    dev << static_cast<qulonglong>(lst.size());

    for(const auto&i:lst)
    {
        dev << i.first.fileName()  <<*i.second->signatures;
    }
    return dev;
}

QDataStream &operator >>(QDataStream &dev,  ImagesList &lst)
{
    lst.clear();
    std::map<int, vDetectors::const_iterator > dict;

    {
        qulonglong sz;
        dev >> sz;


        for(qulonglong i=0;i!=sz;++i)
        {
            QString className;
            dev >> className;

            const auto clPos=std::find_if(lst.vDetect.cbegin(),lst.vDetect.cend(), [&className](const vDetectors::value_type& cur)
                                                                {
                                                                    return cur.first->metaData().find("className").value().toString() == className;
                                                                }
                        );
            if(clPos!=lst.vDetect.end())
            {
                dict[i]=clPos;
            }
        }
    }
    {
        qulonglong sz;
        dev >> sz;

        for(qulonglong i=0;i!=sz;++i)
        {
            QString fName;
            dev>>fName;
            auto sigList=lst.insFile(fName);
                qulonglong cnt;
                dev>>cnt;

                for(qulonglong j=0;j!=cnt;++j)
                {
                    //loading plugin no
                    int plg;
                    dev>>plg;

                    //loading offset
                    qulonglong ptr;
                    dev>>ptr;

                    const QPluginLoader* ldr=dict[plg]->first;

                    const auto pName=ldr->metaData().find("className").value().toString();

                    sigList->second->signatures-> registerSignature(ldr , ptr );
                }

        }
        lst.reindexRows();
    }

    return dev;
}

void ImagesList::clear()
{
    beginRemoveRows(QModelIndex(),0,rowCount(QModelIndex()));
    Filemap::clear();
    imagesIndex.clear();
    nRows=0;
    endRemoveRows();
}
