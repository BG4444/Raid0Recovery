#include "fileglue.h"
#include <set>

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

FileGlue::FileGlue(const size_t countOfImages, const size_t startPosition):Glue(countOfImages-1),startPosition(startPosition)
{
    for(size_t i=0;i<size();++i)
    {
        (*this)[i]=i;
    }
}

Glue FileGlue::insertPersistent()
{
    Glue ret;
    ret.reserve(size()+1);

    ret.push_back(startPosition);

    for(const auto& i:*this)
    {
        ret.push_back( i>=startPosition ? 1+i : i );
    }
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

std::ostream &operator <<(std::ostream &str, const Glue &glue)
{
    for(const auto&i:glue)
    {
        str<<i<<' ';
    }
    return str;
}
