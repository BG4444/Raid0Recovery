#include "xfsdetector.h"
#include <QtEndian>


XFSDetector::XFSDetector(const SignatureDefInterface* parent):
                SignatureDetectorBase(parent,{'X','F','S','B',0,0,0x10,00})

{

}

void XFSDetector::build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size)
{
}


void XFSDetector::run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs)
{
    this->base=base;
    this->ofs=ofs;
    search(stopper,base,size,sgn,ofs);
}


quint64 XFSDetector::granularity()
{
    return SignatureDetectorBase::granularity()+8;
}

void XFSDetector::onFound(const quint64 offset)
{
    const quint64 fsSize = qFromBigEndian(*reinterpret_cast<quint64*>( base+ (offset-ofs)  ));

    const quint64 oneTb = quint64(1)<<(10*4);

    if(fsSize*4096 > oneTb)
    {
        SignatureDetectorBase::onFound(offset);
    }
}
