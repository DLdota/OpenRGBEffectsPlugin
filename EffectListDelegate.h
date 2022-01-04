#ifndef EFFECTLISTDELEGATE_H
#define EFFECTLISTDELEGATE_H

#include <QItemDelegate>
#include <QPainter>

class EffectListDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
        explicit EffectListDelegate(QObject *parent = nullptr);
    protected:
        void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const;
        QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;
    };

#endif // EFFECTLISTDELEGATE_H
