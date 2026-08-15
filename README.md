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

## Engineering highlights

- Account and transaction amounts use integer cents rather than floating-point
  values.
- PIN hashes and salts never leave the C++/Firebase boundary in API responses.
- Firebase ETags protect balance-changing operations from conflicting writes.
- The frontend keeps an entered PIN only in page memory and clears it when the
  user exits an account, deletes it, or refreshes the page.
- GitHub Actions builds the project and runs its CTest security checks.

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
└── CMakeLists.txt            # Build configuration
```
