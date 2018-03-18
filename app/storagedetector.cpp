#include "storagedetector.h"

bool cmpStorages::operator ()(const QStorageInfo &a, const QStorageInfo &b)
{
    const auto& nA=a.displayName();
    const auto& nB=b.displayName();

    return nA.length() == nB.length() ?  nA > nB
                                      :  nA.length() > nB.length();
}

StorageDetector::StorageDetector(const int threadCount)
{
    const auto& t=QStorageInfo::mountedVolumes();

    for(const auto& i:t)
    {
         emplace(i,new QSemaphore(threadCount));
    }
}

storageSet::const_iterator StorageDetector::detect(const QString &fileName) const
{
    return std::find_if(cbegin(),cend(), [fileName](const std::pair<const QStorageInfo&,const PSemaphore&> &nfo)
    {
        return fileName.startsWith(nfo.first.displayName());
    }
    );
}

void StorageDetector::setThreadCount(const int count)
{
    for(auto& i:*this)
    {
        i.second.reset(new QSemaphore(count));
    }
}
