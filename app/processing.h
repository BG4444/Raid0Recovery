#ifndef PROCESSING_H
#define PROCESSING_H

#include <QRunnable>
#include <QObject>
#include <QEventLoop>

class ImagesList;

class Processing :public QObject, public QRunnable
{
    Q_OBJECT

    static constexpr size_t nAlgorithms=1;
    ImagesList* imgs;
public:
    Processing(ImagesList* imgs);

    // QRunnable interface
public:
    void run();
signals:
    void storeLog(const QString&);
    void taskReleased();
    void onTaskRelease();
};

#endif // PROCESSING_H
