#ifndef FLIGHT_INSTANCE_H
#define FLIGHT_INSTANCE_H

#include <string>

// FlightInstance: MỘT LẦN BAY CỤ THỂ của một Flight theo ngày/giờ.
class FlightInstance {
private:
    // Định danh & lịch
    std::string instanceId;     // ví dụ: "VN123-2025-10-10T08:00"
    std::string flightId;       // tham chiếu sang Flight (id nội bộ ổn định)
    std::string departureIso;   // "YYYY-MM-DDTHH:MM:SSZ" (UTC khuyến nghị)
    std::string arrivalIso;     // "YYYY-MM-DDTHH:MM:SSZ" (UTC khuyến nghị)

    // Sức chứa tổng & khả dụng (COUNT mode)
    int capacity;               // tổng số ghế cấu hình cho lần bay
    int seatsAvailable;         // số ghế còn trống (update khi đặt/hủy)

    // Quota theo hạng (COUNT mode)
    int economyTotal;           // số ghế hạng phổ thông
    int economyAvailable;       // số ghế phổ thông còn trống
    int businessTotal;          // số ghế hạng thương gia
    int businessAvailable;      // số ghế thương gia còn trống

    // Giá cơ bản theo hạng
    double fareEconomy;
    double fareBusiness;

public:
    FlightInstance(const std::string& instanceId,
                   const std::string& flightId,
                   const std::string& departureIso,
                   const std::string& arrivalIso,
                   int capacity,
                   int seatsAvailable,
                   int economyTotal,
                   int economyAvailable,
                   int businessTotal,
                   int businessAvailable,
                   double fareEconomy,
                   double fareBusiness);

    // Getters
    const std::string& getInstanceId()      const;
    const std::string& getFlightId()        const;
    const std::string& getDepartureIso()    const;
    const std::string& getArrivalIso()      const;

    int    getCapacity()        const;
    int    getSeatsAvailable()  const;

    int    getEconomyTotal()    const;
    int    getEconomyAvail()    const;
    int    getBusinessTotal()   const;
    int    getBusinessAvail()   const;

    double getFareEconomy()     const;
    double getFareBusiness()    const;

    // Setters/Updaters (logic nghiệp vụ nằm ở service; setters chỉ gán/validate nhẹ)
    void setSeatsAvailable(int remaining);
    void setEconomyTotal(int ecoTotal);
    void setEconomyAvail(int ecoAvail);
    void setBusinessTotal(int busTotal);
    void setBusinessAvail(int busAvail);
    void setFareEconomy(double fare);
    void setFareBusiness(double fare);

    // Tiện ích nhẹ
    bool hasAvailableSeats() const;     // seatsAvailable > 0 ?
    void displayInfo() const;           // chỉ cout; tránh I/O DB ở đây
};

#endif // FLIGHT_INSTANCE_H
