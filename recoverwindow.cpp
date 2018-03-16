#include "imageslist.h"
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
    imgs(new ImagesList(this)),
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


    tpool->setMaxThreadCount(1);


    connect(ui->scanButton,&QToolButton::clicked,this,&RecoverWindow::startScanning);


    QStorageInfo nfo;

    const auto t=QStorageInfo::mountedVolumes();

    for(const auto&i:t)
    {
        storeLog(i.displayName());
    }

    storeLog(tr("Ready!"));
}

RecoverWindow::~RecoverWindow()
{
    delete ui;
}

void RecoverWindow::startScanning()
{
    const auto rows = ui->imagesView->selectionModel()->selectedRows();

    for(const auto& i:rows)
    {
        const auto t=(*imgs)[i.row()];

        const auto def = new SignatureDef(t->base,t->size);

        connect(def,&SignatureDef::found,t,&SignatureList::registerSignature);
        connect(def,&SignatureDef::percent,pBar,&QProgressBar::setValue);

        tpool->start(def);
    }
}

