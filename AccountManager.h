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
        AccountManager();
        static std::vector<Account> accounts;
        static Account& searchAccounts();
        static void deleteAccount();
        static void createAccount();
};



#endif //BANKINGSIMULATOR_ACCOUNTMANAGER_H
