#ifndef SIGNATURELIST_H
#define SIGNATURELIST_H

#include <QAbstractTableModel>
#include <QIODevice>
#include <signaturedefinterface.h>
#include <QPluginLoader>
#include "vdetectors.h"
#include <QDataStream>

class QFile;
class SignatureDetector;
class ImagesList;

class SignatureList : public QAbstractTableModel
{
    Q_OBJECT    

    using Findings=std::multimap<const QPluginLoader*,const uchar*>;

    Findings findings;

    const vDetectors& vDet;

    const QPluginLoader* instanceByInterface(const SignatureDefInterface* det);

    const uchar* base;

    const quint64 size;

    Findings::const_iterator roll(const QModelIndex& index) const;

public:

    class excWrongSender{};
    class excDuplicateAddress{};

    SignatureList(QObject *parent, const vDetectors& vDet, const uchar* base, const u_int64_t size);

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;   
    size_t countOfFindings(const QPluginLoader *det);
    void build(const int rowFindingsList, QWidget *buildTab) const;
public slots:
    void registerSignature(const uchar* offset);
    void registerSignature(const QPluginLoader* plg, qulonglong ptr);
signals:
    void findinsUpdated(const SignatureDefInterface* det);
    friend QDataStream&operator <<( QDataStream &dev, const SignatureList& lst);
};

QDataStream& operator <<( QDataStream &dev, const SignatureList& lst);

#endif // SIGNATURELIST_H
