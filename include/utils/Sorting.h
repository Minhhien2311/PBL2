#ifndef SORTING_H
#define SORTING_H

#include "../DSA/DynamicArray.h"
#include "../entities/FlightInstance.h"

class Sorting {
public:
    // Sắp xếp FlightInstance theo giờ bay (từ sớm tới trễ)
    // So sánh theo arrivalDate và arrivalTime
    static DynamicArray<FlightInstance*> sortByArrivalTime(const DynamicArray<FlightInstance*>& flights);
    
    // Sắp xếp FlightInstance theo giá vé Economy (từ thấp tới cao)
    static DynamicArray<FlightInstance*> sortByPrice(const DynamicArray<FlightInstance*>& flights);

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
    static void quickSortByTime(DynamicArray<FlightInstance*>& arr, int low, int high);
    static int partitionByTime(DynamicArray<FlightInstance*>& arr, int low, int high);
    
    // Quick Sort helpers cho sắp xếp theo giá
    static void quickSortByPrice(DynamicArray<FlightInstance*>& arr, int low, int high);
    static int partitionByPrice(DynamicArray<FlightInstance*>& arr, int low, int high);
};

#endif

