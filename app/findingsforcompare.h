#ifndef FINDINGSFORCOMPARE_H
#define FINDINGSFORCOMPARE_H

#include "imageslist.h"

#include <QAbstractTableModel>
#include <QCryptographicHash>
#include "signaturelist.h"

struct RowContents
{
    const ImagesList::const_iterator file;
    const SignatureList::Findings::const_iterator finding;
    RowContents(ImagesList::const_iterator file,SignatureList::Findings::const_iterator finding):file(file),finding(finding)
    {

    }
};

using VFindings=std::vector<RowContents>;

class FindingsForCompare : public QAbstractTableModel
{    
    const ImagesList& imgs;
    VFindings findings;
public:
    FindingsForCompare(const ImagesList& lst, const QPluginLoader *ldr);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // FINDINGSFORCOMPARE_H
