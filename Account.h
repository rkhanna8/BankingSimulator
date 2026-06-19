//
// Created by Rohan Khanna on 4/29/26.
//

#ifndef BANKINGSIMULATOR_ACCOUNT_H
#define BANKINGSIMULATOR_ACCOUNT_H

#include <vector>
#include "Transaction.h"

class Account {
private:
    double balance;
    int pin;
    int accountNumber;
    std::vector<Transaction> transactions;
    std::string holderName;
    void addTransaction();
    std::string accountType;

public:
    Account(double initialBalance);
    double deposit(double amount);
    double withdraw(double amount);
    double getBalance() const; //These four are all declarations of methods that will be written in Account.cpp
    void transfer(Account& toAccount, double amount);
    bool checkPin();
    void changePin();
    void getTransactions();
    std::string getHolderName();
    std::string getAccountType();
    void setAccountType(std::string accountType);

    static int getDigitCount(int num);



};



#endif //BANKINGSIMULATOR_ACCOUNT_H
