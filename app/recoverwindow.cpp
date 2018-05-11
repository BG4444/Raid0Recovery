#include "imageslist.h"
#include "processing.h"
#include "progressdrawer.h"
#include "recoverwindow.h"
#include "ui_recoverwindow.h"

#include <QFileDialog>
#include <QThreadPool>
#include <imageslist.h>

#include <QCheckBox>
#include <signaturelist.h>

#include <QMessageBox>
#include <QPluginLoader>
#include <QProgressBar>
#include <QSettings>
#include <QStorageInfo>
#include <QSpinBox>
#include <QMetaClassInfo>
#include "macroses.h"
#include <progresssignaler.h>
#include <QCryptographicHash>

void RecoverWindow::loadSpinBoxSetting(QSpinBox *box, const int defValue)
{
    box->setValue( cfg->value(box->objectName(),defValue).toInt() );
    ASSERT(connect(box,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   this, &RecoverWindow::paramChanged));
}

void RecoverWindow::loadPlugins()
{
    QDir dir(QApplication::applicationDirPath());

    if (  !dir.cd("plugins") )
    {
        storeLog(tr("could not cd to plugins dir!"));
        return;
    }

    storeLog(tr("loading plugins from %1").arg(dir.absolutePath()));

    const auto& dirs=dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::Files);

    for(int i=0;i!=dirs.size();++i)
    {

        QPluginLoader* ldr=new QPluginLoader(dirs[i].absoluteFilePath(),this);

        const QObject* obj=ldr->instance();

        const auto t= qobject_cast<SignatureDefInterface*>(obj);

        if(t)
        {
            storeLog(QString(tr("%1 plugin loaded")).arg(dirs[i].fileName()));

            const auto pos=signatureDetectors.size();


            const auto twidget=new QWidget(ui->pluginsSettings);
            const auto name=ldr->metaData().find("className").value().toString();

            ui->pluginsSettings->insertTab(pos,twidget,name);

            const auto vbox=new QVBoxLayout(twidget);
            const auto confWidget=new QWidget(twidget);
            vbox->addWidget(confWidget);

            const auto enableCheck=new QCheckBox(tr("Enabled"),twidget);

            enableCheck->setChecked(cfg->value(QString("%1_enabled").arg(name),true).toBool());

            connect(enableCheck,&QCheckBox::clicked,this,&RecoverWindow::enableChanged);

            vbox->addWidget(enableCheck);

            t->configure(confWidget,cfg);

            signatureDetectors.insert(std::make_pair(ldr,pluginInfo(pos,enableCheck)));
        }
    }
}

RecoverWindow::RecoverWindow(QWidget *parent) :
    QMainWindow(parent),
    cfg(new QSettings(this)),
    dlg(new QFileDialog(this,tr("Open images"),cfg->value("lastDir").toString())),       
    tpool(new QThreadPool(this)),
    pBar(new QProgressBar(this)),
    drw(new ProgressDrawer(this)),
    ui(new Ui::RecoverWindow)
{
    ui->setupUi(this);

    ASSERT(connect(this,&RecoverWindow::storeLog,ui->logStorage,&QPlainTextEdit::appendPlainText));

    loadPlugins();

    ui->detectorForCompare->setModel(&signatureDetectors);

    imgs=new ImagesList(this,sDetect,signatureDetectors);


    ui->statusBar->addWidget(pBar);

    connect(imgs,&ImagesList::ProgressChanged,pBar,&QProgressBar::setValue);
    connect(imgs,&ImagesList::rowsInserted,this,&RecoverWindow::updateProgressMaximum);
    connect(imgs,&ImagesList::storeLog,this,&RecoverWindow::storeLog);


    //обвязка файлового диалога
    dlg->setFileMode(QFileDialog::ExistingFiles);

    connect(ui->addButton,&QToolButton::clicked,dlg,&QFileDialog::show);
    connect(dlg,&QFileDialog::filesSelected,imgs,&ImagesList::addImages);
    connect(dlg,&QFileDialog::directoryEntered,this,&RecoverWindow::storeDir);


    connect(ui->threadsPerDrive,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   &sDetect, &StorageDetector::setThreadCount);
    connect(ui->threadsPerFile,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   imgs, &ImagesList::onSetThreadCount);

    loadSpinBoxSetting(ui->totalThreads,QThread::idealThreadCount());
    loadSpinBoxSetting(ui->threadsPerDrive,1);
    loadSpinBoxSetting(ui->threadsPerFile,1);
    loadSpinBoxSetting(ui->chunkSize,128*1024);




    ui->imagesView->setModel(imgs);
    ui->imagesView->setItemDelegate(drw);

    connect(ui->imagesView,&QTableView::clicked,this,&RecoverWindow::tableClicked);


    connect(ui->scanButton,&QToolButton::clicked,this,&RecoverWindow::startScanning);
    connect(ui->compareButton,&QToolButton::clicked,this,&RecoverWindow::startHashing);

    connect(ui->storeButton,&QToolButton::clicked,this,&RecoverWindow::store);
    connect(ui->loadButton,&QToolButton::clicked,this,&RecoverWindow::load);
    connect(ui->clearButton,&QToolButton::clicked,imgs,&ImagesList::clear);
    connect(ui->buildButton,&QToolButton::clicked,this,&RecoverWindow::build);
    connect(ui->rollButton,&QToolButton::clicked,this,&RecoverWindow::roll);
    connect(ui->fixButton,&QToolButton::clicked,this,&RecoverWindow::fix);

    connect(this,&RecoverWindow::doBuild,this,&RecoverWindow::build,Qt::QueuedConnection);

    for(const auto&i:sDetect)
    {
        storeLog(i.first.displayName());
    }

    storeLog(tr("Ready!"));
}

RecoverWindow::~RecoverWindow()
{
    delete ui;
}

void RecoverWindow::doScan(Applicator apply)
{
    imgs->resetCapturedParts();
    stopper=true;
    std::vector<Processing*> procs;

    procs.reserve(ui->totalThreads->value());

    for(int i=0;i<ui->totalThreads->value();++i)
    {
        procs.push_back(new Processing(imgs, apply  ));
    }

    storeLog(tr("tasks created"));

    for(const auto&i:procs)
    {
        connect(i,&Processing::storeLog,this,&RecoverWindow::storeLog,Qt::QueuedConnection);
        for(const auto&j:procs)
        {
            if(i!=j)
            {
                connect(i,&Processing::taskReleased,j,&Processing::onTaskRelease,Qt::QueuedConnection);
            }
        }
    }

    storeLog(tr("tasks connected"));

    for(const auto&i:procs)
    {
        tpool->start(i);
    }

    storeLog(tr("tasks started"));
}

void RecoverWindow::doStop()
{
    stopper=false;
}

void RecoverWindow::startScanning(const bool toggle)
{
    if(toggle)
    {
        doScan([this](ImageInfo* cur, QFile* file, bool& workDone, bool& allDone)
        {
            size_t algorithm;

            if(cur->nUsedAlgorithms < signatureDetectors.size() && ( algorithm = cur->nUsedAlgorithms++) < signatureDetectors.size() )
            {

                emit storeLog(QString(tr("new search started for file %1")).arg(file->fileName()));

                QPluginLoader* plugin=signatureDetectors[algorithm];

                const QCheckBox* isEnabled=signatureDetectors.find(plugin)->second.enabled;

                if(isEnabled->isChecked())
                {

                    const auto def =  qobject_cast<SignatureDefInterface*> (plugin->instance()) ->make();

                    connect(def,&SignatureDetector::found,cur->signatures.get(),static_cast<void (SignatureList::*)(const quint64)>(&SignatureList::registerSignature));



                    quint64 fsize=file->size();

                    auto signaler=ProgressSignaler::make(fsize,def);

                    connect(signaler,&ProgressSignaler::percent,[&cur,algorithm](const int percent)
                                                            {
                                                                cur->setProgress(algorithm, percent);
                                                            }                          );

                    constexpr quint64 bufsz=(1<<20)*100;

                    const quint64 gran=def->granularity();

                    const quint64 bufcorrected= bufsz - (bufsz % gran); //buffer size


                    for(quint64 pos = 0; const quint64 buffermin=std::min(bufcorrected, fsize-pos ); pos+=buffermin)
                    {
                        const auto buf=file->map(pos, buffermin);

                        def->run(stopper,buf,buffermin,signaler,pos);

                        file->unmap(buf);
                    }


                    def->deleteLater();
                }

                workDone=true;
                allDone=false;

                emit storeLog(QString(tr("search done for file %1")).arg(file->fileName()));

            }
        });
    }
    else
    {
        doStop();
    }
}

void RecoverWindow::startHashing(const bool toggle)
{
    if(toggle)
    {
        doScan([this](ImageInfo* cur, QFile* file, bool& workDone, bool& allDone)
        {
            size_t algorithm;
            const size_t nAlg=ui->detectorForCompare->currentIndex();
            const auto currentDetector=reinterpret_cast<QPluginLoader*>(ui->detectorForCompare->currentData().toULongLong());

            const size_t countOfFindings=cur->signatures->count( currentDetector );

            if(cur->nUsedAlgorithms < countOfFindings && ( algorithm = cur->nUsedAlgorithms++) < countOfFindings )
            {




                auto pos=cur->signatures->allOf(currentDetector).first;

                for(size_t i=0;i<algorithm;++i)
                {
                    ++pos;

                }

                QCryptographicHash hs(QCryptographicHash::Sha256);

                const auto mem=file->map( pos->second.offset,ui->compareSize->value());

                hs.addData(reinterpret_cast<const char*>(mem),ui->compareSize->value());

                file->unmap(mem);

                pos->second.hash=hs.result();


                workDone=true;
                allDone=false;


            }
        });
    }
    else
    {
        doStop();
    }
}

void RecoverWindow::paramChanged()
{
    const auto ctrl=qobject_cast<QSpinBox*>(sender());

    ASSERT(ctrl);

    cfg->setValue(ctrl->objectName(),ctrl->value());  
    updateProgressMaximum();
}

void RecoverWindow::storeDir(const QString &dir)
{
    cfg->setValue("lastDir",dir);
}

void RecoverWindow::updateProgressMaximum()
{
    pBar->setMaximum(100 * signatureDetectors.size() * imgs->rowCount(QModelIndex()) );
}

void RecoverWindow::store()
{
    QFile out("out.r0r");

    out.open(QFile::WriteOnly);

    QDataStream str(&out);

    str<<*imgs;
}

void RecoverWindow::load()
{
    QFile in("out.r0r");

    in.open(QFile::ReadOnly);

    QDataStream str(&in);

    str>>*imgs;
}

void RecoverWindow::tableClicked(const QModelIndex &idx)
{
    ui->findingsView->setModel((*imgs)[idx.row()]);
}

void RecoverWindow::build()
{
    const auto rows=ui->findingsView->selectionModel()->selectedRows();

    ui->tabWidget->setCurrentIndex(1);





    if(rows.size()==1)
    {
        auto lst=qobject_cast<const SignatureList*>(rows[0].model());

        if(lst)
        {
            if(!glue.get())
            {
                glue.reset( new FileGlue(imgs->size(),{static_cast<size_t>(imgs->findSignatureList(lst))}));
            }

            QString str;
            for(const auto&i:glue->insertPersistent())
            {
                str+=QString::number(i)+' ';
            }
            storeLog(str);

            lst->build( rows[0].row(),ui->buildWidget,*glue.get(),ui->chunkSize->value());
        }
    }
}

void RecoverWindow::roll()
{
    if(glue.get())
    {
        glue->ror();
    }
    const auto chld=ui->buildWidget->children();
    for(const auto& i:chld)
    {
        i->deleteLater();
    }
    emit doBuild();
}

void RecoverWindow::fix()
{
    if(const auto oldGlue=glue.get())
    {
        const auto newPersistent=oldGlue->fix();

        glue.reset(new FileGlue(imgs->size(),newPersistent));
    }
}

void RecoverWindow::enableChanged(bool value)
{
    auto box=qobject_cast<QCheckBox*>(sender());
    ASSERT(box);
    const auto pos=std::find_if(signatureDetectors.begin(),signatureDetectors.end(),[box](vDetectors::value_type& cur)
    {
        return cur.second.enabled==box;
    }
    );
    ASSERT(pos!=signatureDetectors.end());
    QPluginLoader* ldr=pos->first;
    const auto name=ldr->metaData().find("className").value().toString();
    cfg->setValue(QString("%1_enabled").arg(name),value);
}

