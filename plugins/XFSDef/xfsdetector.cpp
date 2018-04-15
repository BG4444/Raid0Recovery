#include "xfsdetector.h"
#include <QtEndian>
#include <QtWidgets/QPlainTextEdit>
//#include <xfs/xfs_format.h>


XFSDetector::XFSDetector(const SignatureDefInterface* parent):
                SignatureDetectorBase(parent,{'X','F','S','B',0,0,0x10,0})

{

}

void XFSDetector::build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size)
{
    QPlainTextEdit* edt=new QPlainTextEdit(parent);
//    xfs_sb* sb=reinterpret_cast<xfs_sb*>(base);

}


void XFSDetector::run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs)
{
    this->base=base;
    this->ofs=ofs;
    search(stopper,base,size,sgn,ofs);
}


void XFSDetector::onFound(const quint64 offset)
{
    const quint32 blockSize = qFromBigEndian(*reinterpret_cast<quint32*>( base+ (offset-ofs+4)  ));
    const quint64 fsSize = qFromBigEndian(*reinterpret_cast<quint64*>( base+ (offset-ofs+8)  ));

    const quint64 oneTb = quint64(1)<<(10*4);

    if(fsSize*blockSize > oneTb)
    {
        SignatureDetectorBase::onFound(offset);
    }
}

quint64 XFSDetector::granularity()
{
    return SignatureDetectorBase::granularity()+8;
}
