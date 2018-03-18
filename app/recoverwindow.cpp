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



RecoverWindow::RecoverWindow(QWidget *parent) :
    QMainWindow(parent),
    dlg(new QFileDialog(this,tr("Open images"),cfg->value("lastDir").toString())),
    imgs(new ImagesList(this,sDetect)),
    tpool(new QThreadPool(this)),
    ui(new Ui::RecoverWindow),
    pBar(new QProgressBar(this)),
    drw(new ProgressDrawer(this)),
    cfg(new QSettings(this))
{
    ui->setupUi(this);

    connect(this,&RecoverWindow::storeLog,ui->logStorage,&QPlainTextEdit::appendPlainText);

    ui->statusBar->addWidget(pBar);

    dlg->setFileMode(QFileDialog::ExistingFiles);

    connect(ui->addButton,&QToolButton::clicked,dlg,&QFileDialog::show);
    connect(dlg,&QFileDialog::filesSelected,imgs,&ImagesList::addImages);
    connect(dlg,&QFileDialog::directoryEntered,this,&RecoverWindow::storeDir);

    ui->imagesView->setModel(imgs);
    ui->imagesView->setItemDelegate(drw);


    connect(ui->scanButton,&QToolButton::clicked,this,&RecoverWindow::startScanning);





    for(const auto&i:sDetect)
    {
        storeLog(i.first.displayName());
    }

    QPluginLoader ldr("/home/bg/Projects/Raid0Recovery/plugins/build-XFSDef-qt5_linux-Debug/XFSDef");


    const auto t= qobject_cast<SignatureDefInterface*>(ldr.instance());

    if(t)
    {
        storeLog(QString(tr("%1 plugin loaded")).arg(ldr.fileName()));
        signatureDetectors.push_back(t);
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

    cfg->setValue(ctrl->objectName(),ctrl->value());
}

void RecoverWindow::storeDir(const QString &dir)
{
    cfg->setValue("lastDir",dir);
}

