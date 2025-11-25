#include "../../include/utils/Sorting.h"
#include <algorithm>

#include <iostream>

// Sắp xếp FlightInstance theo giờ đến (arrivalTime) từ sớm tới trễ
std::vector<Flight*> Sorting::sortByArrivalTime(const std::vector<Flight*>& flights) {
    // Tạo vector kết quả
    std::vector<Flight*> result;
    
    // Copy tất cả các phần tử vào vector kết quả
    for (int i = 0; i < flights.size(); i++) {
        result.push_back(flights[i]);
    }
    
    // Sắp xếp sử dụng Quick Sort
    if (result.size() > 1) {
        quickSortByTime(result, 0, result.size() - 1);
    }
    
    return result;
}

// Sắp xếp FlightInstance theo giá vé Economy từ thấp tới cao
std::vector<Flight*> Sorting::sortByPrice(const std::vector<Flight*>& flights) {
    // Tạo vector kết quả
    std::vector<Flight*> result;
    
    // Copy tất cả các phần tử vào vector kết quả
    for (int i = 0; i < flights.size(); i++) {
        result.push_back(flights[i]);
    }
    
    // Sắp xếp sử dụng Quick Sort
    if (result.size() > 1) {
        quickSortByPrice(result, 0, result.size() - 1);
    }
    
    return result;
}

// Sắp xếp FlightInstance theo tên Hãng bay A-Z
std::vector<Flight*> Sorting::sortByAirline(const std::vector<Flight*>& flights) {
    std::vector<Flight*> result;
    
    // Copy dữ liệu
    for (auto* f : flights) {
        result.push_back(f);
    }
    
    // Sắp xếp
    if (result.size() > 1) {
        quickSortByAirline(result, 0, result.size() - 1);
    }
    
    return result;
}

// So sánh hai thời điểm (date + time)
// Trả về true nếu datetime1 < datetime2
bool Sorting::compareDateTime(const std::string& date1, const std::string& time1,
                              const std::string& date2, const std::string& time2) {
    long long dateNum1 = dateToNumber(date1);
    long long dateNum2 = dateToNumber(date2);
    
    if (dateNum1 != dateNum2) {
        return dateNum1 < dateNum2;
    }
    
    // Nếu cùng ngày, so sánh giờ
    return timeToMinutes(time1) < timeToMinutes(time2);
}

long long Sorting::dateToNumber(const std::string& date) {
    if (date.length() < 10) return 0;

    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));

    return year * 10000LL + month * 100LL + day;
}

int Sorting::timeToMinutes(const std::string& time) {
    if (time.length() < 5) return 0;

    int hours = std::stoi(time.substr(0, 2));
    int minutes = std::stoi(time.substr(3, 2));

    return hours * 60 + minutes;
}



// ============= QUICK SORT CHO THỜI GIAN =============

// Hàm Quick Sort đệ quy cho sắp xếp theo thời gian
void Sorting::quickSortByTime(std::vector<Flight*>& arr, int low, int high) {
    if (low < high) {
        // Tìm pivot index
        int pi = partitionByTime(arr, low, high);
        
        // Sắp xếp đệ quy 2 nửa
        quickSortByTime(arr, low, pi - 1);
        quickSortByTime(arr, pi + 1, high);
    }
}

// Hàm partition cho Quick Sort theo thời gian
int Sorting::partitionByTime(std::vector<Flight*>& arr, int low, int high) {
    // Chọn phần tử cuối làm pivot
    Flight* pivot = arr[high];
    int i = low - 1; // Index của phần tử nhỏ hơn
    
    for (int j = low; j < high; j++) {
        // Nếu phần tử hiện tại nhỏ hơn hoặc bằng pivot
        if (compareDateTime(
            arr[j]->getArrivalDate(), arr[j]->getArrivalTime(),
            pivot->getArrivalDate(), pivot->getArrivalTime()
        )) {
            i++;
            // Swap arr[i] và arr[j]
            Flight* temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    // Swap arr[i+1] và arr[high] (pivot)
    Flight* temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;
}

// ============= QUICK SORT CHO GIÁ VÉ =============

// Hàm Quick Sort đệ quy cho sắp xếp theo giá
void Sorting::quickSortByPrice(std::vector<Flight*>& arr, int low, int high) {
    if (low < high) {
        // Tìm pivot index
        int pi = partitionByPrice(arr, low, high);
        
        // Sắp xếp đệ quy 2 nửa
        quickSortByPrice(arr, low, pi - 1);
        quickSortByPrice(arr, pi + 1, high);
    }
}

// Hàm partition cho Quick Sort theo giá
int Sorting::partitionByPrice(std::vector<Flight*>& arr, int low, int high) {
    // Chọn phần tử cuối làm pivot
    Flight* pivot = arr[high];
    int i = low - 1; // Index của phần tử nhỏ hơn
    
    for (int j = low; j < high; j++) {
        // Nếu phần tử hiện tại có giá nhỏ hơn hoặc bằng pivot
        if (arr[j]->getFareEconomy() <= pivot->getFareEconomy()) {
            i++;
            // Swap arr[i] và arr[j]
            Flight* temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    // Swap arr[i+1] và arr[high] (pivot)
    Flight* temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;
}

// ============= QUICK SORT CHO HÃNG BAY =============
// Hàm Quick Sort đệ quy cho sắp xếp theo hãng bay
void Sorting::quickSortByAirline(std::vector<Flight*>& arr, int low, int high) {
    if (low < high) {
        // Tìm pivot index
        int pi = partitionByAirline(arr, low, high);
        
        // Sắp xếp đệ quy 2 nửa
        quickSortByAirline(arr, low, pi - 1);
        quickSortByAirline(arr, pi + 1, high);
    }
}

// Hàm partition cho Quick Sort theo hãng bay
int Sorting::partitionByAirline(std::vector<Flight*>& arr, int low, int high) {
    // Chọn phần tử cuối làm pivot
    Flight* pivot = arr[high];
    int i = low - 1; // Index của phần tử nhỏ hơn
    
    for (int j = low; j < high; j++) {
        // Nếu phần tử hiện tại có hãng bay nhỏ hơn hoặc bằng pivot
        if (arr[j]->getAirline() <= pivot->getAirline()) {
            i++;
            // Swap arr[i] và arr[j]
            Flight* temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    // Swap arr[i+1] và arr[high] (pivot)
    Flight* temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;
}