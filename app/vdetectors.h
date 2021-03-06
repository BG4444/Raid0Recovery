#ifndef VDETECTORS_H
#define VDETECTORS_H

#include <map>
#include <QAbstractListModel>

class QPluginLoader;
class QCheckBox;

struct pluginInfo
{
    int id;
    QCheckBox* enabled;
    pluginInfo(int id,QCheckBox *enabled);
};

using vPlugins=std::map<QPluginLoader  *,pluginInfo>;

vPlugins::const_iterator operator+(vPlugins::const_iterator it,const int count);


class vDetectors:public vPlugins, public QAbstractListModel
{
public:
    vDetectors();

    QPluginLoader * operator[](const size_t idx) const;   
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};


#endif // VDETECTORS_H
