#include "PinHasher.h"

#include <cstdint>
#include <stdexcept>
#include <vector>

#include <argon2.h>

#include "SecureRandom.h"

namespace {
constexpr std::uint32_t timeCost = 2;
constexpr std::uint32_t memoryCostKiB = 19 * 1024;
constexpr std::uint32_t parallelism = 1;
constexpr std::size_t saltLength = 16;
constexpr std::size_t hashLength = 32;

}

std::string PinHasher::hash(const std::string& pin) {
    const std::vector<std::uint8_t> salt = SecureRandom::bytes(saltLength);
    const std::size_t encodedLength = argon2_encodedlen(
        timeCost,
        memoryCostKiB,
        parallelism,
        static_cast<std::uint32_t>(salt.size()),
        hashLength,
        Argon2_id
    );

    std::vector<char> encodedHash(encodedLength);
    const int result = argon2id_hash_encoded(
        timeCost,
        memoryCostKiB,
        parallelism,
        pin.data(),
        pin.size(),
        salt.data(),
        salt.size(),
        hashLength,
        encodedHash.data(),
        encodedHash.size()
    );

    if (result != ARGON2_OK) {
        throw std::runtime_error(
            std::string("Could not hash PIN: ") + argon2_error_message(result)
        );
    }

    return encodedHash.data();
}

bool PinHasher::verify(
    const std::string& encodedHash,
    const std::string& candidatePin
) {
    const int result = argon2id_verify(
        encodedHash.c_str(),
        candidatePin.data(),
        candidatePin.size()
    );

    if (result == ARGON2_OK) {
        return true;
    }
    if (result == ARGON2_VERIFY_MISMATCH) {
        return false;
    }

    throw std::runtime_error(
        std::string("Could not verify PIN: ") + argon2_error_message(result)
    );
}
