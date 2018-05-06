#ifndef IMAGESLIST_H
#define IMAGESLIST_H

#include "fileglue.h"
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
#include "imageinfo.h"

class SignatureList;

struct cmpFiles
{
    bool operator () (const QFile& a,const QFile& b);
};

using Filemap=std::map<QFile, std::unique_ptr<ImageInfo>, cmpFiles>;
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

    const_iterator insFile(const QString &fName);

    void reindexRows();

public:
    ImagesList(QObject* parent, const StorageDetector& sDetect,const vDetectors& vDetect);
    SignatureList* operator[] (int row) const;

    int findSignatureList(const SignatureList *lst) const;

    QByteArray glue(const Glue &glue, const quint64 sector_size, const quint64 offset);
    void resetCapturedParts();

public slots:
    void addImages(const QStringList& files);
    void onSetThreadCount(const int count);
    void onProgressChanged();
    void onFindingsUpdated(const SignatureDefInterface *det);
    void clear();    
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


signals:
    void setThreadCount(const int count);
    void ProgressChanged(const int value);
    friend QDataStream &operator <<(QDataStream &dev, const ImagesList& lst);
    friend QDataStream &operator >>(QDataStream &dev,ImagesList& lst);
    void storeLog(const QString& msg);
};


QDataStream &operator <<(QDataStream &dev, const ImagesList& lst);
QDataStream &operator >>(QDataStream &dev, ImagesList& lst);

#endif // IMAGESLIST_H
