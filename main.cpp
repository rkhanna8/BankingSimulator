#include <iostream>
#include <vector>

#include "Account.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
using namespace std;


int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.


    cout << "Hello and welcome to the banking simulator." << endl;

    int menuPress = 0;
    cout << "This is the main menu. Here you can choose to add an account (enter 1), "
            "view current account information (enter 2), or initiate a transaction (enter 3): " <<endl;

    while(true) {
        cout<<"Enter your menu choice: ";
        cin>>menuPress;
        if (menuPress == 1) {
            double initialBalance = 0;
            cout<<"Specify the initial deposit being made ($500 minimum) to start the account: "<<endl;
            while (initialBalance<500){
                cin>>initialBalance;
                if (initialBalance < 500) {
                    cout<<"Invalid amount. Try again: "<<endl;
                }
            }
            new Account(initialBalance);
            break;
        }
        else if (menuPress == 2) {

            break;
        }
        else if (menuPress == 3) {

            break;
        }
        else {
            cout<<"Invalid input. Enter an integer 1-3. Try again"<<endl;
        }
    }
    cin>>menuPress;



    return 0;
}

