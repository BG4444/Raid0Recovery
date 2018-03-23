#ifndef SIGNATURELIST_H
#define SIGNATURELIST_H

#include <QAbstractTableModel>
#include <QIODevice>
#include <signaturedefinterface.h>

class QFile;
class SignatureDetector;

class SignatureList : public QAbstractTableModel
{
    Q_OBJECT    

    using Findings=std::multimap<const SignatureDefInterface*,const uchar*>;

    Findings findings;

public:

    class excWrongSender{};
    class excDuplicateAddress{};

    SignatureList(QFile* parent);

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;   
    size_t countOfFindings(const SignatureDefInterface* det);

public slots:
    void registerSignature(const uchar* offset);
signals:
    void findinsUpdated(const SignatureDefInterface* det);
    friend QDataStream&operator <<( QDataStream &dev, const SignatureList& lst);

};

QDataStream&operator <<( QDataStream &dev, const SignatureList& lst);

#endif // SIGNATURELIST_H
