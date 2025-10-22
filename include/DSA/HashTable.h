#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>

template<typename Key, typename Value>
class HashTable {
private:
    // Mỗi Node là một "mẫu dữ liệu" chứa 1 cặp key-value và con trỏ trỏ đến Node tiếp theo
    struct Node {
        Key key;
        Value value;
        Node *next;

        Node(Key k, Value v) : key(k), value(v), next(nullptr) {}
    };

    // Mảng các con trỏ, mỗi con trỏ trỏ tới đầu một danh sách liên kết
    Node** table;
    size_t currentSize;
    size_t tableSize;

    // Hàm băm biến Key thành index
    // Thuật toán băm: FNV-1a
    size_t hashFunction(const std::string& key) const {
        // Các hằng số của FNV-1a cho hệ thống 64-bit
        const size_t FNV_PRIME = 1099511628211ULL;
        const size_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

        size_t hash = FNV_OFFSET_BASIS;
        for(char c: key) {
            // Thực hiện phép XOR giữa giá trị băm hiện tại và ký tự c
            hash ^= static_cast<size_t>(c);
            hash *= FNV_PRIME;
        }
        // Chia lấy dư để đảm bảo kết quả nằm trong khoảng [0, tableSize - 1]
        return hash % tableSize;
    } 

public:
    // Constructor
    // Dùng explicit để tránh chuyển đổi kiểu ngầm
    explicit HashTable(size_t size = 101) : table(nullptr), currentSize(0), tableSize(size) {
        // Cấp phát vùng nhớ cho mảng các con trỏ
        table = new Node*[tableSize];

        // Khởi tạo cho mỗi con trỏ trong mảng trỏ tới null (chưa trỏ tới đầu DSLK)
        for (size_t i = 0; i < tableSize; ++i) {
            table[i] = nullptr;
        }
    }

    // Destructor
    ~HashTable() {
        // Duyêt từng ngăn trong mảng con trỏ
        for (size_t i = 0; i < tableSize; ++i) {
            Node *entry = table[i];
            // Duyệt qua DSLK và xóa từng Node
            while (entry != nullptr) {
                Node *prev = entry;
                entry = entry -> next;
                delete prev;
            }
        }
        // Cuối cùng, xóa mảng chứa các con trỏ đầu danh sách
        delete[] table;
    }

    // Hàm thêm vào HashTable
    void insert(const Key& key, const Value& value) {
        size_t index = hashFunction(key);
        Node *entry = table[index];

        // Kiểm tra key đã tồn tại hay chưa
        while (entry != nullptr) {
            // Nếu có cập nhập value và thoát
            if (entry -> key == key) {
                entry -> value = value;
                return;
            }
            // Nếu chưa, tiếp tục duyệt tới cuối danh sách
            entry = entry -> next;
        }

        // Đi đến đây tức là key chưa tồn tại, ta thêm Node mới vào đầu danh sách
        Node* newNode = new Node(key, value);
        newNode -> next = table[index];     // Node mới trỏ vào đâu danh sách cũ
        table[index] = newNode;             // Cập nhập đầu danh sách là Node mới
        currentSize++;
    }

    // Hàm tìm kiếm theo key
    Value* find(const Key& key) const {
        size_t index = hashFunction(key);
        Node *entry = table[index];

        while (entry != nullptr) {
            if (entry -> key == key) {
                // Tìm thấy thì trả về con trỏ trỏ tới value của key
                return &entry -> value;
            }
            entry = entry -> next;
        }
        // Trường hợp không tìm thấy
        return nullptr;
    }

    bool remove(const Key& key) {
        size_t index = hashFunction(key);
        Node *entry = table[index];
        Node *prev = nullptr;

        // Tìm Node cần xóa
        while (entry != nullptr && entry -> key != key) {
            prev = entry;
            entry = entry -> next;
        }

        // Nếu không tìm thấy Node cần xóa
        if (entry == nullptr) return false;

        // Nếu Node cần xóa là đầu danh sách
        if (prev == nullptr) {
            table[index] = entry -> next;
        } else {
            // Nếu Node cần xóa ở giữa hoặc cuối
            prev -> next = entry -> next;
        }
        // Giải phóng bộ nhớ
        delete entry;
        currentSize--;
        return true;
    }

    // Các hàm tiện ích
    size_t getSize() const { return currentSize; }
    bool isEmpty() const { return currentSize == 0; }

};

#endif