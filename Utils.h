//
// Created by Rohan Khanna on 7/6/26.
//

#ifndef BANKINGSIMULATOR_UTILS_H
#define BANKINGSIMULATOR_UTILS_H

#include <string>

class Utils {
    public:
        static int getIntInput(const std::string& prompt);
        static double getDoubleInput(const std::string& prompt);
        static std::string getLineInput(const std::string& prompt);
};

#endif //BANKINGSIMULATOR_UTILS_H
