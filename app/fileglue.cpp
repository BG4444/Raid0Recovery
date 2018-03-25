#include "fileglue.h"
#include <set>
#include <algorithm>


void FileGlue::inc(Glue::iterator pos)
{
    if(++*pos==size())
    {
        *pos++=0;
        if(pos!=end())
        {
            inc(pos);
            return;
        }
    }
}

FileGlue::FileGlue(const size_t countOfImages, const Glue& persistentPart):persistentPart(persistentPart)
{
    for(size_t i=0;i<countOfImages ;++i)
    {
        const auto fnd=std::find(persistentPart.cbegin(),persistentPart.cend(),i);

        if(fnd==persistentPart.cend())
        {
            push_back(i);
        }
    }
}

Glue FileGlue::insertPersistent() const
{
    Glue ret;

    ret.insert(ret.end(),persistentPart.cbegin(),persistentPart.cend());
    ret.insert(ret.end(),begin(),end());

    return ret;
}

void FileGlue::operator ++()
{
    for(;;)
    {
        inc(begin());
        std::set<size_t> temp(cbegin(),cend());
        if(temp.size()==size())
        {
            break;
        }
    }
}

void FileGlue::ror()
{
    if(empty())
    {
        return;
    }
    const auto t=(*this)[0];
    erase(begin(),begin()+1);
    push_back(t);
}

Glue FileGlue::fix() const
{
    Glue ret;

    ret.insert(ret.end(),persistentPart.cbegin(),persistentPart.cend());
    ret.insert(ret.end(),begin(),begin()+1);

    return ret;
}

std::ostream &operator <<(std::ostream &str, const Glue &glue)
{
    for(const auto&i:glue)
    {
        str<<i<<' ';
    }
    return str;
}
