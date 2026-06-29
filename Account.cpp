//
// Created by Rohan Khanna on 4/29/26.
//
//Version 1.1
#include <iostream>
#include "Account.h"

Account::Account(double initialBalance) {
    balance = initialBalance;
    std::string holderName;
    std::cout<<"Enter the holder's full name: "<<std::endl;
    std::cin>>holderName;
    Account::holderName = holderName;

    int accountTypeInt = 0;
    std::string accountTypeString;
    std::cout<<"Is this a checking (Enter 1) or savings (Enter 2) account?"<<std::endl;
    while(true) {
        std::cin>>accountTypeInt;
        if (accountTypeInt == 1) {
            accountTypeString = "checking";
            break;
        }
        else if (accountTypeInt == 2) {
            accountTypeString = "savings";
            break;
        }
        else {
            std::cout<<"Invalid input. Enter an integer, either 1 or 2: "<<std::endl;
        }
    }
    std::cout<<"Account is a "<<accountTypeString<<" account."<<std::endl;
    Account::accountType = accountTypeString;

    //transactions.insert();
    new Transaction("deposit",initialBalance,initialBalance,"Account initialization.");
    //Check if the above transaction actually adds a transaction to right account.
    //Learn more about arraylist amendments.

    //How should account number assignment be done? How to save offline, manage a ton of accounts?
    //Could be a 9-digit number, maybe simply just 000000001, then 000000002, and so on?
    //Should I keep a global variable to track previous account initializations chronologically?
    //Research how real banks assign account numbers?

    int pin = 0;
    std::cout<<"Set a 4-digit pin: "<<std::endl;
    while (getDigitCount(pin) != 4) {
        std::cin>>pin;
        if (getDigitCount(pin)==4) {
            std::cout<<"Pin set. Your pin is "+std::to_string(pin)+"."<<std::endl;
            break;
        }
        else {
            std::cout<<"Pin must be exactly 4 digits. Enter a valid pin: "<<std::endl;
        }
    }
    Account::pin = pin;


    std::cout<<"Your account with an initial balance of $"<<initialBalance<<
        "has been created."<<std::endl;
}

std::string Account::getHolderName() {
    return holderName;
}

void Account::initializeAccountNumber() {
    srand(time(NULL));
    std::string prefix = (rand() % 999999) + 100000 + ""; //6-digit random number.
    /*To implement if there are concerns regarding duplicate random numbers,
    but all accounts are already uniqye because of last 6 digits referring
    to the arrayList position of the account*/
    /*bool isUnique = false;
    while (!isUnique) {
        //Code to check if prefix is already taken
    }*/

    std::string suffix = AccountManager::accounts.size()+"";

    //Determine digits to go in between prefix and suffix to keep a consistent 12-digit number
    std::string middleDigits;
    switch (suffix.length()) {
        case 1:
            middleDigits = "00000";
            break;
        case 2:
            middleDigits = "0000";
            break;
        case 3:
            middleDigits = "000";
            break;
        case 4:
            middleDigits = "00";
            break;
        case 5:
            middleDigits = "0";
            break;
        case 6:
            middleDigits = "";
            break;
    }

    accountNumber = prefix + middleDigits + suffix;
    //Limits to 1 million accounts that can be assigned
}

void Account::initializeAccountType() {
    int accountTypeInt = 0;
    std::string accountTypeString;
    std::cout<<"Is this a checking (Enter 1) or savings (Enter 2) account?"<<std::endl;
    while(true) {
        std::cin>>accountTypeInt;
        if (accountTypeInt == 1) {
            accountTypeString = "checking";
            break;
        }
        else if (accountTypeInt == 2) {
            accountTypeString = "savings";
            break;
        }
        else {
            std::cout<<"Invalid input. Enter an integer, either 1 or 2: "<<std::endl;
        }
    }
    std::cout<<"Account is a "<<accountTypeString<<" account."<<std::endl;
    Account::accountType = accountTypeString;
}

int Account::getDigitCount(int num) {
    int digitCount = 0;
    while (num/10 >= 1) {
        digitCount++;
        num=num/10;
    }
    return digitCount;
}

double Account::deposit(double amount) {
    if (amount<=0) {
        std::cout<<"Invalid amount. Transaction unsuccessful.";
    }
    else {
        balance+=amount;
        std::cout<<"Deposit successful.";
    }
    return balance;
}
double Account::withdraw(double amount) {
    if (amount>balance) {
        std::cout<<"Insufficient funds. Transaction unsuccessful.";
    }
    else {
        balance-=amount;
        std::cout<<"Withdrawal successful";
    }
    return balance;
}
double Account::getBalance() const{
    return balance;
}
void Account::transfer(Account& toAccount, double amount) {
    toAccount.deposit(amount);
    withdraw(amount);
}
void Account::addTransaction() {
    std::string type;
    std::cout<<"Enter type of transaction: ";
    std::cin>>type;
    double trans_amount;
    std::cout<<"Enter amount: ";
    std::cin>>trans_amount;
    double balanceAfter;
    if (type=="Deposit") {
        balanceAfter = deposit(trans_amount);
    }
    else if (type=="Withdrawal") {
        balanceAfter = withdraw(trans_amount);
    }
    //if ()

    std::string note;
    std::cout<<"Enter note: ";
    std::cin>>note;
    transactions.push_back(Transaction(type, trans_amount, balanceAfter, note));
}
bool Account:: checkPin() {
    int userPin;
    std::cout<<"Enter your pin to continue";
    for(int i=0;i<3;i++) {
        std::cin>>userPin;
        if(userPin == Account::pin) {
            std::cout<<"Access granted.";
            return true;
        }
        else {
            i==2 ? std::cout<<"Too many attempts. You have been locked out." : std::cout<<"Invalid pin, try again";
        }
    }
    return false;
}
void Account::changePin() {
    if (checkPin()) {
        int newPin;
        std::cout<<"Enter new pin";
        std::cin>>newPin;
        Account::pin = newPin;
        std::cout<<"Pin changed successfully";
    }
    else {
        std::cout<<"Not authorized.";
    }
}

