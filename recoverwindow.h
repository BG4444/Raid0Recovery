#ifndef RECOVERWINDOW_H
#define RECOVERWINDOW_H

#include <QMainWindow>

namespace Ui {
class RecoverWindow;
}

class RecoverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecoverWindow(QWidget *parent = 0);
    ~RecoverWindow();

private:
    Ui::RecoverWindow *ui;
};

#endif // RECOVERWINDOW_H
