#include "progressdrawer.h"
#include <QApplication>

ProgressDrawer::ProgressDrawer(QObject *parent):QStyledItemDelegate(parent)
{

}


void ProgressDrawer::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == progress_offset)
    {       

           const auto& vProgress=*reinterpret_cast<QVector<int>*>(index.data().toULongLong());

           const auto width=option.rect.width()/vProgress.size();

           QStyleOptionProgressBar progressBarOption;

           progressBarOption.textVisible = true;
           progressBarOption.minimum = 0;
           progressBarOption.maximum = 100;
           progressBarOption.rect = option.rect;

           progressBarOption.rect.setWidth(width);

           for(const auto& i:vProgress)

           {
               progressBarOption.progress = i;
               progressBarOption.text = QString::number(progressBarOption.progress).append('%');

               QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
               progressBarOption.rect.translate(width,0);

           }

     }
    else
    {
           QStyledItemDelegate::paint(painter, option, index);
    }
}
