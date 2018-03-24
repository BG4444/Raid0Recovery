#ifndef VDETECTORS_H
#define VDETECTORS_H

#include <map>

class QPluginLoader;

using vPlugins=std::map<QPluginLoader  *,int>;

vPlugins::const_iterator operator+(vPlugins::const_iterator it,const int count);


class vDetectors:public vPlugins
{
public:
    vDetectors();

    QPluginLoader * operator[](const size_t idx) const;
};


#endif // VDETECTORS_H
