#include "progressdrawer.h"
#include <QApplication>

ProgressDrawer::ProgressDrawer(QObject *parent):QStyledItemDelegate(parent)
{

}


void ProgressDrawer::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == progress_offset)
    {       
           QStyleOptionProgressBar progressBarOption;


           progressBarOption.textVisible = true;

           progressBarOption.rect = option.rect;


           {
               progressBarOption.minimum = 0;
               progressBarOption.maximum = 100;
               progressBarOption.rect.setWidth(option.rect.width()/2);
               progressBarOption.progress = index.data().toPoint().x();
               progressBarOption.text = QString::number(progressBarOption.progress).append('%');

               QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
           }

           {
               progressBarOption.minimum = 0;
               progressBarOption.maximum = 2;
               progressBarOption.rect.translate(option.rect.width()/2,0);
               progressBarOption.progress = index.data().toPoint().y();
               progressBarOption.text = QString::number(progressBarOption.progress).append('%');

               QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
           }

     }
    else
    {
           QStyledItemDelegate::paint(painter, option, index);
    }
}
