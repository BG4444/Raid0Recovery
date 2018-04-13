#ifndef SIGNATUREDETECTORBASE_H
#define SIGNATUREDETECTORBASE_H

#include <QObject>
#include "signaturedefinterface.h"

class ProgressSignaler;

class SignatureDetectorBase:public SignatureDetector
{
    std::vector<uchar> sign;    
public:
    SignatureDetectorBase(const SignatureDefInterface *parent, const std::vector<uchar>& sign);
    void search(const std::atomic<bool>& stopper, const uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs);
    virtual void onFound(const quint64 offset);

    // SignatureDetector interface
public:
    quint64 granularity();
};

#endif // SIGNATUREDETECTORBASE_H
