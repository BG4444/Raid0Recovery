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

    for(const auto&i: dirs)
    {

        QPluginLoader ldr(i.absoluteFilePath());

        const auto t= qobject_cast<SignatureDefInterface*>(ldr.instance());

        if(t)
        {
            storeLog(QString(tr("%1 plugin loaded")).arg(i.fileName()));
            signatureDetectors.push_back(t);
        }
    }
}

RecoverWindow::RecoverWindow(QWidget *parent) :
    QMainWindow(parent),
    dlg(new QFileDialog(this,tr("Open images"),cfg->value("lastDir").toString())),

    tpool(new QThreadPool(this)),
    ui(new Ui::RecoverWindow),
    pBar(new QProgressBar(this)),
    drw(new ProgressDrawer(this)),
    cfg(new QSettings(this))
{
    ui->setupUi(this);

    ASSERT(connect(this,&RecoverWindow::storeLog,ui->logStorage,&QPlainTextEdit::appendPlainText));

    loadPlugins();

    imgs=new ImagesList(this,sDetect,signatureDetectors.size());

    ui->statusBar->addWidget(pBar);


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


    connect(ui->scanButton,&QToolButton::clicked,this,&RecoverWindow::startScanning);

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

void RecoverWindow::startScanning()
{
    std::vector<Processing*> procs;

    procs.reserve(ui->totalThreads->value());

    for(int i=0;i<ui->totalThreads->value();++i)
    {
        procs.push_back(new Processing(imgs,signatureDetectors));
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

void RecoverWindow::paramChanged()
{
    const auto ctrl=qobject_cast<QSpinBox*>(sender());

    ASSERT(ctrl);

    cfg->setValue(ctrl->objectName(),ctrl->value());
}

void RecoverWindow::storeDir(const QString &dir)
{
    cfg->setValue("lastDir",dir);
}

