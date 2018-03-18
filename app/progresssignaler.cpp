#include "progresssignaler.h"


ProgressSignaler::ProgressSignaler(QObject *parent):QObject(parent)
{

}

ProgressSignaler *ProgressSignaler::make(const quint64 count, QObject *parent)
{
    if(count >= 100)
    {
        return new ProgressSignalerGr100(count,parent);
    }
    return new ProgressSignalerLs100(count,parent);
}

ProgressSignalerGr100::ProgressSignalerGr100(quint64 count, QObject *parent):
    ProgressSignaler(parent),
    count(count),
    counterBase(count / 100),
    countOfAdds(count % 100),
    pos(counterBase + bool(countOfAdds)),
    percentPos(0)
{
}

void ProgressSignalerGr100::tick()
{
    if(--pos == 0)
    {
        pos = counterBase + bool (--countOfAdds > 0);
        emit percent(++percentPos);
    }
}

ProgressSignalerLs100::ProgressSignalerLs100(quint64 count, QObject *parent):
    ProgressSignaler(parent),
    count(count),
    counterBase(100 / count),
    countOfAdds(100 % count),
    percentPos(0)
{
}

void ProgressSignalerLs100::tick()
{
    percentPos += counterBase + bool (--countOfAdds >= 0);
    emit percent(percentPos);
}
