#pragma once
#include <stdexcept>

template <typename T> // T sẽ là một kiểu dữ liệu, ví dụ: Flight*
class DynamicArray {
private:
    T* data;            // Một mảng chứa các phần tử kiểu T. Nếu T là Flight*, thì đây là mảng các Flight*
    int currentSize;
    int currentCapacity;

    void resize() {
        currentCapacity *= 2;
        T* newData = new T[currentCapacity]; // Tạo một mảng mới chứa các T
        for (int i = 0; i < currentSize; ++i) {
            newData[i] = data[i]; // Sao chép các phần tử cũ (chính là các con trỏ)
        }
        delete[] data;
        data = newData;
    }

public:
    // Constructor
    DynamicArray(int initialCapacity = 10) {
        // Đảm bảo capacity luôn là một số dương
        currentCapacity = (initialCapacity > 0) ? initialCapacity : 1;
        currentSize = 0;
        data = new T[currentCapacity];
    }

    // Destructor
    ~DynamicArray() {
        clear(); // Gọi hàm clear để dọn dẹp
        delete[] data;
    }

    // // --- CƠ CHẾ AN TOÀN: CẤM SAO CHÉP ---
    // // Ngăn chặn các lỗi quản lý bộ nhớ nguy hiểm
    // DynamicArray(const DynamicArray&) = delete;
    // DynamicArray& operator=(const DynamicArray&) = delete;

    // Hàm push_back giờ đây nhận vào một giá trị kiểu T (chính là một con trỏ như Flight*)
    void push_back(T value) {
        if (currentSize == currentCapacity) {
            resize();
        }
        data[currentSize++] = value;
    }

    // Toán tử [] giờ trả về một giá trị kiểu T& (chính là một con trỏ)
    T& operator[](int index) {
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    // Phiên bản const để đọc
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

    // Xóa phần tử tại một chỉ số cụ thể
    bool erase(int index) {
        if (index < 0 || index >= currentSize) {
            return false; // Chỉ số không hợp lệ
        }

        // 1. Giải phóng bộ nhớ của đối tượng mà con trỏ đang trỏ tới
        delete data[index];

        // 2. Dịch các con trỏ phía sau lên để lấp vào chỗ trống
        for (int i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
        }
        
        currentSize--; // Giảm kích thước mảng
        return true;
    }

    // Xóa tất cả các phần tử khỏi mảng
    void clear() {
        for (int i = 0; i < currentSize; ++i) {
            delete data[i]; // Giải phóng bộ nhớ của từng đối tượng
        }
        currentSize = 0; // Reset kích thước về 0
    }
};



/*  MẢNG ĐỐI TƯỢNG
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <cstddef>   // Cần cho kiểu size_t, là kiểu dữ liệu chuẩn cho kích thước
#include <stdexcept> // Cần để ném ngoại lệ khi truy cập sai chỉ số

DynamicArray: Lớp Mảng Động tự triển khai, mô phỏng đầy đủ std::vector.
- Hỗ trợ templates, iterators, quản lý bộ nhớ, và các hàm thao tác cốt lõi.

template <typename T>
class DynamicArray {
public:
    // --- Iterator Class ---
    // Cung cấp một "con trỏ thông minh" để duyệt qua các phần tử của mảng.
    // Cần thiết cho các vòng lặp for-each và các hàm như erase.
    class Iterator {
    private:
        T* m_ptr; // Con trỏ tới phần tử hiện tại trong mảng data

    public:
        // Constructor
        explicit Iterator(T* ptr) : m_ptr(ptr) {}

        // Các toán tử cần thiết cho một iterator
        T& operator*() const { return *m_ptr; }           // Lấy giá trị (dereference)
        T* operator->() { return m_ptr; }                 // Truy cập thành viên qua con trỏ
        Iterator& operator++() { m_ptr++; return *this; } // Tăng lên (prefix)
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; } // Tăng lên (postfix)
        
        // So sánh hai iterator
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
    };

private:
    T* data;                // Con trỏ tới vùng nhớ heap chứa dữ liệu
    size_t currentSize;     // Số lượng phần tử hiện tại
    size_t currentCapacity; // Sức chứa tối đa của mảng

    // Tăng gấp đôi sức chứa khi cần
    void resize() {
        currentCapacity = (currentCapacity == 0) ? 1 : currentCapacity * 2;
        T* newData = new T[currentCapacity];
        for (size_t i = 0; i < currentSize; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    // --- Constructor / Destructor / Assignment (Rule of Three) ---
    // Cần thiết để quản lý bộ nhớ động một cách an toàn.

    // Constructor mặc định
    DynamicArray() : data(nullptr), currentSize(0), currentCapacity(0) {}

    // Copy Constructor (Tạo bản sao sâu)
    DynamicArray(const DynamicArray& other) {
        currentSize = other.currentSize;
        currentCapacity = other.currentCapacity;
        data = new T[currentCapacity];
        for (size_t i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
    }

    // Copy Assignment Operator
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this; // Chống tự gán
        
        delete[] data; // Giải phóng bộ nhớ cũ
        
        currentSize = other.currentSize;
        currentCapacity = other.currentCapacity;
        data = new T[currentCapacity];
        for (size_t i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // Destructor
    ~DynamicArray() {
        delete[] data;
    }

    // --- Iterators ---
    // Trả về iterator trỏ đến phần tử đầu tiên.
    Iterator begin() { return Iterator(data); }
    // Trả về iterator trỏ đến vị trí "sau" phần tử cuối cùng.
    Iterator end() { return Iterator(data + currentSize); }

    // --- Capacity ---
    size_t size() const { return currentSize; }
    size_t capacity() const { return currentCapacity; }
    bool isEmpty() const { return currentSize == 0; }

    // --- Element access ---
    T& operator[](size_t index) { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }
    T& at(size_t index) {
        if (index >= currentSize) throw std::out_of_range("DynamicArray::at: index out of range");
        return data[index];
    }
    const T& at(size_t index) const {
        if (index >= currentSize) throw std::out_of_range("DynamicArray::at: index out of range");
        return data[index];
    }
    T& front() { return data[0]; }
    T& back() { return data[currentSize - 1]; }


    // --- Modifiers ---
    void push_back(const T& value) {
        if (currentSize == currentCapacity) {
            resize();
        }
        data[currentSize++] = value;
    }

    void pop_back() {
        if (currentSize > 0) {
            currentSize--;
        }
    }

    // Xóa phần tử tại vị trí được chỉ định bởi iterator.
    // Trả về iterator trỏ đến phần tử ngay sau phần tử vừa bị xóa.
    Iterator erase(Iterator position) {
        // Chuyển iterator thành chỉ số (index)
        size_t index = position.m_ptr - data;
        if (index >= currentSize) return end(); // Không hợp lệ

        // Dịch chuyển tất cả các phần tử phía sau về bên trái 1 vị trí
        for (size_t i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
        }
        currentSize--;
        
        // Trả về iterator tại vị trí vừa xóa (giờ đã chứa phần tử mới)
        return Iterator(data + index);
    }
    
    // Nạp chồng erase để xóa phần tử tại một CHỈ SỐ (index) cụ thể.
    // Trả về true nếu xóa thành công.
    bool erase(size_t index) {
        if (index >= currentSize) {
            return false; // Chỉ số không hợp lệ
        }

        // Dịch chuyển tất cả các phần tử phía sau về bên trái 1 vị trí
        for (size_t i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
        }
        currentSize--;
        
        return true;
    }
    
    // Xóa tất cả các phần tử.
    void clear() {
        currentSize = 0;
        // Lưu ý: không giải phóng bộ nhớ, chỉ reset kích thước.
    }
};

#endif // DYNAMIC_ARRAY_H
*/