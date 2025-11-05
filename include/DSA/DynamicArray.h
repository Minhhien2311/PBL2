#pragma once
#include <stdexcept>
#include <utility> // For std::move (C++11)

// T sẽ là một kiểu dữ liệu, ví dụ: Flight* hoặc std::string
template <typename T>
class DynamicArray {
private:
    T* data;
    int currentSize;
    int currentCapacity;

    void resize() {
        currentCapacity *= 2;
        T* newData = new T[currentCapacity]; 
        for (int i = 0; i < currentSize; ++i) {
            newData[i] = data[i]; 
        }
        delete[] data;
        data = newData;
    }

public:
    // ==================== CONSTRUCTOR ====================
    DynamicArray(int initialCapacity = 10) {
        currentCapacity = (initialCapacity > 0) ? initialCapacity : 1;
        currentSize = 0;
        data = new T[currentCapacity];
    }

    // ==================== DESTRUCTOR ====================
    ~DynamicArray() {
        delete[] data;
    }

    // ==================== COPY CONSTRUCTOR ====================
    DynamicArray(const DynamicArray& other) 
        : currentCapacity(other.currentCapacity),
          currentSize(other.currentSize) 
    {
        data = new T[currentCapacity];
        for (int i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
    }

    // ==================== COPY ASSIGNMENT ====================
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) {
            return *this;
        }
        
        delete[] data;
        
        currentCapacity = other.currentCapacity;
        currentSize = other.currentSize;
        data = new T[currentCapacity];
        
        for (int i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
        
        return *this;
    }

    // ==================== MOVE CONSTRUCTOR ====================
    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data),
          currentSize(other.currentSize),
          currentCapacity(other.currentCapacity)
    {
        other.data = nullptr;
        other.currentSize = 0;
        other.currentCapacity = 0;
    }

    // ==================== MOVE ASSIGNMENT ====================
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        
        delete[] data;
        
        data = other.data;
        currentSize = other.currentSize;
        currentCapacity = other.currentCapacity;
        
        other.data = nullptr;
        other.currentSize = 0;
        other.currentCapacity = 0;
        
        return *this;
    }

    // ==================== CÁC HÀM CƠ BẢN ====================

    void push_back(T value) {
        if (currentSize == currentCapacity) {
            resize();
        }
        data[currentSize++] = value;
    }

    T& operator[](int index) {
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    int size() const {
        return currentSize;
    }

    bool empty() const {
        return currentSize == 0;
    }

    bool erase(int index) {
        if (index < 0 || index >= currentSize) {
            return false;
        }

        for (int i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
        }
        
        currentSize--;
        return true;
    }

    void clear() {
        currentSize = 0;
    }

    // Lấy capacity hiện tại
    int capacity() const {
        return currentCapacity;
    }
};