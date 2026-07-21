
#include "AccountManager.h"
#include <vector>
#include "Utils.h"

std::vector<Account> AccountManager::accounts;


void AccountManager::deleteAccount() {
    Account& toDelete = searchAccounts();
    if (!toDelete.checkPin()) {
        return;
    }
    size_t index = &toDelete - accounts.data();
    if (index >= accounts.size()) {
        std::cout<<"Invalid. Cannot delete account.\n";
        return;
    }
    while (true) {
        std::cout<<"Are you sure you want to delete this account? This action cannot be undone.\n";
        std::cout<<"1. Yes, delete this account\n";
        std::cout<<"2. No, do not delete this account\n";
        switch(int userInput = Utils::getIntInput("")) {
            case 1:
                accounts.erase(accounts.begin() + index);
                std::cout<<"Account deleted. Returning to main menu...\n\n";
                return;
            case 2:
                return;
            default:
                std::cout<<"Invalid input. Enter an integer 1 or 2: \n";
        }
    }
}
void AccountManager::createAccount() {
    accounts.push_back(Account());
}

Account& AccountManager::searchAccounts() {
    int searchType = 3;
    std::cout<<"Search for an account: \n";
    while (true) {
        std::cout<<"1. Search by account number.\n";
        std::cout<<"2. Search by holder name.\n";
        while(searchType<1 || searchType>2) {
            searchType = Utils::getIntInput("");
            if (searchType<1 || searchType>2) {
                std::cout<<"Invalid input. Enter an integer 1-2: \n";
            }
        }
        if (searchType==1) {
            std::string enteredAccountNumber = Utils::getLineInput("Search by 12-digit account number: \n");
            for (long long i=0;i<accounts.size();i++){ //Linear search. Look at how to implement the most efficient searching algorithm for this.
                if (accounts[i].getAccountNumber() == enteredAccountNumber) {
                    std::cout<<"Account number "<<enteredAccountNumber<<" found.\n";
                    return accounts[i];
                }
            }
            std::cout<<"Account number not found. Try again.\n";
        }
        else {
            std::string enteredHolderName = Utils::getLineInput("Search by holder name: \n");
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
                return accounts[foundIndex[0]];
            }
            else {
                std::cout<<"Multiple accounts found. Select the account from the list below by entering the corresponding integer: \n";
                for (size_t i=0;i<foundIndex.size();i++) {
                    std::cout<<"Account "<<(i+1)<<": ";
                    accounts[foundIndex[i]].displayDetails();
                }

                long long selectedAccount;

                while (true) {
                    selectedAccount = Utils::getIntInput("");
                    if (selectedAccount >= 1 && selectedAccount <= foundIndex.size()) {
                        break;
                    }
                    std::cout<<"Invalid input. Enter an integer that corresponds to a listed account number: \n";
                }
                return accounts[foundIndex[selectedAccount - 1]];
            }
        }
    }
}
