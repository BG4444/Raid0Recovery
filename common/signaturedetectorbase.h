#ifndef SIGNATUREDETECTORBASE_H
#define SIGNATUREDETECTORBASE_H

#include <QObject>
#include "signaturedefinterface.h"

class ProgressSignaler;

using vSign=std::vector<uchar>;

class SignatureDetectorBase:public SignatureDetector
{
    vSign sign;
public:
    SignatureDetectorBase(const SignatureDefInterface *parent, const vSign& sign);
    void search(const std::atomic<bool>& stopper, const uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs);
    virtual void onFound(const quint64 offset);
    quint64 granularity();
};

#endif // SIGNATUREDETECTORBASE_H
