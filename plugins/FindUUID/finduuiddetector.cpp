#include "finduuiddetector.h"

#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPainter>
#include <QVariant>
#include <iostream>
#include <progresssignaler.h>
#include <qendian.h>

#include <QtWidgets/QWidget>
#include <QGraphicsScene>


FindUUIDDetector::FindUUIDDetector(const SignatureDefInterface* parent, const vSign &sign):SignatureDetectorBase(parent,sign)

{

}

void FindUUIDDetector::build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size)
{

}


void FindUUIDDetector::run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs)
{    
    search(stopper,base,size,sgn,ofs);
}


QString FindUUIDDetector::getDescription(const quint64 offset, QFile *file)
{
    return QString();
}
