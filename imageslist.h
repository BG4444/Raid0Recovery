#ifndef IMAGESLIST_H
#define IMAGESLIST_H

#include "imageinfo.h"

#include <QAbstractTableModel>
#include <QFile>
#include <map>
#include <vector>
#include <storagedetector.h>
#include <memory>
#include <QMutex>

class SignatureList;

struct cmpFiles
{
    bool operator () (const QFile& a,const QFile& b);
};


using Filemap=std::map<QFile, ImageInfo*, cmpFiles>;


using Fileindex=std::vector<Filemap::const_iterator>;


class ImagesList : public QAbstractTableModel, public Filemap
{
    Q_OBJECT

    const StorageDetector& sDetect;
    size_t threadCount;

    Fileindex imagesIndex;
    size_t nRows;

public:
    ImagesList(QObject* parent, const StorageDetector& sDetect);

    const SignatureList* operator[] (int row) const;

public slots:
    void addImages(const QStringList& files);
    void setThreadCount(const int count);
    void progressChanged();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // IMAGESLIST_H
