#include "xfsdetector.h"


XFSDetector::XFSDetector(const SignatureDefInterface* parent):
                SignatureDetectorBase(parent,{'X','F','S','B'})

{

}

void XFSDetector::run(const std::atomic<bool>& stopper, const uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs)
{
    search(stopper,base,size,sgn,ofs);
}

void XFSDetector::build(QWidget *parent, const qulonglong stripeSize)
{
}

