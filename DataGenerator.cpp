#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <algorithm> // shuffle
#include <random>    // random_device, mt19937

using namespace std;

// --- CẤU TRÚC DỮ LIỆU NỘI BỘ ĐỂ LƯU TRỮ TẠM ---
struct FlightGenData {
    string id;
    string departureDate; // DD/MM/YYYY
    int totalSeats;
    int availableSeats;
    long long priceEco;
    int bookedCount; 
};

struct PassengerGenData {
    string id;
    string name;
    string dob;
    string phone;
};

// --- CẤU HÌNH ---
const int YEAR_GEN = 2025;
// const int TOTAL_FLIGHTS = 1000; // <-- ĐÃ BỎ CONST NÀY ĐỂ NHẬP TAY

// --- HÀM HỖ TRỢ RANDOM ---
int randomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

string randomString(int length, bool digitOnly = true) {
    string s = "";
    string alpha = "0123456789";
    if (!digitOnly) alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < length; ++i) s += alpha[rand() % alpha.length()];
    return s;
}

// --- 1. CHUẨN HÓA TÊN TIẾNG VIỆT ---
string randomName() {
    static const vector<string> HO = {
        "Nguyễn", "Trần", "Lê", "Phạm", "Hoàng", "Huỳnh", "Phan", "Vũ", "Võ", "Đặng", 
        "Bùi", "Đỗ", "Hồ", "Ngô", "Dương", "Lý"
    };
    static const vector<string> DEM = {
        "Văn", "Thị", "Minh", "Đức", "Ngọc", "Xuân", "Thu", "Hải", "Tuấn", "Hoàng", 
        "Phương", "Thanh", "Hữu", "Gia", "Khánh"
    };
    static const vector<string> TEN = {
        "Hùng", "Lan", "Hương", "Tuấn", "Dũng", "Minh", "Hạnh", "Thảo", "Trang", 
        "Hiếu", "Đạt", "Kiên", "Cường", "My", "Linh", "Nam", "Giang", "Sơn", "Quân", "Tú"
    };

    string h = HO[randomInt(0, HO.size() - 1)];
    string d = DEM[randomInt(0, DEM.size() - 1)];
    string t = TEN[randomInt(0, TEN.size() - 1)];
    
    return h + " " + d + " " + t;
}

string randomPhone() {
    static const vector<string> DAU_SO = {"090", "091", "098", "097", "032", "070", "089"};
    return DAU_SO[randomInt(0, DAU_SO.size()-1)] + randomString(7);
}

string randomDob() {
    int year = YEAR_GEN - randomInt(18, 60);
    int month = randomInt(1, 12);
    int day = randomInt(1, 28);
    stringstream ss;
    ss << setfill('0') << setw(2) << day << "/" << setw(2) << month << "/" << year;
    return ss.str();
}

// --- HỖ TRỢ NGÀY GIỜ ---
void addMinutes(tm& timeInfo, int minutes) {
    timeInfo.tm_min += minutes;
    mktime(&timeInfo); 
}

string formatDate(const tm& t) {
    stringstream ss;
    ss << setfill('0') << setw(2) << t.tm_mday << "/" 
       << setfill('0') << setw(2) << (t.tm_mon + 1) << "/" 
       << (t.tm_year + 1900);
    return ss.str();
}

string formatTime(const tm& t) {
    stringstream ss;
    ss << setfill('0') << setw(2) << t.tm_hour << ":" 
       << setfill('0') << setw(2) << t.tm_min;
    return ss.str();
}

string getBookingTime(string flightDateStr) {
    stringstream ss;
    ss << flightDateStr << " " 
       << setfill('0') << setw(2) << randomInt(0, 23) << ":" 
       << setw(2) << randomInt(0, 59) << ":00";
    return ss.str();
}

// --- 2. LOGIC TẠO MÃ GHẾ (CHỮ + SỐ 2 DIGIT) ---
vector<string> generateFullSeatList(int totalCapacity) {
    vector<string> seats;
    int numRows = totalCapacity / 8;
    string cols = "ABCDEFGH"; // 8 Cột

    for (int r = 1; r <= numRows; ++r) {
        for (char c : cols) {
            stringstream ss;
            // Format: A01, A10, H05...
            ss << c << setfill('0') << setw(2) << r; 
            seats.push_back(ss.str());
        }
    }
    return seats;
}

// --- CONST DATA ---
struct Airline { string name; string codePrefix; };
struct RouteData { string routeCode; int durationMins; int basePrice; int weight; };

// ================== MAIN PROGRAM ==================
int main() {
    srand(time(0));
    random_device rd;
    mt19937 g(rd());

    // --- NHẬP SỐ LƯỢNG CHUYẾN BAY ---
    int n = 0;
    cout << "Nhap so luong chuyen bay muon sinh (n): ";
    if (!(cin >> n)) {
        cout << "Nhap sai! Mac dinh sinh 100 chuyen." << endl;
        n = 100;
    }
    if (n <= 0) n = 100;

    cout << "=== BAT DAU SINH " << n << " CHUYEN BAY ===" << endl;

    // --- DATA MẪU ---
    vector<Airline> airlines = {
        {"Vietnam Airlines", "VN"}, {"VietJet Air", "VJ"}, {"Bamboo Airways", "QH"},
        {"Pacific Airlines", "BL"}, {"Vietravel Airlines", "VU"}
    };

    vector<RouteData> routes = {
        {"HAN-SGN", 130, 2500000, 50}, {"SGN-HAN", 130, 2500000, 50},
        {"HAN-DAD", 80, 1500000, 40},  {"DAD-HAN", 80, 1500000, 40},
        {"SGN-DAD", 85, 1500000, 40},  {"DAD-SGN", 85, 1500000, 40},
        {"HAN-PQC", 125, 3000000, 30}, {"PQC-HAN", 125, 3000000, 30},
        {"SGN-PQC", 60, 1200000, 30},  {"PQC-SGN", 60, 1200000, 30},
        {"SGN-CXR", 65, 1000000, 30},  {"CXR-SGN", 65, 1000000, 30},
        {"SGN-DLI", 50, 900000, 30},   {"DLI-SGN", 50, 900000, 30},
        {"SGN-BKK", 90, 2500000, 20},  {"BKK-SGN", 90, 2500000, 20},
        {"HAN-ICN", 250, 8000000, 20}, {"ICN-HAN", 250, 8000000, 20},
        {"SGN-SIN", 120, 3500000, 20}, {"SIN-SGN", 120, 3500000, 20}
    };
    int totalWeight = 0;
    for (const auto& r : routes) totalWeight += r.weight;

    // --- GENERATE FLIGHTS ---
    vector<FlightGenData> generatedFlights;
    ofstream fOut("data/flights.txt");
    if (!fOut.is_open()) { cerr << "Loi: Tao folder 'data' truoc khi chay!" << endl; return 1; }

    long long totalBookedSeatsGlobal = 0; 

    // Mốc thời gian hiện tại giả lập: 20/11/2025
    tm currentTm = {};
    currentTm.tm_year = 125; currentTm.tm_mon = 10; currentTm.tm_mday = 20;
    mktime(&currentTm);
    time_t nowTime = mktime(&currentTm);

    // VÒNG LẶP CHẠY THEO BIẾN n VỪA NHẬP
    for (int i = 1; i <= n; ++i) {
        int rVal = randomInt(0, totalWeight - 1);
        int currentWeight = 0, rIdx = 0;
        for (int j = 0; j < routes.size(); ++j) {
            currentWeight += routes[j].weight;
            if (rVal < currentWeight) { rIdx = j; break; }
        }
        RouteData route = routes[rIdx];
        Airline airline = airlines[randomInt(0, airlines.size() - 1)];

        tm depTm = {};
        depTm.tm_year = 125; depTm.tm_mon = 0; depTm.tm_mday = 1;
        mktime(&depTm);
        depTm.tm_mday += randomInt(0, 364); 
        depTm.tm_hour = randomInt(5, 23); 
        depTm.tm_min = randomInt(0, 11) * 5;
        time_t flightTime = mktime(&depTm);
        
        tm arrTm = depTm;
        addMinutes(arrTm, route.durationMins);
        string flightNo = airline.codePrefix + to_string(randomInt(100, 999));

        int numRows = randomInt(20, 40); 
        int totalCapacity = numRows * 8;

        double diffSeconds = difftime(flightTime, nowTime);
        double occupancyRate = 0.0;
        if (diffSeconds < 0) occupancyRate = 0.80 + (rand() % 19) / 100.0; 
        else if (diffSeconds < 7 * 24 * 3600) occupancyRate = 0.50 + (rand() % 36) / 100.0;
        else occupancyRate = (rand() % 41) / 100.0;

        int bookedSeats = (int)(totalCapacity * occupancyRate);
        int availableSeats = totalCapacity - bookedSeats;
        if (availableSeats < 0) availableSeats = 0;

        totalBookedSeatsGlobal += bookedSeats;

        double priceVar = 0.85 + (rand() % 31) / 100.0;
        long long ecoPrice = (long long)(route.basePrice * priceVar / 1000) * 1000;
        double businessMultiplier = 1.8 + (rand() % 8) / 10.0; 
        long long busPrice = (long long)(ecoPrice * businessMultiplier / 1000) * 1000;

        stringstream idSs; idSs << "FI-" << setfill('0') << setw(5) << i;
        string fId = idSs.str();
        string depDateStr = formatDate(depTm);

        FlightGenData fg;
        fg.id = fId;
        fg.departureDate = depDateStr;
        fg.totalSeats = totalCapacity;
        fg.availableSeats = availableSeats;
        fg.priceEco = ecoPrice;
        fg.bookedCount = bookedSeats;
        generatedFlights.push_back(fg);

        fOut << fId << "|" << route.routeCode << "|" << airline.name << "|" << flightNo << "|"
             << depDateStr << "|" << formatTime(depTm) << "|"
             << formatDate(arrTm) << "|" << formatTime(arrTm) << "|"
             << totalCapacity << "|" << availableSeats << "|" << ecoPrice << "|" << busPrice << endl;
    }
    fOut.close();

    // --- GENERATE PASSENGERS ---
    long long numPassengers = totalBookedSeatsGlobal * 0.4;
    if (numPassengers < 1000) numPassengers = 1000;

    cout << "-> Dang sinh " << numPassengers << " hanh khach..." << endl;
    vector<PassengerGenData> passengers;
    ofstream pOut("data/passengers.txt");
    
    for (long long i = 0; i < numPassengers; ++i) {
        PassengerGenData p;
        p.id = "0" + randomString(11); 
        p.name = randomName();
        p.dob = randomDob();
        p.phone = randomPhone();
        passengers.push_back(p);
        
        pOut << p.id << "|" << p.name << "|" << p.dob << "|" << p.phone << endl;
    }
    pOut.close();

    // --- GENERATE BOOKINGS & SEAT MAPS ---
    cout << "-> Dang sinh Booking va Seat Map..." << endl;
    
    ofstream bOut("data/bookings.txt");
    ofstream sOut("data/seat_maps.txt");

    long long bkgCount = 1;

    for (const auto& f : generatedFlights) {
        if (f.bookedCount <= 0) {
            sOut << f.id << "|" << endl;
            continue; 
        }

        vector<string> allSeats = generateFullSeatList(f.totalSeats);
        shuffle(allSeats.begin(), allSeats.end(), g);

        vector<string> bookedSeatCodes;
        for(int k=0; k < f.bookedCount; ++k) {
            bookedSeatCodes.push_back(allSeats[k]);
        }

        // Ghi seat_maps.txt
        vector<string> sortedSeats = bookedSeatCodes;
        sort(sortedSeats.begin(), sortedSeats.end());
        
        sOut << f.id << "|";
        for (size_t i = 0; i < sortedSeats.size(); ++i) {
            sOut << sortedSeats[i] << (i < sortedSeats.size() - 1 ? "," : "");
        }
        sOut << endl;

        // Ghi bookings.txt
        for (const string& seatCode : bookedSeatCodes) {
            const auto& p = passengers[randomInt(0, passengers.size() - 1)];
            
            stringstream bkgId;
            bkgId << "BKG-" << setfill('0') << setw(7) << bkgCount++;

            // --- LOGIC AGENT: AGT-XXX (Không có ONLINE) ---
            stringstream ssAgent;
            ssAgent << "AGT-" << setfill('0') << setw(3) << randomInt(1, 20);
            string agentId = ssAgent.str();
            // ----------------------------------------------

            int status = 1; 
            int luggage = (rand() % 3 == 0) ? 1 : 0;
            long long finalPrice = f.priceEco + (luggage * 500000);

            bOut << bkgId.str() << "|"
                 << f.id << "|"
                 << agentId << "|"
                 << p.id << "|"
                 << seatCode << "|"
                 << getBookingTime(f.departureDate) << "|"
                 << status << "|"
                 << finalPrice << "|"
                 << luggage << endl;
        }
    }

    bOut.close();
    sOut.close();

    cout << "=== HOAN TAT! Kiem tra folder data/ ===" << endl;
    return 0;
}