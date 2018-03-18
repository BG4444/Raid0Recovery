#include "imageslist.h"
#include "processing.h"
#include "signaturedef.h"
#include "signaturelist.h"




Processing::Processing(ImagesList *imgs):imgs(imgs)
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
            ImageInfo* cur=i.second;
            bool workDone=false;
            if(cur->sem->tryAcquire())
            {
                emit storeLog(QString(tr("%1 file locked")).arg(i.first.fileName()));
                if(cur->storage->second->tryAcquire())
                {
                    emit storeLog(QString(tr("%1 drive locked")).arg(i.first.fileName()));
                    if(cur->nUsedAlgorithms < nAlgorithms && cur->nUsedAlgorithms++ < nAlgorithms )
                    {

                        emit storeLog(QString(tr("new search started for file %1")).arg(i.first.fileName()));

                        const auto def = new SignatureDef(cur->base,cur->size);

                        connect(def,&SignatureDef::found,cur->signatures,&SignatureList::registerSignature);
                        connect(def,&SignatureDef::percent,cur,&ImageInfo::setProgress);

                        def->run();

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

                cur->sem->release();
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
