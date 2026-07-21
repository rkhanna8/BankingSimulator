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
    std::string accountNumber;
    std::string holderName;
    std::string accountType;
    void initializeAccountType();
    void initializeAccountNumber();

public:
    Account();

    std::vector<Transaction> transactions;

    void newTransaction();
    void deposit(double amount);
    void withdraw(double amount);
    double getBalance() const; //These four are all declarations of methods that will be written in Account.cpp
    //void transfer(Account& toAccount, double amount);
    bool checkPin() const;
    void changePin();
    void getTransactionHistory();
    std::string getHolderName();
    std::string getAccountType();
    void setAccountType(std::string accountType);
    std::string getAccountNumber();
    void displayDetails();
    static int getDigitCount(int num);



};



#endif //BANKINGSIMULATOR_ACCOUNT_H
