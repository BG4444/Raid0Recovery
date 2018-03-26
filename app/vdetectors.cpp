#include "vdetectors.h"

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
