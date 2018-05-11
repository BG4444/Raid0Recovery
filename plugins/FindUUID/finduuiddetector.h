#ifndef FindUUIDDETECTOR_H
#define FindUUIDDETECTOR_H

#include "signaturedefinterface.h"
#include "signaturedetectorbase.h"

class FindUUIDDetector : public SignatureDetectorBase
{                
public:
    FindUUIDDetector(const SignatureDefInterface* parent,const vSign& sign);
    void run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs);
    void build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size);
    QString getDescription(const quint64 offset, QFile *file);    
};

#endif // FindUUIDDETECTOR_H
