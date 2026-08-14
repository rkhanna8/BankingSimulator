#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Account.h"

namespace {
template<typename Function>
bool throwsInvalidArgument(Function function) {
    try {
        function();
        return false;
    } catch (const std::invalid_argument&) {
        return true;
    }
}
}

int main() {
    Account account("  Portfolio User  ", "checking", "0123");

    assert(account.getHolderName() == "Portfolio User");
    assert(account.getAccountNumber().length() == 12);
    assert(account.getAccountNumber().find_first_not_of("0123456789") ==
           std::string::npos);
    assert(account.getAccountNumber().front() != '0');

    assert(account.getPinHash() != "0123");
    assert(account.getPinHash().starts_with("$argon2id$"));
    assert(account.verifyPin("0123"));
    assert(!account.verifyPin("9999"));

    Account secondAccount("Second User", "savings", "0123");
    assert(secondAccount.getAccountNumber() != account.getAccountNumber());
    assert(secondAccount.getPinHash() != account.getPinHash());
    assert(secondAccount.verifyPin("0123"));

    assert(throwsInvalidArgument([] {
        Account invalid("", "checking", "1234");
    }));
    assert(throwsInvalidArgument([] {
        Account invalid("User", "investment", "1234");
    }));
    assert(throwsInvalidArgument([] {
        Account invalid("User", "checking", "12ab");
    }));
    assert(throwsInvalidArgument([] {
        Account invalid(std::string(101, 'A'), "checking", "1234");
    }));

    std::cout << "Security tests passed.\n";
    return 0;
}
