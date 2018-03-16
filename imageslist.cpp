#include "imageslist.h"
#include "signaturelist.h"

ImagesList::ImagesList(QObject* parent):QAbstractTableModel(parent),nRows(0)
{

}


void ImagesList::addImages(const QStringList &files)
{
    nRows=images.size();

    for(const auto& i:files)
    {
        const auto insertion=images.emplace(i,nullptr);

        if(insertion.second)
        {
            const auto cur=const_cast<QFile*>(&(insertion.first->first));
            if(cur->open(QFile::ReadOnly))
            {
                insertion.first->second=new SignatureList(cur);
            }
        }
    }

    if(nRows!=images.size())
    {
        Fileindex t_index;

        t_index.reserve(images.size());

        auto indexPos=imagesIndex.cbegin();
        size_t pos=0;

        for(auto i=images.cbegin();i!=images.cend();++i,++pos)
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


int ImagesList::rowCount(const QModelIndex &) const
{
    return nRows;
}

int ImagesList::columnCount(const QModelIndex &) const
{
    return 3 + 1;
}

QVariant ImagesList::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            if(index.row() >= 0 && (size_t(index.row())) < nRows )
            {
                const auto& cur=imagesIndex[index.row()]->first;
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
                        if(imagesIndex[index.row()]->second)
                        {
                            return QVariant("Ok");
                        }
                        return QVariant();
                    default:
                        return QVariant();
                }
            }
            return QVariant();
        default:
           return QVariant();
    }
}

bool ImagesList::cmpFiles::operator ()(const QFile &a, const QFile &b)
{
    return a.fileName() < b.fileName();
}
