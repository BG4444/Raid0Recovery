#include "zeroblocksdef.h"
#include "zeroblocksdetector.h"

SignatureDetector *ZeroBlocksDef::make()
{
    return new ZeroBlocksDetector(this);
}


void ZeroBlocksDef::configure(QWidget *parent,QSettings*)
{
}
