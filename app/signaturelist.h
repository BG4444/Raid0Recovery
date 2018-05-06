#ifndef SIGNATURELIST_H
#define SIGNATURELIST_H

#include <QAbstractTableModel>
#include <QIODevice>
#include <signaturedefinterface.h>
#include <QPluginLoader>
#include "fileglue.h"
#include "vdetectors.h"
#include <QDataStream>

class QFile;
class SignatureDetector;
class ImagesList;

class SignatureList : public QAbstractTableModel
{
    Q_OBJECT    
public:
    struct findingInfo
    {
        const quint64 offset;
        QByteArray hash;
        findingInfo(const quint64 offset):offset(offset)
        {

        }
    };

    using Findings=std::multimap<const QPluginLoader*,findingInfo>;
private:
    Findings findings;

    const vDetectors& vDet;

    const QPluginLoader* instanceByInterface(const SignatureDefInterface* det);

    Findings::const_iterator roll(const QModelIndex& index) const;

public:

    class excWrongSender{};
    class excDuplicateAddress{};

    SignatureList(QObject *parent, const vDetectors& vDet);
    size_t count(const QPluginLoader* ldr) const;
    std::pair<Findings::iterator, Findings::iterator> allOf(const QPluginLoader* ldr);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;   
    size_t countOfFindings(const QPluginLoader *det);
    void build(const int rowFindingsList, QWidget *buildTab, const FileGlue& glue, const quint64 chunkSize) const;



public slots:
    void registerSignature(const quint64 offset);
    void registerSignature(const QPluginLoader* plg, quint64 ptr);
signals:
    void findinsUpdated(const SignatureDefInterface* det);
    friend QDataStream&operator <<( QDataStream &dev, const SignatureList& lst);
    void storeLog(const QString& mess) const;
};

QDataStream& operator <<( QDataStream &dev, const SignatureList& lst);

#endif // SIGNATURELIST_H
