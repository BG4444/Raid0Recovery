#ifndef XFSDETECTOR_H
#define XFSDETECTOR_H

#include "signaturedefinterface.h"
#include "signaturedetectorbase.h"

class XFSDetector : public SignatureDetectorBase
{
public:
    XFSDetector(const SignatureDefInterface* parent);
public:
    void run(const std::atomic<bool>& stopper, const uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs);

    // SignatureDetector interface
public:

    void build(QWidget *parent, const qulonglong stripeSize);

};

#endif // XFSDETECTOR_H

