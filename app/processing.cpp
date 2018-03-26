#include "imageslist.h"
#include "processing.h"
#include "signaturelist.h"
#include <QThread>



Processing::Processing(ImagesList *imgs, const vDetectors& dets,const std::atomic<bool> &stopper):imgs(imgs),dets(dets),stopper(stopper)
{

}


void Processing::run()
{
    for(;;)
    {
        bool haveLocked=false;
        bool allDone=true;
        for(auto&i:*imgs)
        {
            const auto& cur=i.second;
            bool workDone=false;
            if(cur->tryAcquire())
            {
                emit storeLog(QString(tr("%1 file locked")).arg(i.first.fileName()));
                if(cur->storage->second->tryAcquire())
                {
                    emit storeLog(QString(tr("%1 drive locked")).arg(i.first.fileName()));

                    size_t algorithm;

                    if(cur->nUsedAlgorithms < dets.size() && ( algorithm = cur->nUsedAlgorithms++) < dets.size() )
                    {

                        emit storeLog(QString(tr("new search started for file %1")).arg(i.first.fileName()));




                        const auto def =  qobject_cast<SignatureDefInterface*> (dets[algorithm] ->instance()) ->make(cur->base,cur->size);

                        connect(def,&SignatureDetector::found,cur->signatures.get(),static_cast<void (SignatureList::*)(const uchar*)>(&SignatureList::registerSignature));



                        connect(def,&SignatureDetector::percent,[&cur,algorithm](const int percent)
                                                                {
                                                                    cur->setProgress(algorithm, percent);
                                                                }                          );

                        for(size_t j=0;j!=100;++j)
                        {
                            try
                            {
                                def->run(stopper);
                            }
                            catch(std::exception& ex)
                            {
                                storeLog(tr("task failed with exception, num retries %1 what is %2").arg(j).arg(ex.what()));
                                QThread::sleep(1);
                            }
                            catch(...)
                            {
                                storeLog(tr("task failed with exception, num retries %1").arg(j));
                                QThread::sleep(1);
                            }

                        }

                        def->deleteLater();

                        workDone=true;
                        allDone=false;

                        emit storeLog(QString(tr("search done for file %1")).arg(i.first.fileName()));

                    }
                    cur->storage->second->release();
                }
                else
                {
                    emit storeLog(QString(tr("%1 drive locked")).arg(i.first.fileName()));
                    haveLocked=true;
                }

                cur->release();
                emit storeLog(QString(tr("%1 file unlocked")).arg(i.first.fileName()));

                if(workDone)
                {
                    emit taskReleased();
                }
            }
            else
            {
                emit storeLog(QString(tr("%1 file lock failed")).arg(i.first.fileName()));
                haveLocked=true;
            }
        }
        if(!haveLocked && allDone)
        {
            break;
        }
        emit storeLog(QString(tr("no free task, waiting")));

        QEventLoop loop;
        connect(this,&Processing::onTaskRelease,&loop,&QEventLoop::quit);

        loop.exec();
        emit storeLog(QString(tr("waiting complete")));
    }
}
