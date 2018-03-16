#ifndef IMAGESLIST_H
#define IMAGESLIST_H

#include <QAbstractTableModel>
#include <QFile>
#include <map>
#include <vector>

class SignatureList;

class ImagesList : public QAbstractTableModel
{
    Q_OBJECT


    struct cmpFiles
    {
        bool operator () (const QFile& a,const QFile& b);
    };

    using Filemap=std::map<QFile,SignatureList*,cmpFiles>;
    using Fileindex=std::vector<Filemap::const_iterator>;

    Filemap images;
    Fileindex imagesIndex;
    size_t nRows;

public:
    ImagesList(QObject* parent);

    SignatureList* operator[] (int row) const
    {
        return imagesIndex[row]->second;
    }

public slots:
    void addImages(const QStringList& files);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // IMAGESLIST_H
