#ifndef PROGRESSDRAWER_H
#define PROGRESSDRAWER_H

#include <QStyledItemDelegate>


class ProgressDrawer : public QStyledItemDelegate
{
      static constexpr size_t progress_offset=3;
public:
    ProgressDrawer(QObject* parent);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PROGRESSDRAWER_H
