#include "signaturedetectorbase.h"
#include "progresssignaler.h"

SignatureDetectorBase::SignatureDetectorBase(const SignatureDefInterface* parent, const uchar *base, const quint64 size,const std::vector<uchar>& sign):
    SignatureDetector(parent),
    sign(sign),
    sgn(ProgressSignaler::make(size,this)),
    base(base),
    size(size)


{
    connect(sgn,&ProgressSignaler::percent, this, &SignatureDetectorBase::percent);
}

void SignatureDetectorBase::search()
{
    auto j=sign.begin();

    for(quint64 i=0; i < size; ++i)
    {
        if(j==sign.end())
        {
            onFound(base+i-sign.size());
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

void SignatureDetectorBase::onFound(const uchar *offset)
{
    emit found(offset);
}
