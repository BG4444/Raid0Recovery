#include "storagedetector.h"

bool cmpStorages::operator ()(const QStorageInfo &a, const QStorageInfo &b)
{
    const auto& nA=a.displayName();
    const auto& nB=b.displayName();

    return nA.length() == nB.length() ?  nA > nB
                                      :  nA.length() > nB.length();
}

StorageDetector::StorageDetector()
{
    const auto t=QStorageInfo::mountedVolumes();
    insert(t.begin(),t.end());
}
