# Firebase setup for BankingSimulator

The server reads and writes accounts through the Firebase Realtime Database
REST API. Configuration is read from environment variables so credentials are
not stored in this repository.

## 1. Copy the database URL

In the Firebase Console, open **Build > Realtime Database**. Copy the URL shown
at the top of the Data page. It usually resembles one of these:

```text
https://PROJECT_ID-default-rtdb.firebaseio.com
https://PROJECT_ID-default-rtdb.REGION.firebasedatabase.app
```

Use the exact URL displayed by Firebase.

## 2. Add the URL to the CLion run configuration

1. Open **Run > Edit Configurations**.
2. Select the **BankingSimulator** application.
3. Find **Environment variables**.
4. Add `FIREBASE_DATABASE_URL` with the copied URL as its value.
5. Apply the changes.

The program does not automatically load `.env` files. The `.env.example` file
only documents the available settings.

## 3. Configure authentication

For a protected database, add exactly one of these environment variables:

```text
FIREBASE_ACCESS_TOKEN=SHORT_LIVED_GOOGLE_OAUTH2_ACCESS_TOKEN
FIREBASE_ID_TOKEN=SHORT_LIVED_FIREBASE_USER_ID_TOKEN
```

`FIREBASE_ACCESS_TOKEN` is sent in an `Authorization: Bearer` header.
`FIREBASE_ID_TOKEN` is sent using Firebase's `auth` query parameter and is
evaluated by Realtime Database Security Rules.

Both token types expire and must be refreshed. Do not place a service-account
private key in this repository and do not paste one into either variable.

An authentication token may be omitted only when using the local Firebase
Emulator or temporary public test rules. Public rules are unsafe for deployed
or sensitive data.

## 4. Run and verify

Start `BankingSimulator` in CLion and open:

```text
http://127.0.0.1:8080
```

Create an account through the form, then open the Realtime Database Data page.
The new record should appear under:

```text
accounts/{accountNumber}
```

Plaintext PINs are never sent to Firebase. New accounts store an Argon2id hash
with a unique random salt in the private `pinHash` field. API responses always
remove that field.

The checked-in `firebase.rules.json` is a secure default-deny example for the
GitHub project. Temporary public test rules make all permitted fields—including
PIN hashes—readable to anyone who knows the database URL. Use fictional data
only, and restore default-deny rules when testing is complete.
