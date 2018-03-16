#include "recoverwindow.h"
#include "ui_recoverwindow.h"

RecoverWindow::RecoverWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RecoverWindow)
{
    ui->setupUi(this);
}

RecoverWindow::~RecoverWindow()
{
    delete ui;
}
