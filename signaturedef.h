#ifndef SIGNATUREDEF_H
#define SIGNATUREDEF_H

#include <QRunnable>
#include <QObject>
#include <vector>

class ProgressSignaler;

class SignatureDef:public  QObject, public QRunnable
{
    Q_OBJECT
    const std::vector<uchar> sign;

    ProgressSignaler* sgn;

public:
    SignatureDef(const uchar *base, const quint64 size);
    const uchar* base;
    const quint64 size;
signals:
    void found(const uchar* offset);
    void percent(int);
    // QRunnable interface
public:
    void run();
};

#endif // SIGNATUREDEF_H
