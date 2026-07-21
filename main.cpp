#include <iostream>
#include <vector>

#include "Account.h"
#include "AccountManager.h"
#include "Utils.h"

int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.


    std::cout << "Hello and welcome to the banking simulator.\n";

    int menuPress = 0;



    while(true) {
        std::cout << "This is the main menu. Here you can choose to create an account, "
            "view account details, or initiate a transaction: \n";
        std::cout<<"Create an Account (1)\n";
        std::cout<<"View Account Details (2)\n";
        std::cout<<"New transaction (3)\n";
        std::cout<<"Delete an Account (4)\n";
        std::cout<<"Exit program (5)\n";

        menuPress = Utils::getIntInput("Enter your menu choice: ");

        switch (menuPress) {
            case 1: {
                std::cout<<"Create an account: \n";
                AccountManager::createAccount();
                break;
            }
            case 2: {
                std::cout<<"View account details: \n";
                Account& account = AccountManager::searchAccounts();
                account.displayDetails();
                break;
            }
            case 3: {
                std::cout<<"New transaction: \n";
                new Transaction(); //Fix this to be same logic as creating an account
                break;
            }
            case 4: {
                std::cout<<"Delete an account: \n";
                AccountManager::deleteAccount();
                break;
            }
            case 5:
                return 0;
            default:
                std::cout<<"Invalid input. Enter an integer 1-3. Try again.\n";
        }
    }
}
