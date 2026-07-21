//
// Created by Rohan Khanna on 5/1/26.
//

#include "Transaction.h"
#include "Account.h"
#include "AccountManager.h"
#include "Utils.h"

Transaction::Transaction() {
    std::cout<<"Find the account you want this transaction to be for: \n";
    Account& account = AccountManager::searchAccounts();

    if (account.checkPin()) {
        initializeTransactionType();

        double trans_amount1 = Utils::getDoubleInput("Enter the amount you want to " + type + ".\n");
        while (trans_amount1 <= 0) {
            trans_amount1 = Utils::getDoubleInput("Invalid amount. Enter a number greater than 0.\n");
        }
        trans_amount = trans_amount1;

        if (type == "deposit") {
            account.deposit(trans_amount);
        }
        else {
            account.withdraw(trans_amount);
        }
        balanceAfter = account.getBalance();

        account.transactions.push_back(*this);
    }
    else {
        std::cout<<"Pin not verified. Transaction voided.\n";
    }
}

void Transaction::initializeTransactionType() {
    std::cout<<"Enter the type of transaction: \n1. deposit\n2. withdraw\n";
    int transactionType = Utils::getIntInput("");
    while (true) {
        switch (transactionType) {
            case 1:
                type = "deposit";
                std::cout<<"Transaction type: Deposit\n";
                return;
            case 2:
                type = "withdraw";
                std::cout<<"Transaction type: Withdraw\n";
                return;
            default:
                std::cout<<"Invalid input. Try again.\n";
                std::cout<<"Enter the type of transaction: \n1. deposit\n2. withdraw\n";
                transactionType = Utils::getIntInput("");
        }
    }
}

double Transaction::getTransAmount() const {
    return trans_amount;
}
void Transaction::display() const {
    std::cout<<"Type: "<<type<<"\nAmount: "<<trans_amount<<"\nBalance after: "<<balanceAfter<<"\n";
}
double Transaction::getBalanceAfter() const {
    return balanceAfter;
}
/*std::string Transaction::getNote() const {
    return note;
}*/
std::string Transaction::getType() const {
    return type;
}
