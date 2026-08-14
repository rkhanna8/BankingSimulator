#ifndef BANKINGSIMULATOR_PINHASHER_H
#define BANKINGSIMULATOR_PINHASHER_H

#include <string>

class PinHasher {
public:
    static std::string hash(const std::string& pin);
    static bool verify(
        const std::string& encodedHash,
        const std::string& candidatePin
    );
};

#endif //BANKINGSIMULATOR_PINHASHER_H
