#ifndef FILEGLUE_H
#define FILEGLUE_H

#include <vector>
#include <cstddef>
#include <ostream>

using Glue=std::vector<size_t>;

class FileGlue:public Glue
{   
    void inc(Glue::iterator pos);
    const Glue persistentPart;
public:
    FileGlue(const size_t countOfImages, const Glue &persistentPart);
    Glue insertPersistent() const;
    void operator ++();
    void ror();
    Glue fix() const;
};

std::ostream &operator << (std::ostream& str,const Glue& glue);

#endif // FILEGLUE_H
