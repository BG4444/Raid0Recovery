#include "signaturelist.h"
#include "signaturedefinterface.h"

#include <QFile>

SignatureList::SignatureList(QFile *parent):QAbstractTableModel(parent)
{

}


int SignatureList::rowCount(const QModelIndex &) const
{
    return findings.size();
}

int SignatureList::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant SignatureList::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

void SignatureList::registerSignature(const uchar *offset)
{
    const auto s = sender();

    const auto def=dynamic_cast<SignatureDetector*>(s);

    if(!def)
    {
        throw excWrongSender();
    }

    const auto it=findings.find(offset);

    if(it!=findings.end())
    {
        throw excDuplicateAddress();
    }

    findings.insert(std::make_pair(offset,def));
}
