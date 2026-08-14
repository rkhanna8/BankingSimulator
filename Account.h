//
// Created by Rohan Khanna on 4/29/26.
//

#ifndef BANKINGSIMULATOR_ACCOUNT_H
#define BANKINGSIMULATOR_ACCOUNT_H

#include <string>
#include <vector>
#include "Transaction.h"

class Account {
private:
    long long balanceCents;
    std::string pinHash;
    std::string accountNumber;
    std::string holderName;
    std::string accountType;
    void initializeAccountNumber();

public:
    Account(std::string holderName, std::string accountType, std::string pin);

    std::vector<Transaction> transactions;

    void deposit(long long amountCents);
    void withdraw(long long amountCents);
    long long getBalanceCents() const;
    bool verifyPin(const std::string& candidatePin) const;
    const std::string& getPinHash() const;
    bool checkPin() const;
    void changePin();
    void getTransactionHistory();
    const std::string& getHolderName() const;
    const std::string& getAccountType() const;
    const std::string& getAccountNumber() const;
    void displayDetails();
};

#endif //BANKINGSIMULATOR_ACCOUNT_H
