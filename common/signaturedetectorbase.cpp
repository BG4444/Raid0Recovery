#include "signaturedetectorbase.h"
#include "progresssignaler.h"

SignatureDetectorBase::SignatureDetectorBase(const SignatureDefInterface* parent, const std::vector<uchar>& sign):
    sign(sign),
    SignatureDetector(parent)
{

}

void SignatureDetectorBase::search(const std::atomic<bool> &stopper, const uchar* base, const quint64 size,ProgressSignaler* sgn, const quint64 ofs)
{
    auto j=sign.begin();

    for(quint64 i=0; (i < size) && stopper; ++i)
    {
        if(j==sign.end())
        {
            onFound(i+ofs);
            j=sign.begin();
        }

        if(base[i] != *j)
        {
            if(j!=sign.begin())
            {
                j=sign.begin();
            }
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
