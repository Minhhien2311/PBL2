#pragma once

#include <string>

// Hash Table: Xử lý xung đột bằng Chaining (Linked List)
template<typename Key, typename Value>
class HashTable {
private:
    struct Node {
        Key key;
        Value value;
        Node *next;
        Node(Key k, Value v) : key(k), value(v), next(nullptr) {}
    };

    Node** table;       // Mảng các con trỏ (buckets)
    size_t currentSize; // Số lượng phần tử hiện tại
    size_t tableSize;   // Kích thước bảng băm

    // Hàm băm FNV-1a (64-bit)
    size_t hashFunction(const std::string& key) const {
        const size_t FNV_PRIME = 1099511628211ULL;
        const size_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
        size_t hash = FNV_OFFSET_BASIS;
        for(char c: key) {
            hash ^= static_cast<size_t>(c);
            hash *= FNV_PRIME;
        }
        return hash % tableSize;
    } 

public:
    // --- Khởi tạo & Hủy ---
    explicit HashTable(size_t size = 101) : table(nullptr), currentSize(0), tableSize(size) {
        table = new Node*[tableSize];
        for (size_t i = 0; i < tableSize; ++i) table[i] = nullptr;
    }

    ~HashTable() {
        for (size_t i = 0; i < tableSize; ++i) {
            Node *entry = table[i];
            while (entry != nullptr) {
                Node *prev = entry;
                entry = entry->next;
                delete prev;
            }
        }
        delete[] table;
    }

    // --- Thao tác chính ---
    void insert(const Key& key, const Value& value) {
        size_t index = hashFunction(key);
        Node *entry = table[index];

        // Cập nhật nếu key đã tồn tại
        while (entry != nullptr) {
            if (entry->key == key) {
                entry->value = value;
                return;
            }
            entry = entry->next;
        }

        // Chèn node mới vào đầu danh sách (bucket)
        Node* newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        currentSize++;
    }

    bool remove(const Key& key) {
        size_t index = hashFunction(key);
        Node *entry = table[index];
        Node *prev = nullptr;

        // Tìm node cần xóa
        while (entry != nullptr && entry->key != key) {
            prev = entry;
            entry = entry->next;
        }

        if (entry == nullptr) return false; // Không tìm thấy

        // Xử lý xóa đầu hoặc giữa/cuối
        if (prev == nullptr) {
            table[index] = entry->next;
        } else {
            prev->next = entry->next;
        }
        
        delete entry;
        currentSize--;
        return true;
    }

    // --- Tìm kiếm ---
    Value* find(const Key& key) const {
        size_t index = hashFunction(key);
        Node *entry = table[index];
        while (entry != nullptr) {
            if (entry->key == key) return &entry->value;
            entry = entry->next;
        }
        return nullptr;
    }

    // --- Tiện ích ---
    size_t getSize() const { return currentSize; }
    bool isEmpty() const { return currentSize == 0; }
};