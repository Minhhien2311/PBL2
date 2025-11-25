#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include "../entities/Flight.h"

class Sorting {
public:
    // Sắp xếp FlightInstance theo giờ bay (từ sớm tới trễ)
    // So sánh theo arrivalDate và arrivalTime
    static std::vector<Flight*> sortByArrivalTime(const std::vector<Flight*>& flights);
    
    // Sắp xếp FlightInstance theo giá vé Economy (từ thấp tới cao, cao tới thấp ta dùng reverse của thư viện STL algorithm)
    static std::vector<Flight*> sortByPrice(const std::vector<Flight*>& flights);

    // Sắp xếp FlightInstance theo tên Hãng bay (A-Z)
    static std::vector<Flight*> sortByAirline(const std::vector<Flight*>& flights);

private:
    // Helper function để so sánh thời gian
    // Trả về true nếu time1 < time2
    static bool compareDateTime(const std::string& date1, const std::string& time1,
                               const std::string& date2, const std::string& time2);
    
    // Helper function để chuyển đổi date string "DD/MM/YYYY" sang số để so sánh
    static long long dateToNumber(const std::string& date);
    
    // Helper function để chuyển đổi time string "HH:MM" sang số phút để so sánh
    static int timeToMinutes(const std::string& time);
    
    // Quick Sort helpers cho sắp xếp theo thời gian
    static void quickSortByTime(std::vector<Flight*>& arr, int low, int high);
    static int partitionByTime(std::vector<Flight*>& arr, int low, int high);
    
    // Quick Sort helpers cho sắp xếp theo giá
    static void quickSortByPrice(std::vector<Flight*>& arr, int low, int high);
    static int partitionByPrice(std::vector<Flight*>& arr, int low, int high);

    // Quick Sort helpers cho sắp xếp theo hãng bay
    static void quickSortByAirline(std::vector<Flight*>& arr, int low, int high);
    static int partitionByAirline(std::vector<Flight*>& arr, int low, int high);
};

#endif

