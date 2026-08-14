#include "FirebaseDatabase.h"

#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <limits>
#include <memory>
#include <mutex>
#include <sstream>
#include <utility>

#include <curl/curl.h>

#include "PinHasher.h"
#include "SecureRandom.h"

namespace {
std::string readEnvironmentVariable(const char* name) {
    const char* value = std::getenv(name);
    return value == nullptr ? "" : value;
}

std::string normalizeDatabaseUrl(std::string value) {
    while (!value.empty() &&
           std::isspace(static_cast<unsigned char>(value.front()))) {
        value.erase(value.begin());
    }
    while (!value.empty() &&
           std::isspace(static_cast<unsigned char>(value.back()))) {
        value.pop_back();
    }

    if (value.length() >= 2 &&
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.length() - 2);
    }

    while (!value.empty() && value.back() == '/') {
        value.pop_back();
    }

    if (value.ends_with(".json")) {
        value.erase(value.length() - 5);
    }

    return value;
}

bool isAllowedDatabaseUrl(const std::string& url) {
    const bool isFirebaseUrl =
        url.starts_with("https://") &&
        (url.find(".firebaseio.com") != std::string::npos ||
         url.find(".firebasedatabase.app") != std::string::npos);

    return isFirebaseUrl || url.starts_with("http://127.0.0.1:") ||
           url.starts_with("http://localhost:");
}

size_t appendResponseBody(
    char* contents,
    size_t size,
    size_t count,
    void* destination
) {
    const size_t byteCount = size * count;
    static_cast<std::string*>(destination)->append(contents, byteCount);
    return byteCount;
}

struct ResponseHeaders {
    std::string etag;
};

std::string trim(std::string value) {
    const auto isWhitespace = [](unsigned char character) {
        return std::isspace(character) != 0;
    };
    value.erase(
        value.begin(),
        std::find_if_not(value.begin(), value.end(), isWhitespace)
    );
    value.erase(
        std::find_if_not(value.rbegin(), value.rend(), isWhitespace).base(),
        value.end()
    );
    return value;
}

std::string lowerCase(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        }
    );
    return value;
}

bool isValidPin(const std::string& pin) {
    return pin.length() == 4 &&
        pin.find_first_not_of("0123456789") == std::string::npos;
}

size_t captureResponseHeader(
    char* contents,
    size_t size,
    size_t count,
    void* destination
) {
    const size_t byteCount = size * count;
    std::string line(contents, byteCount);
    const std::size_t separator = line.find(':');

    if (separator != std::string::npos &&
        lowerCase(line.substr(0, separator)) == "etag") {
        static_cast<ResponseHeaders*>(destination)->etag =
            trim(line.substr(separator + 1));
    }

    return byteCount;
}

void initializeCurl() {
    static std::once_flag initializationFlag;
    std::call_once(initializationFlag, [] {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
            throw FirebaseError("Could not initialize HTTPS support.");
        }
    });
}

std::string firebaseErrorMessage(long status, const std::string& responseBody) {
    std::ostringstream message;
    message << "Firebase request failed with HTTP " << status;

    if (!responseBody.empty()) {
        try {
            const nlohmann::json response = nlohmann::json::parse(responseBody);
            if (response.contains("error") && response["error"].is_string()) {
                message << ": " << response["error"].get<std::string>();
            }
        } catch (const nlohmann::json::exception&) {
            // The HTTP status is sufficient if Firebase did not return JSON.
        }
    }

    return message.str();
}

}

FirebaseDatabase FirebaseDatabase::fromEnvironment() {
    std::string databaseUrl =
        normalizeDatabaseUrl(readEnvironmentVariable("FIREBASE_DATABASE_URL"));

    if (databaseUrl.empty()) {
        throw FirebaseError(
            "FIREBASE_DATABASE_URL is not set. Add it to the CLion run "
            "configuration before starting the server."
        );
    }

    if (!isAllowedDatabaseUrl(databaseUrl)) {
        throw FirebaseError(
            "FIREBASE_DATABASE_URL must be the Realtime Database root URL "
            "ending in firebaseio.com or firebasedatabase.app. Plain HTTP is "
            "allowed only for a Firebase emulator on localhost."
        );
    }

    if (databaseUrl.find_first_of(" \t\r\n") != std::string::npos ||
        databaseUrl.find('?') != std::string::npos ||
        databaseUrl.find('#') != std::string::npos) {
        throw FirebaseError(
            "FIREBASE_DATABASE_URL contains whitespace, a query string, or a "
            "fragment. Paste only the Realtime Database root URL."
        );
    }

    std::string accessToken =
        readEnvironmentVariable("FIREBASE_ACCESS_TOKEN");
    std::string idToken = readEnvironmentVariable("FIREBASE_ID_TOKEN");

    if (!accessToken.empty() && !idToken.empty()) {
        throw FirebaseError(
            "Set either FIREBASE_ACCESS_TOKEN or FIREBASE_ID_TOKEN, not both."
        );
    }

    return FirebaseDatabase(
        std::move(databaseUrl),
        std::move(accessToken),
        std::move(idToken)
    );
}

FirebaseDatabase::FirebaseDatabase(
    std::string databaseUrl,
    std::string accessToken,
    std::string idToken
)
    : databaseUrl(std::move(databaseUrl)),
      accessToken(std::move(accessToken)),
      idToken(std::move(idToken)) {
    initializeCurl();
}

void FirebaseDatabase::createAccount(const Account& account) const {
    nlohmann::json transactions = nlohmann::json::object();
    for (std::size_t index = 0; index < account.transactions.size(); ++index) {
        const Transaction& transaction = account.transactions[index];
        transactions["transaction-" + std::to_string(index)] = {
            {"type", transaction.getType()},
            {"amountCents", transaction.getTransAmountCents()},
            {"balanceAfterCents", transaction.getBalanceAfterCents()}
        };
    }

    const nlohmann::json accountData = {
        {"accountNumber", account.getAccountNumber()},
        {"holderName", account.getHolderName()},
        {"accountType", account.getAccountType()},
        {"balanceCents", account.getBalanceCents()},
        {"pinHash", account.getPinHash()},
        {"createdAt", {{".sv", "timestamp"}}},
        {"updatedAt", {{".sv", "timestamp"}}},
        {"transactions", std::move(transactions)}
    };

    const Response response = request(
        "PUT",
        "/accounts/" + account.getAccountNumber(),
        accountData,
        "null_etag"
    );

    if (response.status == 412) {
        throw FirebaseError(
            "An account with the generated account number already exists."
        );
    }

    if (response.status != 200) {
        throw FirebaseError(firebaseErrorMessage(response.status, response.body));
    }
}

std::optional<nlohmann::json> FirebaseDatabase::getAccount(
    const std::string& accountNumber
) const {
    const Response response = request("GET", "/accounts/" + accountNumber);

    if (response.status != 200) {
        throw FirebaseError(firebaseErrorMessage(response.status, response.body));
    }

    try {
        nlohmann::json account = nlohmann::json::parse(response.body);
        if (account.is_null()) {
            return std::nullopt;
        }
        if (!account.is_object()) {
            throw FirebaseError("Firebase returned an invalid account record.");
        }
        return account;
    } catch (const nlohmann::json::exception&) {
        throw FirebaseError("Firebase returned malformed JSON.");
    }
}

nlohmann::json FirebaseDatabase::accessAccount(
    const std::string& accountNumber,
    const std::string& pin
) const {
    const std::optional<nlohmann::json> account = getAccount(accountNumber);
    if (!account.has_value()) {
        throw AccountNotFoundError("Account not found.");
    }

    verifyAccountPin(*account, pin);
    return *account;
}

std::vector<nlohmann::json> FirebaseDatabase::findAccountsByHolderName(
    const std::string& holderName
) const {
    const std::string normalizedName = lowerCase(trim(holderName));
    if (normalizedName.empty()) {
        throw std::invalid_argument("Holder name is required.");
    }
    if (normalizedName.length() > 100 ||
        std::any_of(
            normalizedName.begin(),
            normalizedName.end(),
            [](unsigned char character) {
                return std::iscntrl(character) != 0;
            }
        )) {
        throw std::invalid_argument(
            "Holder name must be 100 characters or fewer and cannot contain "
            "control characters."
        );
    }

    const Response response = request("GET", "/accounts");
    if (response.status != 200) {
        throw FirebaseError(firebaseErrorMessage(response.status, response.body));
    }

    try {
        const nlohmann::json accounts = nlohmann::json::parse(response.body);
        if (accounts.is_null()) {
            return {};
        }
        if (!accounts.is_object()) {
            throw FirebaseError("Firebase returned invalid account data.");
        }

        std::vector<nlohmann::json> matches;
        for (const auto& item : accounts.items()) {
            const nlohmann::json& account = item.value();
            if (!account.is_object() || !account.contains("holderName") ||
                !account["holderName"].is_string()) {
                continue;
            }

            if (lowerCase(trim(account["holderName"].get<std::string>())) ==
                normalizedName) {
                matches.push_back(account);
            }
        }
        return matches;
    } catch (const nlohmann::json::exception&) {
        throw FirebaseError("Firebase returned malformed JSON.");
    }
}

FirebaseDatabase::VersionedAccount FirebaseDatabase::getVersionedAccount(
    const std::string& accountNumber
) const {
    const Response response = request(
        "GET",
        "/accounts/" + accountNumber,
        std::nullopt,
        std::nullopt,
        true
    );

    if (response.status != 200) {
        throw FirebaseError(firebaseErrorMessage(response.status, response.body));
    }

    try {
        nlohmann::json account = nlohmann::json::parse(response.body);
        if (account.is_null()) {
            throw AccountNotFoundError("Account not found.");
        }
        if (!account.is_object() || response.etag.empty()) {
            throw AccountDataError("The stored account record is invalid.");
        }
        return {std::move(account), response.etag};
    } catch (const nlohmann::json::exception&) {
        throw AccountDataError("The stored account record is malformed.");
    }
}

void FirebaseDatabase::verifyAccountPin(
    const nlohmann::json& account,
    const std::string& pin
) {
    if (!account.contains("pinHash") || !account["pinHash"].is_string()) {
        throw AccountDataError(
            "This account predates PIN security. Create a new demo account."
        );
    }

    if (!isValidPin(pin)) {
        throw AuthenticationError("The account number or PIN is incorrect.");
    }

    try {
        if (!PinHasher::verify(
                account["pinHash"].get<std::string>(),
                pin
            )) {
            throw AuthenticationError(
                "The account number or PIN is incorrect."
            );
        }
    } catch (const AuthenticationError&) {
        throw;
    } catch (const std::exception&) {
        throw AccountDataError("The stored PIN record is invalid.");
    }
}

nlohmann::json FirebaseDatabase::applyTransaction(
    const std::string& accountNumber,
    const std::string& pin,
    const std::string& type,
    long long amountCents
) const {
    constexpr long long maximumTransactionCents = 100'000'000'000LL;
    constexpr int maximumAttempts = 3;

    if (type != "deposit" && type != "withdrawal") {
        throw std::invalid_argument(
            "Transaction type must be deposit or withdrawal."
        );
    }
    if (amountCents <= 0 || amountCents > maximumTransactionCents) {
        throw std::invalid_argument(
            "Amount must be between $0.01 and $1,000,000,000.00."
        );
    }

    for (int attempt = 0; attempt < maximumAttempts; ++attempt) {
        VersionedAccount account = getVersionedAccount(accountNumber);
        verifyAccountPin(account.data, pin);

        if (!account.data.contains("balanceCents") ||
            !account.data["balanceCents"].is_number_integer()) {
            throw AccountDataError("The stored balance is invalid.");
        }

        const long long oldBalance =
            account.data["balanceCents"].get<long long>();
        if (oldBalance < 0) {
            throw AccountDataError("The stored balance is invalid.");
        }
        if (type == "withdrawal" && amountCents > oldBalance) {
            throw InsufficientFundsError(
                "This account does not have enough funds for that withdrawal."
            );
        }
        if (type == "deposit" &&
            amountCents > std::numeric_limits<long long>::max() - oldBalance) {
            throw std::invalid_argument("The resulting balance is too large.");
        }

        const long long newBalance = type == "deposit"
            ? oldBalance + amountCents
            : oldBalance - amountCents;
        account.data["balanceCents"] = newBalance;

        if (!account.data.contains("transactions") ||
            account.data["transactions"].is_null()) {
            account.data["transactions"] = nlohmann::json::object();
        }
        if (!account.data["transactions"].is_object()) {
            throw AccountDataError("The stored transaction history is invalid.");
        }

        std::string transactionId;
        do {
            transactionId = "transaction-" + SecureRandom::numericString(18);
        } while (account.data["transactions"].contains(transactionId));

        account.data["transactions"][transactionId] = {
            {"type", type},
            {"amountCents", amountCents},
            {"balanceAfterCents", newBalance},
            {"createdAt", {{".sv", "timestamp"}}}
        };
        account.data["updatedAt"] = {{".sv", "timestamp"}};

        const Response response = request(
            "PUT",
            "/accounts/" + accountNumber,
            account.data,
            account.etag
        );
        if (response.status == 412) {
            continue;
        }
        if (response.status != 200) {
            throw FirebaseError(
                firebaseErrorMessage(response.status, response.body)
            );
        }

        try {
            return nlohmann::json::parse(response.body);
        } catch (const nlohmann::json::exception&) {
            throw FirebaseError("Firebase returned malformed JSON.");
        }
    }

    throw TransactionConflictError(
        "The account changed at the same time. Please try again."
    );
}

nlohmann::json FirebaseDatabase::changePin(
    const std::string& accountNumber,
    const std::string& currentPin,
    const std::string& newPin
) const {
    constexpr int maximumAttempts = 3;
    if (!isValidPin(newPin)) {
        throw std::invalid_argument("The new PIN must contain exactly 4 digits.");
    }

    const std::string newPinHash = PinHasher::hash(newPin);
    for (int attempt = 0; attempt < maximumAttempts; ++attempt) {
        VersionedAccount account = getVersionedAccount(accountNumber);
        verifyAccountPin(account.data, currentPin);
        account.data["pinHash"] = newPinHash;
        account.data["updatedAt"] = {{".sv", "timestamp"}};

        const Response response = request(
            "PUT",
            "/accounts/" + accountNumber,
            account.data,
            account.etag
        );
        if (response.status == 412) {
            continue;
        }
        if (response.status != 200) {
            throw FirebaseError(
                firebaseErrorMessage(response.status, response.body)
            );
        }

        try {
            return nlohmann::json::parse(response.body);
        } catch (const nlohmann::json::exception&) {
            throw FirebaseError("Firebase returned malformed JSON.");
        }
    }

    throw TransactionConflictError(
        "The account changed at the same time. Please try again."
    );
}

void FirebaseDatabase::deleteAccount(
    const std::string& accountNumber,
    const std::string& pin
) const {
    constexpr int maximumAttempts = 3;
    for (int attempt = 0; attempt < maximumAttempts; ++attempt) {
        const VersionedAccount account = getVersionedAccount(accountNumber);
        verifyAccountPin(account.data, pin);

        const Response response = request(
            "DELETE",
            "/accounts/" + accountNumber,
            std::nullopt,
            account.etag
        );
        if (response.status == 412) {
            continue;
        }
        if (response.status != 200) {
            throw FirebaseError(
                firebaseErrorMessage(response.status, response.body)
            );
        }
        return;
    }

    throw TransactionConflictError(
        "The account changed at the same time. Please try again."
    );
}

FirebaseDatabase::Response FirebaseDatabase::request(
    const std::string& method,
    const std::string& path,
    const std::optional<nlohmann::json>& body,
    const std::optional<std::string>& ifMatch,
    bool requestEtag
) const {
    using CurlHandle = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
    using HeaderList = std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;

    CurlHandle curl(curl_easy_init(), &curl_easy_cleanup);
    if (!curl) {
        throw FirebaseError("Could not create an HTTPS request.");
    }

    std::string url = databaseUrl + path + ".json";
    if (!idToken.empty()) {
        char* encodedToken = curl_easy_escape(
            curl.get(),
            idToken.c_str(),
            static_cast<int>(idToken.length())
        );
        if (encodedToken == nullptr) {
            throw FirebaseError("Could not encode the Firebase ID token.");
        }
        url += "?auth=" + std::string(encodedToken);
        curl_free(encodedToken);
    }

    curl_slist* rawHeaders = nullptr;
    rawHeaders = curl_slist_append(rawHeaders, "Content-Type: application/json");

    const std::string authorizationHeader =
        "Authorization: Bearer " + accessToken;
    if (!accessToken.empty()) {
        rawHeaders = curl_slist_append(
            rawHeaders,
            authorizationHeader.c_str()
        );
    }

    const std::string ifMatchHeader =
        "if-match: " + ifMatch.value_or("");
    if (ifMatch.has_value()) {
        rawHeaders = curl_slist_append(rawHeaders, ifMatchHeader.c_str());
    }
    if (requestEtag) {
        rawHeaders = curl_slist_append(rawHeaders, "X-Firebase-ETag: true");
    }

    HeaderList headers(rawHeaders, &curl_slist_free_all);
    std::string responseBody;
    ResponseHeaders responseHeaders;
    const std::string requestBody = body.has_value() ? body->dump() : "";

    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get());
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, appendResponseBody);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, captureResponseHeader);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &responseHeaders);
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, "BankingSimulator/1.0");

    if (body.has_value()) {
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(
            curl.get(),
            CURLOPT_POSTFIELDSIZE,
            static_cast<long>(requestBody.size())
        );
    }

    const CURLcode result = curl_easy_perform(curl.get());
    if (result != CURLE_OK) {
        throw FirebaseError(
            std::string("Could not reach Firebase: ") + curl_easy_strerror(result)
        );
    }

    long status = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status);
    return {status, std::move(responseBody), std::move(responseHeaders.etag)};
}
