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

const SignatureList *ImagesList::operator[](int row) const
{
    return imagesIndex[row]->second->signatures;
}


void ImagesList::addImages(const QStringList &files)
{
    nRows=size();

    for(const auto& i:files)
    {
        const auto insertion=emplace(i, nullptr );

        if(insertion.second)
        {
            const auto cur=const_cast<QFile*>(&(insertion.first->first));
            if(cur->open(QFile::ReadOnly))
            {
                const auto lst=new SignatureList(cur);
                insertion.first->second=new ImageInfo
                                                 (lst,
                                                  sDetect.detect(cur->fileName()),
                                                  threadCount,
                                                  cur->size(),
                                                  cur->map(0,cur->size()),
                                                  this
                                                 );

                connect(insertion.first->second,&ImageInfo::progressChanged,this,&ImagesList::onProgressChanged);

                connect(this,&ImagesList::setThreadCount,insertion.first->second,&ImageInfo::setThreadCount);

                connect(lst,&SignatureList::findinsUpdated, this, &ImagesList::onFindingsUpdated );

            }
        }
    }

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
                                return cur->second==det;
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

    updateCellBy( 5 + std::find(vDetect.begin(),vDetect.end(),det) - vDetect.begin(), [snd](const Fileindex::value_type& cur)
                            {
                                return cur->second->signatures==snd;
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
                        return QVariant(cur.fileName());
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
                            const int col=index.column() - 5;
                            if(col >= 0 && col < vDetect.size() )
                            {
                                 return QVariant(static_cast<qulonglong>(info->signatures->countOfFindings(vDetect[col])));
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

bool cmpFiles::operator ()(const QFile &a, const QFile &b)
{
    return a.fileName() < b.fileName();
}

QDataStream &operator <<(QDataStream &dev, const ImagesList &lst)
{
    for(const auto&i:lst)
    {
        dev << i.first.fileName()  <<*i.second->signatures;
    }
    return dev;
}
