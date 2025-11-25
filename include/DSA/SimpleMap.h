#pragma once

#include <vector>

// Map đơn giản sử dụng std::vector (Tìm kiếm tuyến tính O(n))
template<typename Key, typename Value>
class SimpleMap {
private:
    struct Entry {
        Key key;
        Value value;
        Entry() : key(), value() {}
        Entry(const Key& k, const Value& v) : key(k), value(v) {}
    };
    
    // Dữ liệu lưu trữ
    std::vector<Entry> entries;

public:
    // --- Khởi tạo & Hủy ---
    SimpleMap() = default;
    ~SimpleMap() = default;
    
    // --- Thao tác chính ---
    // Thêm hoặc cập nhật (nếu key đã tồn tại)
    void insert(const Key& key, const Value& value) {
        for (auto& entry : entries) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }
        entries.emplace_back(key, value);
    }

    // Xóa phần tử theo key
    bool remove(const Key& key) {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if (it->key == key) {
                entries.erase(it);
                return true;
            }
        }
        return false;
    }

    // --- Truy cập dữ liệu ---
    Value* find(const Key& key) {
        for (auto& entry : entries) {
            if (entry.key == key) return &entry.value;
        }
        return nullptr;
    }

    const Value* find(const Key& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key) return &entry.value;
        }
        return nullptr;
    }

    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }

    // Toán tử []: Truy cập hoặc tạo mới mặc định
    Value& operator[](const Key& key) {
        for (auto& entry : entries) {
            if (entry.key == key) return entry.value;
        }
        entries.emplace_back(key, Value());
        return entries.back().value;
    }

    // --- Tiện ích ---
    std::vector<Key> getKeys() const {
        std::vector<Key> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) keys.push_back(entry.key);
        return keys;
    }

    int size() const { return entries.size(); }
    bool empty() const { return entries.empty(); }
    void clear() { entries.clear(); }
};