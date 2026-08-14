//
// Created by Rohan Khanna on 6/11/26.
//

#ifndef BANKINGSIMULATOR_ACCOUNTMANAGER_H
#define BANKINGSIMULATOR_ACCOUNTMANAGER_H

#include <vector>
#include <string>
#include "Account.h"


class AccountManager {
    public:
        static std::vector<Account> accounts;

        // Web/API-facing operations: these do not read from or write to the terminal.
        static Account& createAccount(
            const std::string& holderName,
            const std::string& accountType,
            const std::string& pin
        );
        static Account* findByAccountNumber(const std::string& accountNumber);
        static bool removeByAccountNumber(const std::string& accountNumber);

        // Terminal-facing wrappers kept for the original console interface.
        static Account& searchAccounts();
        static void deleteAccount();
        static void createAccount();
};



#endif //BANKINGSIMULATOR_ACCOUNTMANAGER_H
