#include <iostream>
#include <vector>

#include "Account.h"

int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.


    std::cout << "Hello and welcome to the banking simulator.\n";

    int menuPress = 0;
    std::cout << "This is the main menu. Here you can choose to create an account, "
            "view account details, or initiate a transaction: \n";
    std::cout<<"Create an Account (1)\n";
    std::cout<<"View Account Details (3)\n";
    std::cout<<"New transaction (2)\n";

    while(true) {
        std::cout<<"Enter your menu choice: ";
        std::cin>>menuPress;
        if (menuPress == 1) {

            break;
        }
        else if (menuPress == 2) {

        switch (menuPress) {
            case 1:
                std::cout<<"Create an account: ";
            case 2:
                std::cout<<"View account details: ";
            case 3:
                std::cout<<"New transaction: ";
            default:
                std::cout<<"Invalid input. Enter an integer 1-3. Try again.\n";
        }
    }
    std::cin>>menuPress;



    return 0;
}

