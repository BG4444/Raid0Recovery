#include "xfsdef.h"
#include "xfsdetector.h"

SignatureDetector *XFSDef::make(const uchar *offset, const quint64 size)
{
    return new XFSDetector( this, offset,size);
}
