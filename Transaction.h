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

    Transaction();

    std::string getType() const;
    double getTransAmount() const;
    double getBalanceAfter() const;
    //std::string getNote() const;
    void initializeTransactionType();

    void display() const;


};



#endif //BANKINGSIMULATOR_TRANSACTION_H
