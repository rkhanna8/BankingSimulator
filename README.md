# Banking Simulator

[![CI](https://github.com/rkhanna8/BankingSimulator/actions/workflows/ci.yml/badge.svg)](https://github.com/rkhanna8/BankingSimulator/actions/workflows/ci.yml)

Banking Simulator is a full-stack C++20 portfolio project with a responsive web
frontend, a JSON HTTP API, and Firebase Realtime Database persistence. It
demonstrates backend API design, secure credential handling, concurrency-safe
cloud writes, input validation, and automated testing.

## Features

- C++ HTTP server built with `cpp-httplib`
- HTML, CSS, and JavaScript frontend
- Firebase Realtime Database persistence over its REST API
- Monetary values stored as integer cents
- PINs hashed with Argon2id and unique random salts
- Random 12-digit account identifiers
- Conditional Firebase writes that prevent accidental account overwrites
- PIN-secured account access, deposits, withdrawals, history, PIN changes, and
  confirmed account deletion
- Privacy-limited exact holder-name search
- Environment-based Firebase configuration with credentials excluded from Git
- CTest security checks

## Technology

| Layer | Technology |
| --- | --- |
| Backend | C++20, cpp-httplib, nlohmann/json |
| Persistence | Firebase Realtime Database REST API, libcurl |
| Security | Argon2id, operating-system secure randomness |
| Frontend | HTML, CSS, JavaScript |
| Build and test | CMake, CTest, GitHub Actions |

## Architecture

```text
Browser (HTML/CSS/JavaScript)
              |
              | JSON over HTTP
              v
       C++ HTTP server
              |
              +-- AccountManager / Account
              |
              +-- FirebaseDatabase
                         |
                         v
              Firebase Realtime Database
```

The browser never talks directly to Firebase and never receives a PIN or PIN
hash. The C++ server validates requests and controls persistence.

## Requirements

- CMake 3.20 or newer
- A C++20 compiler
- libcurl development files
- Git, used by CMake to fetch pinned dependencies
- A Firebase project with Realtime Database enabled

CMake fetches `cpp-httplib`, `nlohmann/json`, and the reference Argon2 library.

## Firebase configuration

Follow [FIREBASE_SETUP.md](FIREBASE_SETUP.md). At minimum, the run environment
must contain:

```text
FIREBASE_DATABASE_URL=https://YOUR_DATABASE_NAME.firebaseio.com
```

Do not commit access tokens, ID tokens, database secrets, or service-account
private keys. The included `.env.example` contains names and placeholders only.

## Build and run locally

```bash
cmake -S . -B build
cmake --build build
./build/BankingSimulator
```

Open <http://127.0.0.1:8080> after the server starts.

The application deliberately binds to the local machine only. The C++ process
serves both the website and the API, so opening `web/index.html` directly will
not work.

## API

### Health check

```http
GET /api/health
```

### Create an account

```http
POST /api/accounts
Content-Type: application/json

{
  "holderName": "Portfolio User",
  "accountType": "checking",
  "pin": "0123"
}
```

### Access an account

```http
POST /api/accounts/{12-digit-account-number}/access
Content-Type: application/json

{
  "pin": "0123"
}
```

### Search by exact holder name

```http
POST /api/accounts/search
Content-Type: application/json

{
  "holderName": "Portfolio User"
}
```

Search results expose only holder name, account type, and account number. A PIN
is still required to access balances or history.

### Deposit or withdraw

```http
POST /api/accounts/{12-digit-account-number}/transactions
Content-Type: application/json

{
  "pin": "0123",
  "type": "deposit",
  "amountCents": 2500
}
```

Use `"withdrawal"` for a withdrawal. Insufficient funds are rejected.

### Change a PIN

```http
POST /api/accounts/{12-digit-account-number}/pin
Content-Type: application/json

{
  "pin": "0123",
  "newPin": "4567"
}
```

### Delete an account

```http
DELETE /api/accounts/{12-digit-account-number}
Content-Type: application/json

{
  "pin": "4567"
}
```

All account-detail responses intentionally omit `pinHash`. The frontend keeps
the entered PIN only in page memory and clears it when the user exits the
account, deletes it, or refreshes the page.

## Tests

```bash
ctest --test-dir build --output-on-failure
```

The security test verifies input validation, account-number shape, Argon2id
hash creation, correct-PIN verification, and wrong-PIN rejection.

The same build and test sequence runs automatically for pushes and pull
requests through GitHub Actions.

## Repository structure

```text
BankingSimulator/
├── main.cpp                  # HTTP routes and local server
├── FirebaseDatabase.*        # Firebase REST persistence
├── Account* / Transaction*   # Banking domain model
├── PinHasher.*               # Argon2id PIN hashing
├── SecureRandom.*            # Secure account and transaction identifiers
├── web/                      # Browser frontend
├── tests/                    # CTest security checks
└── FIREBASE_SETUP.md         # Firebase and CLion configuration
```

## Security scope and limitations

This project demonstrates secure development practices, but it is not a
production banking system:

- Firebase Authentication and user ownership rules are not implemented.
- Temporary public Firebase rules must only be used with fictional test data.
- The local HTTP server binds to loopback and does not provide TLS.
- The website is served by the C++ process and its features require that process
  to be running. It is not a standalone hosted frontend.
- Exact holder-name search intentionally reveals limited account-discovery data
  for this fictional portfolio demo; a production system should use authenticated
  ownership-based account discovery instead.
- Transfers are not part of the current simulator.
- Request throttling and automatic PIN lockouts are not implemented.
- Production auditing, compliance, rate limiting, key management, and disaster
  recovery are outside this project's scope.

The PIN is intentionally limited to four digits to match the simulator design.
Argon2id slows offline guessing, but a four-digit PIN still has a very small
search space and must not be treated like a strong password.
