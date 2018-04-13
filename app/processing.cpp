#include "imageslist.h"
#include "processing.h"
#include "signaturelist.h"
#include <QThread>
#include <progresssignaler.h>


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
            QFile* file=const_cast<QFile*>(&i.first);
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




                        const auto def =  qobject_cast<SignatureDefInterface*> (dets[algorithm] ->instance()) ->make();

                        connect(def,&SignatureDetector::found,cur->signatures.get(),static_cast<void (SignatureList::*)(const quint64)>(&SignatureList::registerSignature));



                        quint64 fsize=file->size();

                        auto signaler=ProgressSignaler::make(fsize,def);

                        connect(signaler,&ProgressSignaler::percent,[&cur,algorithm](const int percent)
                                                                {
                                                                    cur->setProgress(algorithm, percent);
                                                                }                          );

                        constexpr quint64 bufsz=(1<<20)*100;

                        const quint64 gran=def->granularity();

                        const quint64 bufcorrected= bufsz - (bufsz % gran);


                        for(quint64 pos = 0; const size_t buffermin=std::min(bufcorrected, fsize-pos ); )
                        {
                            const auto buf=file->map(pos, buffermin);

                            def->run(stopper,buf,buffermin,signaler,pos);

                            file->unmap(buf);

                            pos+=buffermin;

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
