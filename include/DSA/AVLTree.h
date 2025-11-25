#pragma once

#include <vector>
#include <algorithm>

// AVL Tree: O(log n) cho Thêm/Xóa/Tìm + Range Query
template<typename Key, typename Value>
class AVLTree {
private:
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;
        int height;
        explicit Node(const Key& k, const Value& v) 
            : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    };
    
    Node* root;
    size_t currentSize; // Số lượng node

    // --- Hàm trợ giúp nội bộ (Chiều cao & Cân bằng) ---
    int getHeight(Node* node) const { return node ? node->height : 0; }
    
    void updateHeight(Node* node) {
        if (node) node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }
    
    int getBalanceFactor(Node* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    // --- Các phép quay (Rotations) ---
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }
    
    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }
    
    // Cân bằng lại cây
    Node* balance(Node* node) {
        if (!node) return nullptr;
        updateHeight(node);
        int bf = getBalanceFactor(node);
        
        // Left Left
        if (bf > 1 && getBalanceFactor(node->left) >= 0) return rotateRight(node);
        // Left Right
        if (bf > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Right
        if (bf < -1 && getBalanceFactor(node->right) <= 0) return rotateLeft(node);
        // Right Left
        if (bf < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    // --- Đệ quy Thêm/Xóa ---
    Node* insertNode(Node* node, const Key& key, const Value& value) {
        if (!node) { currentSize++; return new Node(key, value); }
        
        if (key < node->key) node->left = insertNode(node->left, key, value);
        else if (key > node->key) node->right = insertNode(node->right, key, value);
        else { node->value = value; return node; } // Cập nhật value nếu key tồn tại

        return balance(node);
    }
    
    Node* findMin(Node* node) const {
        while (node && node->left) node = node->left;
        return node;
    }
    
    Node* removeNode(Node* node, const Key& key) {
        if (!node) return nullptr;
        
        if (key < node->key) node->left = removeNode(node->left, key);
        else if (key > node->key) node->right = removeNode(node->right, key);
        else {
            // Tìm thấy node cần xóa
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) { temp = node; node = nullptr; }
                else { 
                    // Copy dữ liệu để tránh lỗi shallow copy
                    node->key = temp->key;
                    node->value = temp->value;
                    node->left = temp->left;
                    node->right = temp->right;
                    node->height = temp->height;
                }
                delete temp;
                currentSize--;
            } else {
                // Node có 2 con: tìm node nhỏ nhất bên phải thế vào
                Node* temp = findMin(node->right);
                node->key = temp->key;
                node->value = temp->value;
                node->right = removeNode(node->right, temp->key);
            }
        }
        if (!node) return nullptr;
        return balance(node);
    }
    
    // Tìm kiếm đệ quy
    Node* searchNode(Node* node, const Key& key) const {
        if (!node || node->key == key) return node;
        return (key < node->key) ? searchNode(node->left, key) : searchNode(node->right, key);
    }
    
    // Truy vấn khoảng (Range Query Helper)
    void rangeQueryHelper(Node* node, const Key& minKey, const Key& maxKey, std::vector<Value>& result) const {
        if (!node) return;
        if (node->key > minKey) rangeQueryHelper(node->left, minKey, maxKey, result);
        if (node->key >= minKey && node->key <= maxKey) result.push_back(node->value);
        if (node->key < maxKey) rangeQueryHelper(node->right, minKey, maxKey, result);
    }
    
    // Duyệt In-Order (Helper)
    void inOrderTraversal(Node* node, std::vector<Value>& result) const {
        if (!node) return;
        inOrderTraversal(node->left, result);
        result.push_back(node->value);
        inOrderTraversal(node->right, result);
    }
    
    // Xóa cây (Helper)
    void clearTree(Node* node) {
        if (!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
    
    // Copy cây (Helper cho copy constructor)
    Node* copyTree(Node* node) {
        if (!node) return nullptr;
        Node* newNode = new Node(node->key, node->value);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        newNode->height = node->height;
        return newNode;
    }

public:
    // --- Constructor & Destructor ---
    AVLTree() : root(nullptr), currentSize(0) {}
    ~AVLTree() { clear(); }
    
    AVLTree(const AVLTree& other) : root(nullptr), currentSize(0) {
        root = copyTree(other.root);
        currentSize = other.currentSize;
    }
    
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clear();
            root = copyTree(other.root);
            currentSize = other.currentSize;
        }
        return *this;
    }
    
    // --- Thao tác chính ---
    void insert(const Key& key, const Value& value) { root = insertNode(root, key, value); }
    
    bool remove(const Key& key) {
        size_t oldSize = currentSize;
        root = removeNode(root, key);
        return currentSize < oldSize;
    }
    
    // --- Tìm kiếm ---
    Value* find(const Key& key) {
        Node* node = searchNode(root, key);
        return node ? &node->value : nullptr;
    }
    const Value* find(const Key& key) const {
        Node* node = searchNode(root, key);
        return node ? &node->value : nullptr;
    }
    bool contains(const Key& key) const { return find(key) != nullptr; }
    
    // --- Tính năng nâng cao ---
    // Tìm các giá trị nằm trong khoảng [minKey, maxKey]
    std::vector<Value> rangeQuery(const Key& minKey, const Key& maxKey) const {
        std::vector<Value> result;
        rangeQueryHelper(root, minKey, maxKey, result);
        return result;
    }
    
    // Lấy tất cả giá trị (đã sắp xếp)
    std::vector<Value> getAllValues() const {
        std::vector<Value> result;
        inOrderTraversal(root, result);
        return result;
    }
    
    // --- Tiện ích ---
    size_t size() const { return currentSize; }
    bool empty() const { return currentSize == 0; }
    void clear() { clearTree(root); root = nullptr; currentSize = 0; }
    int getTreeHeight() const { return getHeight(root); }
};