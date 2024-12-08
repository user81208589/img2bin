#include "utils.h"
#include <sstream>
#include <iomanip>

std::string gridToHex(const std::vector<std::vector<int>> &grid) {
    std::ostringstream hexStream;
    
    std::string binaryString;
    for (const auto &row : grid) {
        for (int cell : row) {
            binaryString += std::to_string(cell);
        }
    }

    for (size_t i = 0; i < binaryString.size(); i += 8) {
        std::string byteString = binaryString.substr(i, 8);
        while (byteString.size() < 8) { 
            byteString += "0";
        }
        int byteValue = std::stoi(byteString, nullptr, 2);
        hexStream << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << byteValue;
    }

    return hexStream.str();
}
