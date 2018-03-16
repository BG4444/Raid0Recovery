#ifndef PROGRESSSIGNALER_H
#define PROGRESSSIGNALER_H

#include <QObject>


class ProgressSignaler:public QObject
{
    Q_OBJECT
public:
    ProgressSignaler(QObject* parent);

    static ProgressSignaler* make(const quint64 count, QObject* parent);

public slots:
    virtual void tick()=0;
signals:
    void percent(int);
};


class ProgressSignalerGr100:public ProgressSignaler
{
    const quint64 count;
    const quint64 counterBase;
    int countOfAdds;
    quint64 pos;
    int percentPos;
public:
    ProgressSignalerGr100(quint64 count, QObject* parent);
    void tick();
};

class ProgressSignalerLs100:public ProgressSignaler
{
    const quint64 count;
    const quint64 counterBase;
    int countOfAdds;
    int percentPos;
public:
    ProgressSignalerLs100(quint64 count, QObject* parent);
    void tick();
};

#endif // PROGRESSSIGNALER_H
