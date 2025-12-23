#ifndef BOLDITEMDELEGATE_H
#define BOLDITEMDELEGATE_H

#include <QStyledItemDelegate>

class BoldItemDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        
        if (opt.state & QStyle::State_Selected) {
            opt.font.setWeight(QFont::DemiBold);
        }
        
        QStyledItemDelegate::paint(painter, opt, index);
    }
};

#endif