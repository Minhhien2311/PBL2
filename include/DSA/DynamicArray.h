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
        currentCapacity = initialCapacity;
        currentSize = 0;
        data = new T[currentCapacity];
    }

    // Destructor - RẤT QUAN TRỌNG
    // Vì chúng ta biết T là một con trỏ (ví dụ Flight*), chúng ta phải tự dọn dẹp bộ nhớ
    ~DynamicArray() {
        // 1. Xóa từng đối tượng mà các con trỏ đang trỏ tới
        for (int i = 0; i < currentSize; ++i) {
            delete data[i];
        }
        // 2. Xóa mảng chứa các con trỏ
        delete[] data;
    }

    // <<< PHẦN SỬA LỖI CHÍNH NẰM Ở ĐÂY
    // Hàm push_back giờ đây nhận vào một giá trị kiểu T (chính là một con trỏ như Flight*)
    void push_back(T value) {
        if (currentSize == currentCapacity) {
            resize();
        }
        data[currentSize++] = value;
    }

    // <<< PHẦN SỬA LỖI CHÍNH NẰM Ở ĐÂY
    // Toán tử [] giờ trả về một giá trị kiểu T (chính là một con trỏ)
    T operator[](int index) {
        if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    // Phiên bản const để đọc
    const T operator[](int index) const {
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
};