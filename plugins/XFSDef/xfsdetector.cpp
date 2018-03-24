#include "xfsdetector.h"


XFSDetector::XFSDetector(const SignatureDefInterface* parent, const uchar *base, const quint64 size):
                SignatureDetectorBase(parent,base,size,{'X','F','S','B'})

{

}

void XFSDetector::run(const std::atomic<bool>& stopper)
{
    search(stopper);
}


void XFSDetector::build()
{
}
