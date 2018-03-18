#ifndef XFSDETECTOR_H
#define XFSDETECTOR_H

#include "signaturedefinterface.h"
#include "progresssignaler.h"

class XFSDetector :public SignatureDetector
{
    std::vector<uchar> sign;
    ProgressSignaler* sgn;
    const uchar* base;
    const quint64 size;
public:
    XFSDetector(const uchar* base, const quint64 size);

    // SignatureDetector interface
public:
    void run();
};

#endif // XFSDETECTOR_H
