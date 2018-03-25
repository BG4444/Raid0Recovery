#include "imageslist.h"
#include "processing.h"
#include "progressdrawer.h"
#include "recoverwindow.h"
#include "ui_recoverwindow.h"

#include <QFileDialog>
#include <QThreadPool>
#include <imageslist.h>

#include <signaturelist.h>

#include <QMessageBox>
#include <QPluginLoader>
#include <QProgressBar>
#include <QSettings>
#include <QStorageInfo>
#include <QSpinBox>
#include <QMetaClassInfo>
#include "macroses.h"

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
            signatureDetectors.insert(std::make_pair(ldr,i));
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

    imgs=new ImagesList(this,sDetect,signatureDetectors);


    ui->statusBar->addWidget(pBar);

    connect(imgs,&ImagesList::ProgressChanged,pBar,&QProgressBar::setValue);
    connect(imgs,&ImagesList::rowsInserted,this,&RecoverWindow::updateProgressMaximum);


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




    ui->imagesView->setModel(imgs);
    ui->imagesView->setItemDelegate(drw);

    connect(ui->imagesView,&QTableView::clicked,this,&RecoverWindow::tableClicked);


    connect(ui->scanButton,&QToolButton::clicked,this,&RecoverWindow::startScanning);

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

void RecoverWindow::doScan()
{
    stopper=true;
    std::vector<Processing*> procs;

    procs.reserve(ui->totalThreads->value());

    for(int i=0;i<ui->totalThreads->value();++i)
    {
        procs.push_back(new Processing(imgs,signatureDetectors, stopper));
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
        doScan();
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
                glue.reset( new FileGlue(imgs->size(),{imgs->findSignatureList(lst)}));
            }

            QString str;
            for(const auto&i:glue->insertPersistent())
            {
                str+=QString::number(i)+' ';
            }
            storeLog(str);

            lst->build( rows[0].row(),ui->buildWidget,*glue.get());
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

