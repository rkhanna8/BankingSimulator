//
// Created by Rohan Khanna on 5/1/26.
//

#include "Transaction.h"

double Transaction::getAmount() const {
    return trans_amount;
}
void Transaction::display() const {
    std::cout<<"Type: "<<type<<"\nAmount: "<<trans_amount<<"\nBalance after: "<<balanceAfter<<"\nNote: "<<note<<"\n";
}
double Transaction::getBalanceAfter() const {
    return balanceAfter;
}
std::string Transaction::getNote() const {
    return note;
}
std::string Transaction::getType() const {
    return type;
}
