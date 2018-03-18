#include "progressdrawer.h"
#include <QApplication>

ProgressDrawer::ProgressDrawer(QObject *parent):QStyledItemDelegate(parent)
{

}


void ProgressDrawer::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == progress_offset)
    {
           const int progress = index.data().toInt();

           QStyleOptionProgressBar progressBarOption;

           progressBarOption.progress = progress;

           progressBarOption.rect = option.rect;
           progressBarOption.minimum = 0;
           progressBarOption.maximum = 100;
           progressBarOption.text = QString::number(progress) + "%";
           progressBarOption.textVisible = true;

           QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
     }
    else
    {
           QStyledItemDelegate::paint(painter, option, index);
    }
}
