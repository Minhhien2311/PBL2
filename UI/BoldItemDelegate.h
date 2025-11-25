#ifndef BOLDITEMDELEGATE_H
#define BOLDITEMDELEGATE_H

#include <QStyledItemDelegate>

// Class này giúp can thiệp vào quá trình vẽ của bảng
class BoldItemDelegate : public QStyledItemDelegate {
public:
    // Kế thừa constructor của lớp cha cho gọn
    using QStyledItemDelegate::QStyledItemDelegate;

    // Hàm vẽ lại ô (ghi đè từ lớp cha)
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        
        // LOGIC CHÍNH: Nếu dòng này đang được chọn (Selected) -> Ép font in đậm
        if (opt.state & QStyle::State_Selected) {
            // Mức 1: DemiBold (Hơi đậm - Weight 600) -> Khuyên dùng
            opt.font.setWeight(QFont::DemiBold); 
            
            // Mức 2: Medium (Đậm vừa phải - Weight 500)
            // opt.font.setWeight(QFont::Medium);
        }
        
        // Gọi hàm vẽ mặc định của Qt nhưng với font đã chỉnh sửa
        QStyledItemDelegate::paint(painter, opt, index);
    }
};

#endif // BOLDITEMDELEGATE_H