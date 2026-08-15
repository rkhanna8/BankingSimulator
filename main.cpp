#include <iostream>
#include <string>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "Account.h"
#include "AccountManager.h"
#include "FirebaseDatabase.h"

using json = nlohmann::json;

namespace {
constexpr std::size_t maximumRequestBodyBytes = 4096;

void setJson(httplib::Response& response, int status, const json& body) {
    response.status = status;
    response.set_content(body.dump(), "application/json");
}

void setJsonError(
    httplib::Response& response,
    int status,
    const std::string& message
) {
    setJson(response, status, {{"error", message}});
}

json parseRequestBody(const httplib::Request& request) {
    if (request.body.empty()) {
        throw std::invalid_argument("Request body is required.");
    }
    if (request.body.size() > maximumRequestBodyBytes) {
        throw std::length_error("Request body is too large.");
    }
    return json::parse(request.body);
}

std::string requiredString(const json& input, const std::string& field) {
    if (!input.contains(field) || !input[field].is_string()) {
        throw std::invalid_argument(field + " must be a string.");
    }
    return input[field].get<std::string>();
}

long long requiredInteger(const json& input, const std::string& field) {
    if (!input.contains(field) || !input[field].is_number_integer()) {
        throw std::invalid_argument(field + " must be a whole number.");
    }
    return input[field].get<long long>();
}

json accountToJson(const Account& account) {
    return {
        {"accountNumber", account.getAccountNumber()},
        {"holderName", account.getHolderName()},
        {"accountType", account.getAccountType()},
        {"balanceCents", account.getBalanceCents()},
        {"transactions", json::object()}
    };
}

json firebaseAccountToJson(const json& account) {
    if (!account.is_object() ||
        !account.contains("accountNumber") ||
        !account.contains("holderName") ||
        !account.contains("accountType") ||
        !account.contains("balanceCents")) {
        throw AccountDataError("The stored account record is incomplete.");
    }

    json publicAccount = {
        {"accountNumber", account.at("accountNumber")},
        {"holderName", account.at("holderName")},
        {"accountType", account.at("accountType")},
        {"balanceCents", account.at("balanceCents")},
        {"transactions", json::object()}
    };

    if (account.contains("transactions") &&
        account["transactions"].is_object()) {
        publicAccount["transactions"] = account["transactions"];
    }
    if (account.contains("createdAt")) {
        publicAccount["createdAt"] = account["createdAt"];
    }
    if (account.contains("updatedAt")) {
        publicAccount["updatedAt"] = account["updatedAt"];
    }

    return publicAccount;
}

json searchResultToJson(const json& account) {
    if (!account.is_object() ||
        !account.contains("accountNumber") ||
        !account.contains("holderName") ||
        !account.contains("accountType")) {
        throw AccountDataError("A stored account record is incomplete.");
    }

    return {
        {"accountNumber", account.at("accountNumber")},
        {"holderName", account.at("holderName")},
        {"accountType", account.at("accountType")}
    };
}

template<typename Action>
void runApiAction(
    httplib::Response& response,
    const std::string& logContext,
    Action action
) {
    try {
        action();
    } catch (const nlohmann::json::parse_error&) {
        setJsonError(response, 400, "Request body must be valid JSON.");
    } catch (const nlohmann::json::exception&) {
        setJsonError(response, 400, "Request data has an invalid format.");
    } catch (const std::length_error& error) {
        setJsonError(response, 413, error.what());
    } catch (const std::invalid_argument& error) {
        setJsonError(response, 400, error.what());
    } catch (const AuthenticationError& error) {
        setJsonError(response, 401, error.what());
    } catch (const AccountNotFoundError& error) {
        setJsonError(response, 404, error.what());
    } catch (const InsufficientFundsError& error) {
        setJsonError(response, 409, error.what());
    } catch (const AccountDataError& error) {
        setJsonError(response, 409, error.what());
    } catch (const TransactionConflictError& error) {
        setJsonError(response, 409, error.what());
    } catch (const FirebaseError& error) {
        setJsonError(response, 502, error.what());
    } catch (const std::exception& error) {
        std::cerr << logContext << ": " << error.what() << "\n";
        setJsonError(response, 500, "The request could not be completed.");
    }
}
}

int runServer(FirebaseDatabase& firebase) {
    httplib::Server server;
    server.set_socket_options([](socket_t socket) {
        httplib::set_socket_opt(socket, SOL_SOCKET, SO_REUSEADDR, 1);
#ifdef SO_REUSEPORT
        httplib::set_socket_opt(socket, SOL_SOCKET, SO_REUSEPORT, 0);
#endif
    });

    server.set_default_headers({
        {"Cache-Control", "no-store, max-age=0"},
        {"Pragma", "no-cache"}
    });

    server.new_task_queue = [] {
        return new httplib::ThreadPool(1);
    };

    if (!server.set_mount_point("/", "./web")) {
        std::cerr
            << "Could not find the web directory.\n"
            << "Check the program's working directory.\n";
        return 1;
    }

    server.Get(
        "/api/health",
        [](const httplib::Request&, httplib::Response& response) {
            setJson(
                response,
                200,
                {{"status", "ok"}, {"message", "Connected to C++ server."}}
            );
        }
    );

    server.Post(
        "/api/accounts",
        [&firebase](const httplib::Request& request,
                    httplib::Response& response) {
            std::string createdAccountNumber;
            runApiAction(response, "Account creation failed", [&] {
                const json input = parseRequestBody(request);
                const std::string holderName =
                    requiredString(input, "holderName");
                const std::string accountType =
                    requiredString(input, "accountType");
                const std::string pin = requiredString(input, "pin");

                const Account& account = AccountManager::createAccount(
                    holderName,
                    accountType,
                    pin
                );
                createdAccountNumber = account.getAccountNumber();

                try {
                    firebase.createAccount(account);
                } catch (...) {
                    AccountManager::removeByAccountNumber(createdAccountNumber);
                    throw;
                }

                setJson(response, 201, accountToJson(account));
            });
        }
    );

    server.Post(
        "/api/accounts/search",
        [&firebase](const httplib::Request& request,
                    httplib::Response& response) {
            runApiAction(response, "Holder-name search failed", [&] {
                const json input = parseRequestBody(request);
                const std::string holderName =
                    requiredString(input, "holderName");
                const std::vector<json> accounts =
                    firebase.findAccountsByHolderName(holderName);

                json results = json::array();
                for (const json& account : accounts) {
                    results.push_back(searchResultToJson(account));
                }
                setJson(response, 200, {{"accounts", std::move(results)}});
            });
        }
    );

    server.Post(
        R"(/api/accounts/([0-9]{12})/access)",
        [&firebase](const httplib::Request& request,
                    httplib::Response& response) {
            runApiAction(response, "Account access failed", [&] {
                const json input = parseRequestBody(request);
                const std::string pin = requiredString(input, "pin");
                const std::string accountNumber = request.matches[1].str();
                const json account =
                    firebase.accessAccount(accountNumber, pin);
                setJson(response, 200, firebaseAccountToJson(account));
            });
        }
    );

    server.Post(
        R"(/api/accounts/([0-9]{12})/transactions)",
        [&firebase](const httplib::Request& request,
                    httplib::Response& response) {
            runApiAction(response, "Transaction failed", [&] {
                const json input = parseRequestBody(request);
                const std::string pin = requiredString(input, "pin");
                const std::string type = requiredString(input, "type");
                const long long amountCents =
                    requiredInteger(input, "amountCents");
                const std::string accountNumber = request.matches[1].str();

                const json account = firebase.applyTransaction(
                    accountNumber,
                    pin,
                    type,
                    amountCents
                );
                setJson(response, 200, firebaseAccountToJson(account));
            });
        }
    );

    server.Post(
        R"(/api/accounts/([0-9]{12})/pin)",
        [&firebase](const httplib::Request& request,
                    httplib::Response& response) {
            runApiAction(response, "PIN change failed", [&] {
                const json input = parseRequestBody(request);
                const std::string pin = requiredString(input, "pin");
                const std::string newPin = requiredString(input, "newPin");
                const std::string accountNumber = request.matches[1].str();

                const json account = firebase.changePin(
                    accountNumber,
                    pin,
                    newPin
                );
                setJson(response, 200, firebaseAccountToJson(account));
            });
        }
    );

    server.Delete(
        R"(/api/accounts/([0-9]{12}))",
        [&firebase](const httplib::Request& request,
                    httplib::Response& response) {
            runApiAction(response, "Account deletion failed", [&] {
                const json input = parseRequestBody(request);
                const std::string pin = requiredString(input, "pin");
                const std::string accountNumber = request.matches[1].str();

                firebase.deleteAccount(accountNumber, pin);
                setJson(response, 200, {{"message", "Account deleted."}});
            });
        }
    );

    std::cout << "Banking server started.\n";
    std::cout << "Open http://127.0.0.1:8080\n";

    const bool started = server.listen("127.0.0.1", 8080);
    if (!started) {
        std::cerr << "The server could not start.\n";
        return 1;
    }
    return 0;
}

int main() {
    try {
        FirebaseDatabase firebase = FirebaseDatabase::fromEnvironment();
        std::cout << "Firebase persistence enabled.\n";
        return runServer(firebase);
    } catch (const FirebaseError& error) {
        std::cerr << "Firebase configuration error: " << error.what() << "\n";
        return 1;
    }
}
