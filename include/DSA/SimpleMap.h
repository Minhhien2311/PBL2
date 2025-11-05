#ifndef SIMPLEMAP_H
#define SIMPLEMAP_H

#include "DynamicArray.h"

//  Map đơn giản sử dụng DynamicArray với tìm kiếm tuyến tính
//  Sử dụng DynamicArray để lưu trữ các cặp key-value
//  Tìm kiếm: Linear search O(n)
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
    
    // Thêm hoặc cập nhật cặp key-value
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

    // Tìm kiếm value theo key (non-const)
    Value* find(const Key& key) {
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                return &entries_[i].value;
            }
        }
        return nullptr;
    }

    // Tìm kiếm value theo key (const)
    const Value* find(const Key& key) const {
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                return &entries_[i].value;
            }
        }
        return nullptr;
    }

    // Xóa một cặp key-value
    bool remove(const Key& key) {
        for (int i = 0; i < entries_.size(); ++i) {
            if (entries_[i].key == key) {
                entries_.erase(i);
                return true;
            }
        }
        return false;
    }

    // Kiểm tra xem key có tồn tại không
    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }

    // Lấy danh sách tất cả các key
    DynamicArray<Key> getKeys() const {
        DynamicArray<Key> keys;
        for (int i = 0; i < entries_.size(); ++i) {
            keys.push_back(entries_[i].key);
        }
        return keys;
    }

    // Lấy số lượng phần tử trong map
    int size() const {
        return entries_.size();
    }
    
    // Kiểm tra map có rỗng không
    bool empty() const {
        return entries_.empty();
    }
    
    // Xóa tất cả phần tử
    void clear() {
        entries_.clear();
    }
    
    // Toán tử [] để truy cập/thêm mới phần tử
    // Nhập key cần truy cập, tham chiếu đến value (tạo mới nếu chưa tồn tại)
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

#endif
