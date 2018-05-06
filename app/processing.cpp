#include "imageslist.h"
#include "processing.h"
#include "signaturelist.h"
#include <QThread>


Processing::Processing(ImagesList *imgs,std::function<void(ImageInfo*,QFile*,bool&, bool&)> apply):imgs(imgs),apply(apply)
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

                    apply(cur.get(),file,workDone,allDone);

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

