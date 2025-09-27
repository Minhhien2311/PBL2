#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>

// Flight: tuyến bay (route) định danh bởi flightNumber (VD: VN123).
// Không gắn ngày/giờ cụ thể; ngày/giờ & số ghế & giá sẽ ở FlightInstance.
class Flight {
private:
    std::string flightId;           // id nội bộ duy nhất (nếu cần)
    std::string flightNumber;       // mã tuyến công bố: VN123
    std::string airline;            // hãng (VD: Vietnam Airlines)
    std::string departureAirport;   // điểm đi; IATA code: HAN
    std::string arrivalAirport;     // điểm đến; IATA code: SGN

public:
    Flight(const std::string& id,
           const std::string& number,
           const std::string& airline,
           const std::string& departureIATA,
           const std::string& arrivalIATA);

    // Getter (trả const-ref/int để tránh copy)
    const std::string& getFlightId()        const;
    const std::string& getFlightNumber()    const;
    const std::string& getAirline()         const;
    const std::string& getDepartureAirport()const;
    const std::string& getArrivalAirport()  const;

    // Tiện ích nhẹ (không dính I/O hoặc DB)
    std::string getFlightInfo() const; // gộp info dạng chuỗi để hiển thị nhanh
};

#endif // FLIGHT_H
