#include "imageslist.h"
#include "signaturelist.h"

ImagesList::ImagesList(QObject* parent, const StorageDetector &sDetect):QAbstractTableModel(parent),nRows(0),sDetect(sDetect),threadCount(2)
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
                insertion.first->second=new ImageInfo
                                                 (new SignatureList(cur),
                                                  sDetect.detect(cur->fileName()),
                                                  threadCount,
                                                  cur->size(),
                                                  cur->map(0,cur->size()),
                                                  this
                                                 );
                connect(insertion.first->second,&ImageInfo::progressChanged,this,&ImagesList::progressChanged);
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

void ImagesList::setThreadCount(const int count)
{
    if(threadCount!=count)
    {
        threadCount=count;
        for(auto& i:*this)
        {
            i.second->sem.reset(new QSemaphore(count));
        }
    }
}

void ImagesList::progressChanged()
{
    for(size_t i=0; i < imagesIndex.size(); ++i)
    {
        if(imagesIndex[i]->second==sender())
        {
            emit dataChanged(index(i,4),index(i,4),{Qt::DisplayRole});
            break;
        }
    }
}


int ImagesList::rowCount(const QModelIndex &) const
{
    return nRows;
}

int ImagesList::columnCount(const QModelIndex &) const
{
    return 4 + 1;
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
                        return info->getProgress();
                    default:
                        return QVariant();
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
