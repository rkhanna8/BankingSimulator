#ifndef BANKINGSIMULATOR_FIREBASEDATABASE_H
#define BANKINGSIMULATOR_FIREBASEDATABASE_H

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "Account.h"

class FirebaseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class AccountNotFoundError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class AuthenticationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class InsufficientFundsError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class AccountDataError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class TransactionConflictError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class FirebaseDatabase {
public:
    static FirebaseDatabase fromEnvironment();

    void createAccount(const Account& account) const;
    std::optional<nlohmann::json> getAccount(
        const std::string& accountNumber
    ) const;
    nlohmann::json accessAccount(
        const std::string& accountNumber,
        const std::string& pin
    ) const;
    std::vector<nlohmann::json> findAccountsByHolderName(
        const std::string& holderName
    ) const;
    nlohmann::json applyTransaction(
        const std::string& accountNumber,
        const std::string& pin,
        const std::string& type,
        long long amountCents
    ) const;
    nlohmann::json changePin(
        const std::string& accountNumber,
        const std::string& currentPin,
        const std::string& newPin
    ) const;
    void deleteAccount(
        const std::string& accountNumber,
        const std::string& pin
    ) const;

private:
    FirebaseDatabase(
        std::string databaseUrl,
        std::string accessToken,
        std::string idToken
    );

    struct Response {
        long status;
        std::string body;
        std::string etag;
    };

    struct VersionedAccount {
        nlohmann::json data;
        std::string etag;
    };

    VersionedAccount getVersionedAccount(
        const std::string& accountNumber
    ) const;
    static void verifyAccountPin(
        const nlohmann::json& account,
        const std::string& pin
    );

    Response request(
        const std::string& method,
        const std::string& path,
        const std::optional<nlohmann::json>& body = std::nullopt,
        const std::optional<std::string>& ifMatch = std::nullopt,
        bool requestEtag = false
    ) const;

    std::string databaseUrl;
    std::string accessToken;
    std::string idToken;
};

#endif //BANKINGSIMULATOR_FIREBASEDATABASE_H
