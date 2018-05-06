#include "findingsforcompare.h"

FindingsForCompare::FindingsForCompare(const ImagesList &lst,const QPluginLoader* ldr):imgs(lst)
{    
    for(auto i=lst.begin();i!=lst.end();++i)
    {
        findings.reserve(findings.capacity()+ i->second->signatures->count(ldr));
        const auto all=i->second->signatures->allOf(ldr);

        for(auto j=all.first;j!=all.second;++j)
        {
           findings.emplace_back(i,j);
        }
    }
}


int FindingsForCompare::rowCount(const QModelIndex &parent) const
{
    return findings.size();
}

int FindingsForCompare::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant FindingsForCompare::data(const QModelIndex &index, int role) const
{
    if(role!=Qt::DisplayRole || index.row()<0 || index.row() >= rowCount(QModelIndex()) )
    {
        return QVariant();
    }

    const auto& cur=findings[index.row()];
    switch(index.column())
    {
        case 0:
            return cur.file->first.fileName();
        case 1:
            return QString::number(cur.finding->second.offset,16);
        case 2:
            return cur.finding->second.hash.toBase64();
        default:
            return QVariant();
    }
}
