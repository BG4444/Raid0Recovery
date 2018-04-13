#include "xfsdef.h"
#include "xfsdetector.h"

SignatureDetector *XFSDef::make()
{
    return new XFSDetector( this);
}
