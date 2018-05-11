#include "xfsdef.h"
#include "xfsdetector.h"

SignatureDetector *XFSDef::make()
{
    return new XFSDetector( this);
}


void XFSDef::configure(QWidget *parent, QSettings *set)
{
}
