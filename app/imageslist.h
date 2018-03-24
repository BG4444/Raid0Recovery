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
#include <functional>
#include <signaturedefinterface.h>
#include "vdetectors.h"

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
    const vDetectors& vDetect;
    int threadCount;

    Fileindex imagesIndex;
    size_t nRows;    

    class InvalidSender{};
    void updateCellBy(const int col,const std::function<bool (const Fileindex::value_type &)> &&func);

public:
    ImagesList(QObject* parent, const StorageDetector& sDetect,const vDetectors& vDetect);
    const SignatureList* operator[] (int row) const;

public slots:
    void addImages(const QStringList& files);
    void onSetThreadCount(const int count);
    void onProgressChanged();
    void onFindingsUpdated(const SignatureDefInterface *det);

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;



signals:
    void setThreadCount(const int count);
    void ProgressChanged(const int value);
    friend QDataStream &operator <<(QDataStream &dev, const ImagesList& lst);
};

QDataStream &operator <<(QDataStream &dev, const ImagesList& lst);

#endif // IMAGESLIST_H
