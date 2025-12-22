#pragma once

#include <string>

// Hash Table: Xử lý xung đột bằng Chaining (Linked List)
template<typename Key, typename Value>
class HashTable {
private:
    // Cấu trúc Node cho danh sách liên kết đơn
    struct Node {
        Key key;
        Value value;
        Node* next;
        Node(Key k, Value v) : key(k), value(v), next(nullptr) {}
    };

    Node** table;       // Mảng các con trỏ (buckets)
    size_t currentSize; // Số lượng phần tử hiện tại
    size_t tableSize;   // Kích thước bảng băm
    size_t hashFunction(const std::string& key) const;

public:
    explicit HashTable(size_t size = 101);
    ~HashTable();

    void insert(const Key& key, const Value& value);
    bool remove(const Key& key);
    Value* find(const Key& key) const;
    size_t getSize() const;
    bool isEmpty() const;
};

// --- Implement Hàm băm ---
template<typename Key, typename Value>
size_t HashTable<Key, Value>::hashFunction(const std::string& key) const {
    const size_t FNV_PRIME = 1099511628211ULL;
    const size_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    size_t hash = FNV_OFFSET_BASIS;
    for (char c : key) {
        hash ^= static_cast<size_t>(c);
        hash *= FNV_PRIME;
    }
    return hash % tableSize;
}

// --- Implement Constructor ---
template<typename Key, typename Value>
HashTable<Key, Value>::HashTable(size_t size) 
    : table(nullptr), currentSize(0), tableSize(size) {
    table = new Node*[tableSize];
    for (size_t i = 0; i < tableSize; ++i) {
        table[i] = nullptr;
    }
}

// --- Implement Destructor ---
template<typename Key, typename Value>
HashTable<Key, Value>::~HashTable() {
    for (size_t i = 0; i < tableSize; ++i) {
        Node* entry = table[i];
        while (entry != nullptr) {
            Node* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

// --- Implement Insert ---
template<typename Key, typename Value>
void HashTable<Key, Value>::insert(const Key& key, const Value& value) {
    size_t index = hashFunction(key);
    Node* entry = table[index];

    // Cập nhật nếu key đã tồn tại trong bucket
    while (entry != nullptr) {
        if (entry->key == key) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    // Nếu chưa tồn tại, chèn node mới vào đầu danh sách (bucket)
    Node* newNode = new Node(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    currentSize++;
}

// --- Implement Remove ---
template<typename Key, typename Value>
bool HashTable<Key, Value>::remove(const Key& key) {
    size_t index = hashFunction(key);
    Node* entry = table[index];
    Node* prev = nullptr;

    // Tìm node cần xóa
    while (entry != nullptr && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    // Không tìm thấy
    if (entry == nullptr) {
        return false;
    }

    // Xử lý xóa nút
    if (prev == nullptr) {
        // Nút cần xóa là nút đầu tiên trong bucket
        table[index] = entry->next;
    } else {
        // Nút cần xóa nằm ở giữa hoặc cuối
        prev->next = entry->next;
    }

    delete entry;
    currentSize--;
    return true;
}

// --- Implement Find ---
template<typename Key, typename Value>
Value* HashTable<Key, Value>::find(const Key& key) const {
    size_t index = hashFunction(key);
    Node* entry = table[index];
    while (entry != nullptr) {
        if (entry->key == key) {
            return &entry->value;
        }
        entry = entry->next;
    }
    return nullptr;
}

// --- Implement GetSize ---
template<typename Key, typename Value>
size_t HashTable<Key, Value>::getSize() const {
    return currentSize;
}

// --- Implement IsEmpty ---
template<typename Key, typename Value>
bool HashTable<Key, Value>::isEmpty() const {
    return currentSize == 0;
}