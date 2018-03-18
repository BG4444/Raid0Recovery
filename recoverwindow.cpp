#include "imageslist.h"
#include "processing.h"
#include "progressdrawer.h"
#include "recoverwindow.h"
#include "ui_recoverwindow.h"

#include <QFileDialog>
#include <QThreadPool>
#include <imageslist.h>
#include <signaturedef.h>
#include <signaturelist.h>

#include <QMessageBox>
#include <QProgressBar>
#include <QStorageInfo>
#include <progresssignaler.h>

RecoverWindow::RecoverWindow(QWidget *parent) :
    QMainWindow(parent),
    dlg(new QFileDialog(this)),
    imgs(new ImagesList(this,sDetect)),
    tpool(new QThreadPool(this)),
    ui(new Ui::RecoverWindow),
    pBar(new QProgressBar(this)),
    drw(new ProgressDrawer(this))
{
    ui->setupUi(this);

    connect(this,&RecoverWindow::storeLog,ui->logStorage,&QPlainTextEdit::appendPlainText);

    ui->statusBar->addWidget(pBar);
    dlg->setFileMode(QFileDialog::ExistingFiles);

    connect(ui->addButton,&QToolButton::clicked,dlg,&QFileDialog::show);
    connect(dlg,&QFileDialog::filesSelected,imgs,&ImagesList::addImages);

    ui->imagesView->setModel(imgs);
    ui->imagesView->setItemDelegate(drw);


    connect(ui->scanButton,&QToolButton::clicked,this,&RecoverWindow::startScanning);

    ui->totalThreads->setValue( QThread::idealThreadCount() );


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
        procs.push_back(new Processing(imgs));
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

