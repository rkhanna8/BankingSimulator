//
// Created by Rohan Khanna on 5/1/26.
//

#ifndef BANKINGSIMULATOR_TRANSACTION_H
#define BANKINGSIMULATOR_TRANSACTION_H
#include <iostream>


class Transaction {

private:
    std::string type;
    double trans_amount;
    double balanceAfter;
    std::string note;
public:
    std::string getType() const;
    double getAmount() const;
    double getBalanceAfter() const;
    std::string getNote() const;

    void display() const;
    Transaction(std::string type, double trans_amount, double balanceAfter, std::string note);

};



#endif //BANKINGSIMULATOR_TRANSACTION_H
