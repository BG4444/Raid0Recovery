#include "xfsdef.h"
#include "xfsdetector.h"

SignatureDetector *XFSDef::make(uchar *offset, quint64 size)
{
    return new XFSDetector(offset,size);
}
