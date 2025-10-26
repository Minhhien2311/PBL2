#pragma once
#include <stdexcept>

template <typename T> // T sẽ là một kiểu dữ liệu, ví dụ: Flight* hoặc std::string
class DynamicArray {
    private:
        T* data;      // Một mảng chứa các phần tử kiểu T
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
        // Constructor (Không thay đổi)
        DynamicArray(int initialCapacity = 10) {
            currentCapacity = (initialCapacity > 0) ? initialCapacity : 1;
            currentSize = 0;
            data = new T[currentCapacity];
        }

        // Destructor (ĐÃ SỬA)
        ~DynamicArray() {
                // KHÔNG GỌI clear() nữa.
                // Hàm hủy này CHỈ chịu trách nhiệm giải phóng
                // khối bộ nhớ MẢNG (data) mà chính nó đã 'new'.
                // Nó không sở hữu các phần tử T bên trong.
            delete[] data;
        }

        // Hàm push_back (Không thay đổi)
        void push_back(T value) {
            if (currentSize == currentCapacity) {
            resize();
            }
            data[currentSize++] = value;
        }

        // Toán tử [] (Không thay đổi)
        T& operator[](int index) {
            if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
            }
            return data[index];
        }

        // Phiên bản const (Không thay đổi)
        const T& operator[](int index) const {
            if (index < 0 || index >= currentSize) {
            throw std::out_of_range("Index out of range");
            }
            return data[index];
        }

        // size() (Không thay đổi)
        int size() const {
            return currentSize;
        }

        // empty() (Không thay đổi)
        bool empty() const {
            return currentSize == 0;
        }

        // Xóa phần tử tại một chỉ số cụ thể (ĐÃ SỬA)
        bool erase(int index) {
            if (index < 0 || index >= currentSize) {
            return false; // Chỉ số không hợp lệ
            }

            // 1. (ĐÃ XÓA) KHÔNG 'delete data[index]'.
                // Lớp này không sở hữu phần tử, nó chỉ giữ.
                // Việc 'delete' là trách nhiệm của 'FlightManager'.

            // 2. Dịch các phần tử phía sau lên để lấp vào chỗ trống
            for (int i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
            }
            
            currentSize--; // Giảm kích thước mảng
            return true;
        }

        // Xóa tất cả các phần tử khỏi mảng (ĐÃ SỬA)
        void clear() {
                // (ĐÃ XÓA) Vòng lặp 'for' 'delete' các phần tử.
                // Lớp này không sở hữu chúng.
            currentSize = 0; // Chỉ Reset kích thước về 0
        }
};