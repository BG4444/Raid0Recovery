#ifndef ZeroBlocksDETECTOR_H
#define ZeroBlocksDETECTOR_H

#include "signaturedefinterface.h"
#include "signaturedetectorbase.h"

class ZeroBlocksDetector : public SignatureDetectorBase
{        
    static constexpr size_t gran=128;
    quint64 counter=0;
public:
    ZeroBlocksDetector(const SignatureDefInterface* parent);
    void run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs);
    void build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size);
    QString getDescription(const quint64 offset, QFile *file);
    quint64 granularity();
};

#endif // ZeroBlocksDETECTOR_H
