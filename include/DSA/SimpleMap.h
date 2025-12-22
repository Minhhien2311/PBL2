#pragma once

#include <vector>

template <typename Key, typename Value>
class SimpleMap {
private:
    // Cấu trúc lưu trữ cặp Key-Value
    struct Entry {
        Key key;
        Value value;
        Entry() : key(), value() {}
        Entry(const Key& k, const Value& v) : key(k), value(v) {}
    };

    // Dữ liệu lưu trữ chính
    std::vector<Entry> entries;

public:
    // --- Khởi tạo & Hủy ---
    SimpleMap() = default;
    ~SimpleMap() = default;

    // --- Thao tác chính ---
    void insert(const Key& key, const Value& value);
    bool remove(const Key& key);
    void clear();

    // --- Truy cập dữ liệu ---
    Value* find(const Key& key);
    const Value* find(const Key& key) const;
    bool contains(const Key& key) const;
    Value& operator[](const Key& key);

    // --- Tiện ích ---
    std::vector<Key> getKeys() const;
    int size() const;
    bool empty() const;
};

// --- Thao tác chính ---

template <typename Key, typename Value>
void SimpleMap<Key, Value>::insert(const Key& key, const Value& value) {
    // Cập nhật nếu key đã tồn tại
    for (auto& entry : entries) {
        if (entry.key == key) {
            entry.value = value;
            return;
        }
    }
    // Nếu chưa có, thêm mới vào cuối
    entries.emplace_back(key, value);
}

template <typename Key, typename Value>
bool SimpleMap<Key, Value>::remove(const Key& key) {
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->key == key) {
            entries.erase(it);
            return true;
        }
    }
    return false;
}

template <typename Key, typename Value>
void SimpleMap<Key, Value>::clear() {
    entries.clear();
}

// --- Truy cập dữ liệu ---

template <typename Key, typename Value>
Value* SimpleMap<Key, Value>::find(const Key& key) {
    for (auto& entry : entries) {
        if (entry.key == key) {
            return &entry.value;
        }
    }
    return nullptr;
}

template <typename Key, typename Value>
const Value* SimpleMap<Key, Value>::find(const Key& key) const {
    for (const auto& entry : entries) {
        if (entry.key == key) {
            return &entry.value;
        }
    }
    return nullptr;
}

template <typename Key, typename Value>
bool SimpleMap<Key, Value>::contains(const Key& key) const {
    return find(key) != nullptr;
}

template <typename Key, typename Value>
Value& SimpleMap<Key, Value>::operator[](const Key& key) {
    // Tìm kiếm xem key đã tồn tại chưa
    for (auto& entry : entries) {
        if (entry.key == key) {
            return entry.value;
        }
    }
    // Nếu chưa, tạo mới mặc định và trả về tham chiếu
    entries.emplace_back(key, Value());
    return entries.back().value;
}

// --- Tiện ích ---

template <typename Key, typename Value>
std::vector<Key> SimpleMap<Key, Value>::getKeys() const {
    std::vector<Key> keys;
    keys.reserve(entries.size());
    for (const auto& entry : entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

template <typename Key, typename Value>
int SimpleMap<Key, Value>::size() const {
    return static_cast<int>(entries.size());
}

template <typename Key, typename Value>
bool SimpleMap<Key, Value>::empty() const {
    return entries.empty();
}