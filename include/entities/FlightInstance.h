#ifndef FLIGHT_INSTANCE_H
#define FLIGHT_INSTANCE_H

#include <string>

// FlightInstance: MỘT LẦN BAY CỤ THỂ của một Flight theo ngày/giờ.
// Ghế/giá/sức chứa gắn với từng lần bay (khác Flight tuyến).
class FlightInstance {
private:
    std::string instanceId;   // ví dụ: VN123-2025-10-10T08:00
    std::string flightId;     // tham chiếu sang Flight (id nội bộ ổn định)
    std::string departureIso; // "YYYY-MM-DDTHH:MM:SSZ" (lịch dự kiến)
    std::string arrivalIso;   // "YYYY-MM-DDTHH:MM:SSZ" (lịch dự kiến)

    int capacity;             // tổng số ghế cấu hình cho lần bay
    int seatsAvailable;       // số ghế còn trống (update khi đặt/hủy)

    double fareEconomy;
    double fareBusiness;

public:
    FlightInstance(const std::string& instanceId,
                   const std::string& flightId,
                   const std::string& departureIso,
                   const std::string& arrivalIso,
                   int capacity,
                   int seatsAvailable,
                   double fareEconomy,
                   double fareBusiness);

    // Getter
    const std::string& getInstanceId()      const;
    const std::string& getFlightId()        const;
    const std::string& getDepartureIso()    const;
    const std::string& getArrivalIso()      const;
    int                getCapacity()        const;
    int                getSeatsAvailable()  const;
    double getFareEconomy()                 const;
    double getFareBusiness()                const;

    // Setter/Updater tối thiểu (nghiệp vụ đặt vé/đổi vé để ở service)
    void setSeatsAvailable(int remaining);
    void setFareEconomy(double fare);
    void setFareBusiness(double fare);

    // Tiện ích nhẹ
    bool hasAvailableSeats() const;     // seatsAvailable > 0 ?
    void displayInfo() const;           // chỉ cout thông tin; không dính DB/I/O khác
};

#endif // FLIGHT_INSTANCE_H
