#pragma once

#include <vector>
#include <algorithm>

// AVL Tree Template Class
template <typename Key, typename Value>
class AVLTree {
private:
    // Cấu trúc Node (được giữ trong private để đóng gói)
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
    size_t currentSize;

public:

    AVLTree();
    ~AVLTree();

    // Thao tác chính
    void insert(const Key& key, const Value& value);
    bool remove(const Key& key);
    void clear();

    // Tìm kiếm
    Value* find(const Key& key);
    const Value* find(const Key& key) const;

    // Tính năng nâng cao
    std::vector<Value> rangeQuery(const Key& minKey, const Key& maxKey) const;
    std::vector<Value> getAllValues() const;

    // Getter
    size_t size() const;
    bool empty() const;
    int getTreeHeight() const;

private:
    // --- KHAI BÁO CÁC HÀM TRỢ GIÚP (PRIVATE) ---
    int getHeight(Node* node) const;
    void updateHeight(Node* node);
    int getBalanceFactor(Node* node) const;

    // Các phép quay
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* balance(Node* node);

    // Đệ quy
    Node* insertNode(Node* node, const Key& key, const Value& value);
    Node* findMin(Node* node) const;
    Node* removeNode(Node* node, const Key& key);
    Node* searchNode(Node* node, const Key& key) const;

    // Helper tiện ích
    void rangeQueryHelper(Node* node, const Key& minKey, const Key& maxKey, std::vector<Value>& result) const;
    void inOrderTraversal(Node* node, std::vector<Value>& result) const;
    void clearTree(Node* node);
    Node* copyTree(Node* node);
};

// =========================================================================
// PHẦN HIỆN THỰC (IMPLEMENTATION)
// Lưu ý: Với template class, phần này bắt buộc phải nằm trong file header
// =========================================================================

// -------------------------------------------------------------------------
// Helpers: Chiều cao & Cân bằng
// -------------------------------------------------------------------------

template <typename Key, typename Value>
int AVLTree<Key, Value>::getHeight(Node* node) const {
    return node ? node->height : 0;
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::updateHeight(Node* node) {
    if (node) {
        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }
}

template <typename Key, typename Value>
int AVLTree<Key, Value>::getBalanceFactor(Node* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// -------------------------------------------------------------------------
// Helpers: Phép quay & Cân bằng
// -------------------------------------------------------------------------

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::balance(Node* node) {
    if (!node) return nullptr;
    updateHeight(node);
    int bf = getBalanceFactor(node);

    // Left Left
    if (bf > 1 && getBalanceFactor(node->left) >= 0) {
        return rotateRight(node);
    }
    // Left Right
    if (bf > 1 && getBalanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    // Right Right
    if (bf < -1 && getBalanceFactor(node->right) <= 0) {
        return rotateLeft(node);
    }
    // Right Left
    if (bf < -1 && getBalanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

// -------------------------------------------------------------------------
// Helpers: Đệ quy Thêm/Xóa/Tìm
// -------------------------------------------------------------------------

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::insertNode(Node* node, const Key& key, const Value& value) {
    if (!node) {
        currentSize++;
        return new Node(key, value);
    }

    if (key < node->key) {
        node->left = insertNode(node->left, key, value);
    } else if (key > node->key) {
        node->right = insertNode(node->right, key, value);
    } else {
        node->value = value; // Cập nhật value nếu key tồn tại
        return node;
    }

    return balance(node);
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::findMin(Node* node) const {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::removeNode(Node* node, const Key& key) {
    if (!node) return nullptr;

    if (key < node->key) {
        node->left = removeNode(node->left, key);
    } else if (key > node->key) {
        node->right = removeNode(node->right, key);
    } else {
        // Tìm thấy node cần xóa
        if (!node->left || !node->right) {
            Node* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            } else {
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

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::searchNode(Node* node, const Key& key) const {
    if (!node || node->key == key) return node;
    return (key < node->key) ? searchNode(node->left, key) : searchNode(node->right, key);
}

// -------------------------------------------------------------------------
// Helpers: Tiện ích khác
// -------------------------------------------------------------------------

template <typename Key, typename Value>
void AVLTree<Key, Value>::rangeQueryHelper(Node* node, const Key& minKey, const Key& maxKey, std::vector<Value>& result) const {
    if (!node) return;
    if (node->key > minKey) rangeQueryHelper(node->left, minKey, maxKey, result);
    if (node->key >= minKey && node->key <= maxKey) result.push_back(node->value);
    if (node->key < maxKey) rangeQueryHelper(node->right, minKey, maxKey, result);
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::inOrderTraversal(Node* node, std::vector<Value>& result) const {
    if (!node) return;
    inOrderTraversal(node->left, result);
    result.push_back(node->value);
    inOrderTraversal(node->right, result);
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::clearTree(Node* node) {
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::copyTree(Node* node) {
    if (!node) return nullptr;
    Node* newNode = new Node(node->key, node->value);
    newNode->left = copyTree(node->left);
    newNode->right = copyTree(node->right);
    newNode->height = node->height;
    return newNode;
}

// =========================================================================
// PUBLIC METHODS
// =========================================================================

template <typename Key, typename Value>
AVLTree<Key, Value>::AVLTree() : root(nullptr), currentSize(0) {}

template <typename Key, typename Value>
AVLTree<Key, Value>::~AVLTree() {
    clear();
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::insert(const Key& key, const Value& value) {
    root = insertNode(root, key, value);
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::remove(const Key& key) {
    size_t oldSize = currentSize;
    root = removeNode(root, key);
    return currentSize < oldSize;
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::clear() {
    clearTree(root);
    root = nullptr;
    currentSize = 0;
}

template <typename Key, typename Value>
Value* AVLTree<Key, Value>::find(const Key& key) {
    Node* node = searchNode(root, key);
    return node ? &node->value : nullptr;
}

template <typename Key, typename Value>
const Value* AVLTree<Key, Value>::find(const Key& key) const {
    Node* node = searchNode(root, key);
    return node ? &node->value : nullptr;
}

template <typename Key, typename Value>
std::vector<Value> AVLTree<Key, Value>::rangeQuery(const Key& minKey, const Key& maxKey) const {
    std::vector<Value> result;
    rangeQueryHelper(root, minKey, maxKey, result);
    return result;
}

template <typename Key, typename Value>
std::vector<Value> AVLTree<Key, Value>::getAllValues() const {
    std::vector<Value> result;
    inOrderTraversal(root, result);
    return result;
}

template <typename Key, typename Value>
size_t AVLTree<Key, Value>::size() const {
    return currentSize;
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::empty() const {
    return currentSize == 0;
}

template <typename Key, typename Value>
int AVLTree<Key, Value>::getTreeHeight() const {
    return getHeight(root);
}