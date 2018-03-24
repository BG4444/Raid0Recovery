#ifndef XFSDETECTOR_H
#define XFSDETECTOR_H

#include "signaturedefinterface.h"
#include "signaturedetectorbase.h"

class XFSDetector : public SignatureDetectorBase
{
public:
    XFSDetector(const SignatureDefInterface* parent,const uchar* base, const quint64 size);
public:
    void run(const std::atomic<bool>& stopper);

    // SignatureDetector interface
public:
    void build();
};

#endif // XFSDETECTOR_H

