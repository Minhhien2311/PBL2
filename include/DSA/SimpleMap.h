#ifndef SIMPLEMAP_H
#define SIMPLEMAP_H

#include "DynamicArray.h"

/**
 * @brief Map đơn giản sử dụng DynamicArray với tìm kiếm tuyến tính
 * @details CTDL tự viết, không dùng std::map
 *          Sử dụng DynamicArray để lưu trữ các cặp key-value
 *          Tìm kiếm: Linear search O(n)
 */
template<typename Key, typename Value>
class SimpleMap {
private:
    // Struct lưu trữ cặp key-value
    struct Entry {
        Key key;
        Value value;
        
        Entry() : key(), value() {}
        Entry(const Key& k, const Value& v) : key(k), value(v) {}
    };
    
    // Mảng động chứa các Entry
    DynamicArray<Entry> entries_;

public:
    // Constructor
    SimpleMap() : entries_() {}
    
    // Destructor
    ~SimpleMap() = default;
    
    /**
     * @brief Thêm hoặc cập nhật cặp key-value
     * @param key Khóa cần thêm/cập nhật
     * @param value Giá trị tương ứng
     */
    void insert(const Key& key, const Value& value) {
        // Tìm xem key đã tồn tại chưa
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                // Key đã tồn tại, cập nhật value
                entries_[i].value = value;
                return;
            }
        }
        
        // Key chưa tồn tại, thêm Entry mới
        entries_.push_back(Entry(key, value));
    }
    
    /**
     * @brief Tìm kiếm value theo key (non-const)
     * @param key Khóa cần tìm
     * @return Con trỏ đến value nếu tìm thấy, nullptr nếu không tìm thấy
     */
    Value* find(const Key& key) {
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                return &entries_[i].value;
            }
        }
        return nullptr;
    }
    
    /**
     * @brief Tìm kiếm value theo key (const)
     * @param key Khóa cần tìm
     * @return Con trỏ hằng đến value nếu tìm thấy, nullptr nếu không tìm thấy
     */
    const Value* find(const Key& key) const {
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                return &entries_[i].value;
            }
        }
        return nullptr;
    }
    
    /**
     * @brief Xóa một cặp key-value
     * @param key Khóa cần xóa
     * @return true nếu xóa thành công, false nếu không tìm thấy
     */
    bool remove(const Key& key) {
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                entries_.erase(i);
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Kiểm tra xem key có tồn tại không
     * @param key Khóa cần kiểm tra
     * @return true nếu key tồn tại, false nếu không
     */
    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }
    
    /**
     * @brief Lấy danh sách tất cả các key
     * @return DynamicArray chứa tất cả các key
     */
    DynamicArray<Key> getKeys() const {
        DynamicArray<Key> keys;
        for (int i = 0; i < entries_.size(); ++i) {
            keys.push_back(entries_[i].key);
        }
        return keys;
    }
    
    /**
     * @brief Lấy số lượng phần tử trong map
     * @return Số lượng cặp key-value
     */
    int size() const {
        return entries_.size();
    }
    
    /**
     * @brief Kiểm tra map có rỗng không
     * @return true nếu rỗng, false nếu không
     */
    bool empty() const {
        return entries_.empty();
    }
    
    /**
     * @brief Xóa tất cả phần tử
     */
    void clear() {
        entries_.clear();
    }
    
    /**
     * @brief Toán tử [] để truy cập/thêm mới phần tử
     * @param key Khóa cần truy cập
     * @return Tham chiếu đến value (tạo mới nếu chưa tồn tại)
     */
    Value& operator[](const Key& key) {
        // Tìm xem key đã tồn tại chưa
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                return entries_[i].value;
            }
        }
        
        // Key chưa tồn tại, thêm Entry mới với value mặc định
        entries_.push_back(Entry(key, Value()));
        return entries_[entries_.size() - 1].value;
    }
};

#endif // SIMPLEMAP_H
