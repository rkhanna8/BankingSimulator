//
// Created by Rohan Khanna on 5/1/26.
//

#ifndef BANKINGSIMULATOR_TRANSACTION_H
#define BANKINGSIMULATOR_TRANSACTION_H
#include <iostream>


class Transaction {

private:
    std::string type;
    long long transAmountCents;
    long long balanceAfterCents;
public:
    Transaction();

    std::string getType() const;
    long long getTransAmountCents() const;
    long long getBalanceAfterCents() const;
    void initializeTransactionType();
    void display() const;
};

#endif //BANKINGSIMULATOR_TRANSACTION_H
