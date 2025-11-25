#pragma once

#include <vector>
#include <string>
#include "../entities/Flight.h"

class Sorting {
public:
    // --- Public Sorting API ---
    // Sắp xếp theo giờ đến (ArrivalTime) tăng dần
    static std::vector<Flight*> sortByArrivalTime(const std::vector<Flight*>& flights);
    
    // Sắp xếp theo giá vé Economy tăng dần
    static std::vector<Flight*> sortByPrice(const std::vector<Flight*>& flights);

    // Sắp xếp theo tên hãng bay (A-Z)
    static std::vector<Flight*> sortByAirline(const std::vector<Flight*>& flights);

private:
    // --- Helpers: So sánh & Chuyển đổi ---
    static bool compareDateTime(const std::string& date1, const std::string& time1,
                               const std::string& date2, const std::string& time2);
    static long long dateToNumber(const std::string& date);
    static int timeToMinutes(const std::string& time);
    
    // --- Internal QuickSort Implementations ---
    // Sort by Time
    static void quickSortByTime(std::vector<Flight*>& arr, int low, int high);
    static int partitionByTime(std::vector<Flight*>& arr, int low, int high);
    
    // Sort by Price
    static void quickSortByPrice(std::vector<Flight*>& arr, int low, int high);
    static int partitionByPrice(std::vector<Flight*>& arr, int low, int high);

    // Sort by Airline
    static void quickSortByAirline(std::vector<Flight*>& arr, int low, int high);
    static int partitionByAirline(std::vector<Flight*>& arr, int low, int high);
};