#include "SecureRandom.h"

#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#endif

std::vector<std::uint8_t> SecureRandom::bytes(std::size_t count) {
    std::vector<std::uint8_t> result(count);

#ifdef _WIN32
    const NTSTATUS status = BCryptGenRandom(
        nullptr,
        result.data(),
        static_cast<ULONG>(result.size()),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );
    if (status < 0) {
        throw std::runtime_error("The operating system random generator failed.");
    }
#else
    std::ifstream randomSource("/dev/urandom", std::ios::binary);
    if (!randomSource.read(
            reinterpret_cast<char*>(result.data()),
            static_cast<std::streamsize>(result.size()))) {
        throw std::runtime_error("The operating system random generator failed.");
    }
#endif

    return result;
}

std::string SecureRandom::numericString(std::size_t digitCount) {
    if (digitCount == 0) {
        return "";
    }

    std::string result;
    result.reserve(digitCount);

    while (result.length() < digitCount) {
        for (const std::uint8_t byte : bytes(32)) {
            if (result.empty()) {
                // 252 is divisible by 9, preventing modulo bias for digits 1-9.
                if (byte < 252) {
                    result.push_back(static_cast<char>('1' + (byte % 9)));
                }
            }
            else if (byte < 250) {
                // 250 is divisible by 10, preventing modulo bias for digits 0-9.
                result.push_back(static_cast<char>('0' + (byte % 10)));
            }

            if (result.length() == digitCount) {
                break;
            }
        }
    }

    return result;
}
