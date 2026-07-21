//
// Created by Rohan Khanna on 4/29/26.
//
//Version 1.1
#include <iostream>
#include "Account.h"
#include "AccountManager.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include "Utils.h"

Account::Account() {
    balance = 0;

    holderName = Utils::getLineInput("Enter the holder's full name: \n");

    initializeAccountType();
    initializeAccountNumber();

    int pin = 0;
    std::cout<<"Set a 4-digit pin: \n";
    while (Utils::getDigitCount(pin) != 4) {
        pin = Utils::getIntInput("");
        if (Utils::getDigitCount(pin)==4) {
            std::cout<<"Pin set. Your pin is "+std::to_string(pin)+".\n";
            break;
        }
        else {
            std::cout<<"Pin must be exactly 4 digits. Enter a valid pin: \n\n";
        }
    }
    Account::pin = pin;

    std::cout<<"Your account has been created with the following details: \n";
    std::cout<<"Account number: "<<accountNumber<<"\nHolder name: "<<holderName<<"\nAccount type: "<<accountType<<"\nBalance: $"<<balance<<"\n";
}

std::string Account::getHolderName() {
    return holderName;
}

void Account::initializeAccountNumber() {
    srand(time(NULL)); //Not truly random, update later
    std::string prefix = std::to_string((rand() % 999999) + 100000); //6-digit random number.
    /*To implement if there are concerns regarding duplicate random numbers,
    but all accounts are already unique because of last 6 digits referring
    to the arrayList position of the account*/
    /*bool isUnique = false;
    while (!isUnique) {
        //Code to check if prefix is already taken
    }*/



    std::string suffix = std::to_string(AccountManager::accounts.size());

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
        default:
            middleDigits = "00000";
    }

    accountNumber = prefix + middleDigits + suffix;
    //Limits to 1 million accounts that can be assigned
}

void Account::initializeAccountType() {
    int accountTypeInt = 0;
    std::string accountTypeString;
    std::cout<<"Is this a checking (Enter 1) or savings (Enter 2) account?\n";
    while(true) {
        accountTypeInt = Utils::getIntInput("");

        switch (accountTypeInt) {
            case 1:
                accountTypeString = "checking";
                std::cout<<"Account is a "<<accountTypeString<<" account.\n";
                accountType = accountTypeString;
                return;
            case 2:
                accountTypeString = "savings";
                std::cout<<"Account is a "<<accountTypeString<<" account.\n";
                accountType = accountTypeString;
                return;
            default:
                std::cout<<"Invalid input. Enter an integer, either 1 or 2:\n";
        }
    }
}

std::string Account::getAccountNumber() {
    return accountNumber;
}

double Account::getBalance() const{
    return balance;
}

void Account::deposit(double amount) {
    if (amount<=0) {
        std::cout<<"Invalid amount. Transaction unsuccessful.\n";
    }
    else {
        balance+=amount;
        std::cout<<"Deposit successful.\n";
    }
}
void Account::withdraw(double amount) {
    if (amount>balance) {
        std::cout<<"Insufficient funds. Transaction unsuccessful.\n";
    }
    else {
        balance-=amount;
        std::cout<<"Withdrawal successful.\n";
    }
}
/*void Account::transfer(Account& toAccount, double amount) {
    toAccount.deposit(amount);
    withdraw(amount);
}*/

void Account::displayDetails() {
    if (!checkPin()) {
        return;
    }
    std::cout<<"Account number: "<<accountNumber<<"\nHolder name: "<<holderName<<"\nAccount type: "<<accountType<<"\nBalance: "<<balance<<"\n";
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
    int userPin;
    std::cout<<"Enter your pin to continue: \n";
    for(int i=0;i<3;i++) {
        userPin = Utils::getIntInput("");
        if(userPin == pin) {
            std::cout<<"Access granted.\n";
            return true;
        }
        else {
            i==2 ? std::cout<<"Too many attempts. You have been locked out.\n" : std::cout<<"Invalid pin, try again.\n";
        }
    }
    return false;
}
void Account::changePin() {//not implemented yet
    if (checkPin()) {
        int newPin = Utils::getIntInput("Enter new pin: \n");
        pin = newPin;
        std::cout<<"Pin changed successfully.\n";
    }
    else {
        std::cout<<"Not authorized.\n";
    }
}
