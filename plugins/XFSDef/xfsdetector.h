#ifndef XFSDETECTOR_H
#define XFSDETECTOR_H

#include "signaturedefinterface.h"
#include "signaturedetectorbase.h"

class XFSDetector : public SignatureDetectorBase
{
public:
    XFSDetector(const uchar* base, const quint64 size);
public:
    void run();
};

#endif // XFSDETECTOR_H
