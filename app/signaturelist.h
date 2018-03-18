#ifndef SIGNATURELIST_H
#define SIGNATURELIST_H

#include <QAbstractTableModel>

class QFile;
class SignatureDetector;

class SignatureList : public QAbstractTableModel
{
    Q_OBJECT    

    std::map<const uchar*,SignatureDetector*> findings;

public:

    class excWrongSender{};
    class excDuplicateAddress{};

    SignatureList(QFile* parent);


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;   
public slots:
    void registerSignature(const uchar* offset);
};

#endif // SIGNATURELIST_H
