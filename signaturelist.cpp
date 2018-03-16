#include "signaturelist.h"
#include "signaturedef.h"

#include <QFile>

SignatureList::SignatureList(QFile *parent):QAbstractTableModel(parent),
    size(parent->size()),
    base(parent->map(0,size))
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

bool SignatureList::isMapped() const
{
    return base;
}

void SignatureList::registerSignature(const uchar *offset)
{
    const auto def=qobject_cast<SignatureDef*>(sender());

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
