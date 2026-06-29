//
// Created by Rohan Khanna on 6/11/26.
//

#ifndef BANKINGSIMULATOR_ACCOUNTMANAGER_H
#define BANKINGSIMULATOR_ACCOUNTMANAGER_H

#include <vector>


class AccountManager {
    public:
        AccountManager();
        static std::vector<Account> accounts;
        static long long searchAccounts(); //returns vector index of found account
private:

};



#endif //BANKINGSIMULATOR_ACCOUNTMANAGER_H
