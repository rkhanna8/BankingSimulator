#ifndef BANKINGSIMULATOR_SECURERANDOM_H
#define BANKINGSIMULATOR_SECURERANDOM_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class SecureRandom {
public:
    static std::vector<std::uint8_t> bytes(std::size_t count);
    static std::string numericString(std::size_t digitCount);
};

#endif //BANKINGSIMULATOR_SECURERANDOM_H
