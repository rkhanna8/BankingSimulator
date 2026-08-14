
#include "AccountManager.h"
#include <iomanip>
#include <vector>
#include "Utils.h"

std::vector<Account> AccountManager::accounts;

Account& AccountManager::createAccount(
    const std::string& holderName,
    const std::string& accountType,
    const std::string& pin
) {
    accounts.emplace_back(holderName, accountType, pin);
    return accounts.back();
}

Account* AccountManager::findByAccountNumber(const std::string& accountNumber) {
    for (Account& account : accounts) {
        if (account.getAccountNumber() == accountNumber) {
            return &account;
        }
    }

    return nullptr;
}

bool AccountManager::removeByAccountNumber(const std::string& accountNumber) {
    for (auto account = accounts.begin(); account != accounts.end(); ++account) {
        if (account->getAccountNumber() == accountNumber) {
            accounts.erase(account);
            return true;
        }
    }

    return false;
}


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
    std::string holderName;
    while (holderName.empty()) {
        holderName = Utils::getLineInput("Enter the holder's full name: \n");
        if (holderName.empty()) {
            std::cout << "Holder name is required.\n";
        }
    }

    std::string accountType;
    while (accountType.empty()) {
        const int selection = Utils::getIntInput(
            "Is this a checking (Enter 1) or savings (Enter 2) account?\n"
        );

        if (selection == 1) {
            accountType = "checking";
        }
        else if (selection == 2) {
            accountType = "savings";
        }
        else {
            std::cout << "Invalid input. Enter 1 or 2.\n";
        }
    }

    std::string pin;
    while (pin.length() != 4 ||
           pin.find_first_not_of("0123456789") != std::string::npos) {
        pin = Utils::getLineInput("Set a 4-digit PIN: \n");
        if (pin.length() != 4 ||
            pin.find_first_not_of("0123456789") != std::string::npos) {
            std::cout << "PIN must contain exactly four digits.\n";
        }
    }

    const Account& account = createAccount(holderName, accountType, pin);

    std::cout << "Your account has been created with the following details:\n"
              << "Account number: " << account.getAccountNumber()
              << "\nHolder name: " << account.getHolderName()
              << "\nAccount type: " << account.getAccountType()
              << "\nBalance: $" << std::fixed << std::setprecision(2)
              << static_cast<double>(account.getBalanceCents()) / 100.0
              << "\n";
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
            Account* account = findByAccountNumber(enteredAccountNumber);
            if (account != nullptr) {
                std::cout<<"Account number "<<enteredAccountNumber<<" found.\n";
                return *account;
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
