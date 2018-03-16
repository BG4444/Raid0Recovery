#ifndef STORAGEDETECTOR_H
#define STORAGEDETECTOR_H

#include <set>
#include <QStorageInfo>

struct cmpStorages
{
    bool operator () (const QStorageInfo& a,const QStorageInfo& b);
};

using storageSet=std::set<QStorageInfo,cmpStorages>;

class StorageDetector:public storageSet
{
public:
    StorageDetector();
    storageSet::const_iterator detect(const QString& fileName) const
    {
        for(auto i=cbegin(); i!= cend(); ++i)
        {
            if(fileName.startsWith(i -> displayName()))
            {
                return i;
            }
        }
        return cend();
    }
};

#endif // STORAGEDETECTOR_H
