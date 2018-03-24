#ifndef FILEGLUE_H
#define FILEGLUE_H

#include <vector>
#include <cstddef>
#include <ostream>

using Glue=std::vector<size_t>;

class FileGlue:public Glue
{
    const size_t startPosition;
    void inc(Glue::iterator pos);

public:
    FileGlue(const size_t countOfImages,const size_t startPosition);
    Glue insertPersistent();
    void operator ++();
};

std::ostream &operator << (std::ostream& str,const Glue& glue);

#endif // FILEGLUE_H
