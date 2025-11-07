#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>
#include <algorithm>

// AVL Tree - Self-balancing Binary Search Tree
// Sử dụng cho tìm kiếm và truy vấn theo khoảng (range query)
// Thời gian: Insert, Delete, Search: O(log n)
// Range query: O(log n + k) với k là số phần tử trong khoảng
template<typename Key, typename Value>
class AVLTree {
private:
    // Node trong AVL Tree chứa key, value, chiều cao và các con trỏ trái/phải
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;
        int height;
        
        Node(const Key& k, const Value& v)
            : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    };
    
    Node* root_;
    int size_;
    
    // Lấy chiều cao của node
    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }
    
    // Cập nhật chiều cao của node
    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }
    
    // Tính balance factor của node
    // Balance factor = height(left subtree) - height(right subtree)
    // Nếu |balance factor| > 1 thì node không cân bằng
    int getBalanceFactor(Node* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    // Xoay phải (Right rotation)
    // Dùng khi left subtree cao hơn right subtree
    //       y                               x
    //      / \     Right Rotation          /  \
    //     x   T3   - - - - - - - ->       T1   y
    //    / \                                   / \
    //   T1  T2                                T2  T3
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        
        // Thực hiện rotation
        x->right = y;
        y->left = T2;
        
        // Cập nhật chiều cao
        updateHeight(y);
        updateHeight(x);
        
        return x; // x trở thành root mới
    }
    
    // Xoay trái (Left rotation)
    // Dùng khi right subtree cao hơn left subtree
    //     x                                y
    //    /  \     Left Rotation           / \
    //   T1   y    - - - - - - - ->       x   T3
    //       / \                          / \
    //      T2  T3                       T1  T2
    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        
        // Thực hiện rotation
        y->left = x;
        x->right = T2;
        
        // Cập nhật chiều cao
        updateHeight(x);
        updateHeight(y);
        
        return y; // y trở thành root mới
    }
    
    // Cân bằng node sau khi insert/delete
    Node* balance(Node* node) {
        if (!node) return nullptr;
        
        updateHeight(node);
        int balanceFactor = getBalanceFactor(node);
        
        // Left-Left case (LL)
        if (balanceFactor > 1 && getBalanceFactor(node->left) >= 0) {
            return rotateRight(node);
        }
        
        // Left-Right case (LR)
        if (balanceFactor > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right-Right case (RR)
        if (balanceFactor < -1 && getBalanceFactor(node->right) <= 0) {
            return rotateLeft(node);
        }
        
        // Right-Left case (RL)
        if (balanceFactor < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    // Thêm node vào cây (đệ quy)
    Node* insertNode(Node* node, const Key& key, const Value& value) {
        // Nếu node rỗng, tạo node mới
        if (!node) {
            size_++;
            return new Node(key, value);
        }
        
        // Tìm vị trí insert
        if (key < node->key) {
            node->left = insertNode(node->left, key, value);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, value);
        } else {
            // Key đã tồn tại, cập nhật value
            node->value = value;
            return node;
        }
        
        // Cân bằng cây sau khi insert
        return balance(node);
    }
    
    // Tìm node có key nhỏ nhất trong subtree
    Node* findMin(Node* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }
    
    // Xóa node khỏi cây (đệ quy)
    Node* removeNode(Node* node, const Key& key) {
        if (!node) return nullptr;
        
        // Tìm node cần xóa
        if (key < node->key) {
            node->left = removeNode(node->left, key);
        } else if (key > node->key) {
            node->right = removeNode(node->right, key);
        } else {
            // Tìm thấy node cần xóa
            
            // Trường hợp 1: Node là leaf hoặc có 1 con
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                
                if (!temp) {
                    // Node là leaf
                    temp = node;
                    node = nullptr;
                } else {
                    // Node có 1 con
                    *node = *temp;
                }
                
                delete temp;
                size_--;
            } else {
                // Trường hợp 2: Node có 2 con
                // Tìm successor (node nhỏ nhất ở subtree phải)
                Node* temp = findMin(node->right);
                
                // Copy data của successor vào node hiện tại
                node->key = temp->key;
                node->value = temp->value;
                
                // Xóa successor
                node->right = removeNode(node->right, temp->key);
            }
        }
        
        if (!node) return nullptr;
        
        // Cân bằng cây sau khi xóa
        return balance(node);
    }
    
    // Tìm kiếm node theo key (đệ quy)
    Node* searchNode(Node* node, const Key& key) const {
        if (!node || node->key == key) {
            return node;
        }
        
        if (key < node->key) {
            return searchNode(node->left, key);
        } else {
            return searchNode(node->right, key);
        }
    }
    
    // Truy vấn theo khoảng (range query) - đệ quy
    // Tìm tất cả các node có key trong khoảng [minKey, maxKey]
    void rangeQueryHelper(Node* node, const Key& minKey, const Key& maxKey, 
                         std::vector<Value>& result) const {
        if (!node) return;
        
        // Nếu key hiện tại > minKey, tìm ở subtree trái
        if (node->key > minKey) {
            rangeQueryHelper(node->left, minKey, maxKey, result);
        }
        
        // Nếu key nằm trong khoảng, thêm vào kết quả
        if (node->key >= minKey && node->key <= maxKey) {
            result.push_back(node->value);
        }
        
        // Nếu key hiện tại < maxKey, tìm ở subtree phải
        if (node->key < maxKey) {
            rangeQueryHelper(node->right, minKey, maxKey, result);
        }
    }
    
    // Duyệt cây theo thứ tự in-order (trái - gốc - phải)
    void inOrderTraversal(Node* node, std::vector<Value>& result) const {
        if (!node) return;
        
        inOrderTraversal(node->left, result);
        result.push_back(node->value);
        inOrderTraversal(node->right, result);
    }
    
    // Xóa toàn bộ cây (đệ quy)
    void clearTree(Node* node) {
        if (!node) return;
        
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
    
    // Copy cây (đệ quy) - dùng cho copy constructor
    Node* copyTree(Node* node) {
        if (!node) return nullptr;
        
        Node* newNode = new Node(node->key, node->value);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        newNode->height = node->height;
        
        return newNode;
    }

public:
    // Constructor
    AVLTree() : root_(nullptr), size_(0) {}
    
    // Destructor
    ~AVLTree() {
        clear();
    }
    
    // Copy constructor
    AVLTree(const AVLTree& other) : root_(nullptr), size_(0) {
        root_ = copyTree(other.root_);
        size_ = other.size_;
    }
    
    // Copy assignment operator
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clear();
            root_ = copyTree(other.root_);
            size_ = other.size_;
        }
        return *this;
    }
    
    // Thêm hoặc cập nhật cặp key-value
    void insert(const Key& key, const Value& value) {
        root_ = insertNode(root_, key, value);
    }
    
    // Xóa cặp key-value
    bool remove(const Key& key) {
        if (!find(key)) return false;
        root_ = removeNode(root_, key);
        return true;
    }
    
    // Tìm kiếm value theo key
    Value* find(const Key& key) {
        Node* node = searchNode(root_, key);
        return node ? &node->value : nullptr;
    }
    
    // Tìm kiếm value theo key (const version)
    const Value* find(const Key& key) const {
        Node* node = searchNode(root_, key);
        return node ? &node->value : nullptr;
    }
    
    // Kiểm tra key có tồn tại không
    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }
    
    // Truy vấn theo khoảng - lấy tất cả value có key trong [minKey, maxKey]
    // Ứng dụng: Tìm các chuyến bay trong khoảng giá min-max
    std::vector<Value> rangeQuery(const Key& minKey, const Key& maxKey) const {
        std::vector<Value> result;
        rangeQueryHelper(root_, minKey, maxKey, result);
        return result;
    }
    
    // Lấy tất cả value trong cây (sorted theo key)
    std::vector<Value> getAllValues() const {
        std::vector<Value> result;
        inOrderTraversal(root_, result);
        return result;
    }
    
    // Lấy số lượng phần tử trong cây
    int size() const {
        return size_;
    }
    
    // Kiểm tra cây có rỗng không
    bool empty() const {
        return size_ == 0;
    }
    
    // Xóa tất cả phần tử
    void clear() {
        clearTree(root_);
        root_ = nullptr;
        size_ = 0;
    }
    
    // Lấy chiều cao của cây
    int getTreeHeight() const {
        return getHeight(root_);
    }
};

#endif
