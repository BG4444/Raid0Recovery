#ifndef PROGRESSSIGNALER_H
#define PROGRESSSIGNALER_H

#include <QObject>

class ProgressSignaler : public QObject
{
    Q_OBJECT
public:
    explicit ProgressSignaler(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PROGRESSSIGNALER_H