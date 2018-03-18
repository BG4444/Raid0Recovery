#ifndef SIGNATUREDETECTORBASE_H
#define SIGNATUREDETECTORBASE_H

#include <QObject>
#include "signaturedefinterface.h"

class ProgressSignaler;

class SignatureDetectorBase:public SignatureDetector
{
    std::vector<uchar> sign;
    ProgressSignaler* sgn;
    const uchar* base;
    const quint64 size;
public:
    SignatureDetectorBase(const uchar *base, const quint64 size,const std::vector<uchar>& sign);
    void search();
    virtual void onFound(const uchar* offset);
};

#endif // SIGNATUREDETECTORBASE_H
