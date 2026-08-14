//
// Created by Rohan Khanna on 7/6/26.
//

#include "Utils.h"
#include <string>
#include <iostream>
#include <limits>

int Utils::getIntInput(const std::string& prompt) {
    int value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid input. Please enter an integer.\n";

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double Utils::getDoubleInput(const std::string& prompt) {
    double value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid input. Please enter a number.\n";

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string Utils::getLineInput(const std::string& prompt) {
    std::string value;

    std::cout << prompt;
    std::getline(std::cin, value);

    return value;
}
