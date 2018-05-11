#include "zeroblocksdetector.h"

#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPainter>
#include <QVariant>
#include <iostream>
#include <progresssignaler.h>
#include <qendian.h>

#include <QtWidgets/QWidget>
#include <QGraphicsScene>


ZeroBlocksDetector::ZeroBlocksDetector(const SignatureDefInterface* parent):SignatureDetectorBase(parent,vSign())

{

}

void ZeroBlocksDetector::build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size)
{

}


void ZeroBlocksDetector::run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs)
{    
    for(quint64 i=0; (i < size) && stopper; ++i)
    {
        if(base[i])
        {
            if(counter>=gran)
            {
                onFound(i-counter+ofs);
            }
            counter=0;
        }
        else
        {
            ++counter;
        }
        sgn->tick();
    }
}


QString ZeroBlocksDetector::getDescription(const quint64 offset, QFile *file)
{
    return QString();
}


quint64 ZeroBlocksDetector::granularity()
{
    return gran;
}
