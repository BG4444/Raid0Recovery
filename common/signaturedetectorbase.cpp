#include "signaturedetectorbase.h"
#include "progresssignaler.h"

SignatureDetectorBase::SignatureDetectorBase(const SignatureDefInterface* parent, const vSign &sign):
    sign(sign),
    SignatureDetector(parent)
{

}

void SignatureDetectorBase::search(const std::atomic<bool> &stopper, const uchar* base, const quint64 size,ProgressSignaler* sgn, const quint64 ofs)
{
    size_t j=0;
    const size_t ss=sign.size();

    for(quint64 i=0; (i < size) && stopper; ++i)
    {
        if(j==ss)
        {
            onFound(i-j+ofs);
            j=0;
        }

        if(base[i] != sign[j])
        {
            j=0;
        }
        else
        {
            ++j;
        }
        sgn->tick();
    }
}

void SignatureDetectorBase::onFound(const quint64 offset)
{
    emit found(offset);
}


quint64 SignatureDetectorBase::granularity()
{
    return sign.size();
}
