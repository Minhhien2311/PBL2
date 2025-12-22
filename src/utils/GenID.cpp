#include "utils/GenID.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace IdGenerator {

    static int adminCounter = 0;
    static int agentCounter = 0;
    static int instanceCounter = 0;
    static int bookingCounter = 0;

    void loadCountersFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            size_t delimiterPos = line.find(':');
            if (delimiterPos == std::string::npos) {
                continue;
            }

            std::string key = line.substr(0, delimiterPos);
            int value = std::stoi(line.substr(delimiterPos + 1));
            if (key == "ADMIN_COUNTER") {
                adminCounter = value;
            } else if (key == "AGENT_COUNTER") {
                agentCounter = value;
            } else if (key == "INSTANCE_COUNTER") {
                instanceCounter = value;
            } else if (key == "BOOKING_COUNTER") {
                bookingCounter = value;
            }
        }
        file.close();
    }

    void saveCountersToFile(const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return;
        }

        file << "ADMIN_COUNTER:" << adminCounter << "\n";
        file << "AGENT_COUNTER:" << agentCounter << "\n";
        file << "INSTANCE_COUNTER:" << instanceCounter << "\n";
        file << "BOOKING_COUNTER:" << bookingCounter << "\n";

        file.close();
    }

    std::string generateAdminId() {
        ++adminCounter;

        std::stringstream ss;
        ss << "ADM-" 
           << std::setw(3) << std::setfill('0') << adminCounter;
        
        return ss.str();
    }

    std::string generateAgentId() {
        ++agentCounter;

        std::stringstream ss;
        ss << "AGT-" 
           << std::setw(3) << std::setfill('0') << agentCounter;
           
        return ss.str();
    }

    std::string generateFlightId() {
        ++instanceCounter;
        
        std::stringstream ss;
        ss << "FI-" << std::setw(5) << std::setfill('0') << instanceCounter;
        return ss.str();
    }

    std::string generateBookingId() {
        ++bookingCounter;
        
        std::stringstream ss;
        ss << "BKG-" << std::setw(7) << std::setfill('0') << bookingCounter;
        return ss.str();
    }
}