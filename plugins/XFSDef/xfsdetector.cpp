#include "xfsdetector.h"


XFSDetector::XFSDetector(const uchar *base, const quint64 size):SignatureDetectorBase(base,size,{'X','F','S','B'})

{

}


void XFSDetector::run()
{
    search();
}
