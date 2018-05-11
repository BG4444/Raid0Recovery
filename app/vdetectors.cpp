#include "vdetectors.h"
#include <QPluginLoader>

vDetectors::vDetectors()
{

}

QPluginLoader *vDetectors::operator[](const size_t idx) const
{
    const auto newpos=begin()+idx;
    return ( newpos )->first;
}

vPlugins::const_iterator operator+(vPlugins::const_iterator it, const int count)
{
    for(int k=0; k<count;++k,++it);
    return it;
}


int vDetectors::rowCount(const QModelIndex &parent) const
{
    return size();
}

QVariant vDetectors::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= rowCount(QModelIndex()) || index.column()!=0)
    {
        return QVariant();
    }

    switch(role)
    {
        case Qt::DisplayRole:
            return operator [](index.row())->metaData().find("className").value().toString();
        case Qt::UserRole:
            return reinterpret_cast<qulonglong>(operator [](index.row()));
        default:
            return QVariant();
    }
}

pluginInfo::pluginInfo(int id, QCheckBox *enabled):id(id),enabled(enabled)
{

}
