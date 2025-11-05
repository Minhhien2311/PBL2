#pragma once
#include <stdexcept>

// T sẽ là một kiểu dữ liệu, ví dụ: Flight* hoặc std::string
template <typename T>
class DynamicArray {
private:
    // Mảng chứa các phần tử kiểu T
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
    // Constructor
    DynamicArray(int initialCapacity = 10) {
        currentCapacity = (initialCapacity > 0) ? initialCapacity : 1;
        currentSize = 0;
        data = new T[currentCapacity];
    }

    // Destructor
    ~DynamicArray() {
            // KKhông gọi clear() nữa.
            // Hàm hủy này CHỈ chịu trách nhiệm giải phóng khối bộ nhớ mảng (data) mà chính nó đã 'new'.
        delete[] data;
    }

    // Hàm push_back
    void push_back(T value) {
        if (currentSize == currentCapacity) {
        resize();
        }
        data[currentSize++] = value;
    }

    // Toán tử []
    T& operator[](int index) {
        if (index < 0 || index >= currentSize) {
        throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    // Phiên bản const của toán tử []
    const T& operator[](int index) const {
        if (index < 0 || index >= currentSize) {
        throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    // size()
    int size() const {
        return currentSize;
    }

    // empty()
    bool empty() const {
        return currentSize == 0;
    }

    // Xóa phần tử tại một chỉ số cụ thể
    bool erase(int index) {
        if (index < 0 || index >= currentSize) {
        return false; // Chỉ số không hợp lệ
        }

        // KHÔNG delete data[index].
        // Lớp này không sở hữu phần tử, nó chỉ giữ.
        // Việc 'delete' là trách nhiệm của Manager khác.

        // Dịch các phần tử phía sau lên để lấp vào chỗ trống
        for (int i = index; i < currentSize - 1; ++i) {
        data[i] = data[i + 1];
        }
        
        // Giảm kích thước mảng
        currentSize--;
        return true;
    }

    // Xóa tất cả các phần tử khỏi mảng
    void clear() {
        // Chỉ Reset kích thước về 0, không delete từng phần tử.
        currentSize = 0;
    }
};