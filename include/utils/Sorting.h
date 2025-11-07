#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include "../entities/FlightInstance.h"

class Sorting {
public:
    // Sắp xếp FlightInstance theo giờ bay (từ sớm tới trễ)
    // So sánh theo arrivalDate và arrivalTime
    static std::vector<FlightInstance*> sortByArrivalTime(const std::vector<FlightInstance*>& flights);
    
    // Sắp xếp FlightInstance theo giá vé Economy (từ thấp tới cao)
    static std::vector<FlightInstance*> sortByPrice(const std::vector<FlightInstance*>& flights);

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
    static void quickSortByTime(std::vector<FlightInstance*>& arr, int low, int high);
    static int partitionByTime(std::vector<FlightInstance*>& arr, int low, int high);
    
    // Quick Sort helpers cho sắp xếp theo giá
    static void quickSortByPrice(std::vector<FlightInstance*>& arr, int low, int high);
    static int partitionByPrice(std::vector<FlightInstance*>& arr, int low, int high);
};

#endif

