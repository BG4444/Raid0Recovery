#ifndef XFSDETECTOR_H
#define XFSDETECTOR_H

#include "signaturedefinterface.h"
#include "signaturedetectorbase.h"


class XFSDetector : public SignatureDetectorBase
{
    uchar* base;
    quint64 ofs;
public:
    XFSDetector(const SignatureDefInterface* parent);
    void build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size);

    // SignatureDetector interface
public:
    void run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs);

    // SignatureDetector interface
    // SignatureDetectorBase interface
public:
    void onFound(const quint64 offset);

    // SignatureDetector interface
public:
    quint64 granularity();
};

#endif // XFSDETECTOR_H

