//
// Created by Rohan Khanna on 4/29/26.
//
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>
#include "Account.h"
#include "PinHasher.h"
#include "SecureRandom.h"
#include "Utils.h"

namespace {
std::string trim(std::string value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }).base();

    if (first >= last) {
        return "";
    }
    return std::string(first, last);
}

bool containsControlCharacter(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](char c) {
        return std::iscntrl(static_cast<unsigned char>(c));
    });
}
}

Account::Account(
    std::string holderName,
    std::string accountType,
    std::string pin
)
    : balanceCents(0),
      holderName(std::move(holderName)),
      accountType(std::move(accountType)) {
    this->holderName = trim(std::move(this->holderName));

    if (this->holderName.empty()) {
        throw std::invalid_argument("Holder name is required.");
    }
    if (this->holderName.length() > 100 ||
        containsControlCharacter(this->holderName)) {
        throw std::invalid_argument(
            "Holder name must be 100 characters or fewer and cannot contain "
            "control characters."
        );
    }

    if (this->accountType != "checking" && this->accountType != "savings") {
        throw std::invalid_argument("Account type must be checking or savings.");
    }

    if (pin.length() != 4 ||
        pin.find_first_not_of("0123456789") != std::string::npos) {
        throw std::invalid_argument("PIN must contain exactly four digits.");
    }

    pinHash = PinHasher::hash(pin);
    initializeAccountNumber();
}

const std::string& Account::getHolderName() const {
    return holderName;
}

void Account::initializeAccountNumber() {
    accountNumber = SecureRandom::numericString(12);
}

const std::string& Account::getAccountNumber() const {
    return accountNumber;
}

const std::string& Account::getAccountType() const {
    return accountType;
}

long long Account::getBalanceCents() const {
    return balanceCents;
}

bool Account::verifyPin(const std::string& candidatePin) const {
    return PinHasher::verify(pinHash, candidatePin);
}

const std::string& Account::getPinHash() const {
    return pinHash;
}

void Account::deposit(long long amountCents) {
    if (amountCents <= 0) {
        std::cout<<"Invalid amount. Transaction unsuccessful.\n";
    }
    else {
        balanceCents += amountCents;
        std::cout<<"Deposit successful.\n";
    }
}
void Account::withdraw(long long amountCents) {
    if (amountCents <= 0) {
        std::cout<<"Invalid amount. Transaction unsuccessful.\n";
    }
    else if (amountCents > balanceCents) {
        std::cout<<"Insufficient funds. Transaction unsuccessful.\n";
    }
    else {
        balanceCents -= amountCents;
        std::cout<<"Withdrawal successful.\n";
    }
}
void Account::displayDetails() {
    if (!checkPin()) {
        return;
    }
    std::cout << "Account number: " << accountNumber
              << "\nHolder name: " << holderName
              << "\nAccount type: " << accountType
              << "\nBalance: $" << std::fixed << std::setprecision(2)
              << static_cast<double>(balanceCents) / 100.0 << "\n";
    std::cout<<"Would you  like to check transaction history?\n1. Yes\n2. No\n";
    int checkTransactionHistory = Utils::getIntInput("");
    while (true) {
        switch (checkTransactionHistory) {
            case 1:
                std::cout<<"Transaction history: \n";
                getTransactionHistory();
                std::cout<<"Returning to main menu...\n";
                return;
            case 2:
                std::cout<<"Returning to main menu...\n";
                return;
            default:
                std::cout<<"Invalid input. Try again.\n";
                std::cout<<"Would you  like to check transaction history?\n1. Yes\n2. No\n";
                checkTransactionHistory = Utils::getIntInput("");
        }
    }
}

void Account::getTransactionHistory() {
    for (long long i=0;i<transactions.size();i++) {
        transactions[i].display();
    }
}

bool Account::checkPin() const {
    std::cout<<"Enter your pin to continue: \n";
    for(int i=0;i<3;i++) {
        const std::string userPin = Utils::getLineInput("");
        if(verifyPin(userPin)) {
            std::cout<<"Access granted.\n";
            return true;
        }
        else {
            i==2 ? std::cout<<"Too many attempts. You have been locked out.\n" : std::cout<<"Invalid pin, try again.\n";
        }
    }
    return false;
}
void Account::changePin() {
    if (checkPin()) {
        const std::string newPin = Utils::getLineInput("Enter new 4-digit pin: \n");
        if (newPin.length() == 4 &&
            newPin.find_first_not_of("0123456789") == std::string::npos) {
            pinHash = PinHasher::hash(newPin);
            std::cout<<"Pin changed successfully.\n";
        }
        else {
            std::cout<<"PIN must contain exactly four digits.\n";
        }
    }
    else {
        std::cout<<"Not authorized.\n";
    }
}
