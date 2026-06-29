//
// Created by Rohan Khanna on 6/11/26.
//

#include "AccountManager.h"
#include <vector>


long long AccountManager::searchAccounts() {
    int searchType = 3;
    while (true) {
        std::cout<<"Search for an account: \n";
        std::cout<<"Search by account number (1)\n";
        std::cout<<"Search by holder name (2)\n";
        while(searchType<1 || searchType>2) {
            std::cin>>searchType;
            if (searchType<1 || searchType>2) {
                std::cout<<"Invalid input. Enter an integer 1-2: \n";
            }
        }
        if (searchType==1) {
            std::cout<<"Search by 12-digit account number: \n";
            std::string enteredAccountNumber;
            std::cin>>enteredAccountNumber;
            for (long long i=0;i<accounts.size();i++){ //Linear search. Look at how to implement the most efficient searching algorithm for this.
                if (accounts[i].getAccountNumber() == enteredAccountNumber) {
                    std::cout<<"Account number "<<enteredAccountNumber<<" found.\n";
                    return i;
                }
            }
            std::cout<<"Account number not found. Try again.\n";
        }
        else {
            std::cout<<"Search by holder name: \n";
            std::string enteredHolderName;
            std::cin>>enteredHolderName;
            std::vector<long long> foundIndex;
            for (long long i=0;i<accounts.size();i++){ //Linear search. Look at how to implement the most efficient searching algorithm for this.
                if (accounts[i].getHolderName() == enteredHolderName) {
                    foundIndex.push_back(i);
                }
            }
            if (foundIndex.empty()) {
                std::cout<<"Account holder name not found.\n";
            }
            else if (foundIndex.size()==1) {
                return foundIndex[0];
            }
            else {
                std::cout<<"Multiple accounts found. Select the account from the list below by entering the corresponding integer: \n";
                for (size_t i=0;i<foundIndex.size();i++) {
                    std::cout<<"Account "<<(i+1)<<": ";
                    accounts[foundIndex[i]].displayDetails();
                }

                long long selectedAccount;

                while (true) {
                    std::cin>>selectedAccount;
                    if (selectedAccount >= 1 && selectedAccount <= foundIndex.size()) {
                        break;
                    }
                    std::cout<<"Invalid input. Enter an integer that corresponds to a listed account number: \n";
                }
                return foundIndex[selectedAccount - 1];
            }
        }
    }
}

