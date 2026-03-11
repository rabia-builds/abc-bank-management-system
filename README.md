\# 🏦 ABC Bank Management System



A fully functional, console-based \*\*Banking Management System\*\* built in \*\*C++\*\* using core Object-Oriented Programming principles — no STL containers, no external libraries. All data structures, string operations, and file I/O are implemented from scratch.



\---



\## 📋 Table of Contents



\- \[Overview](#overview)

\- \[Features](#features)

\- \[OOP Architecture](#oop-architecture)

\- \[Class Hierarchy](#class-hierarchy)

\- \[How It Works](#how-it-works)

\- \[Input Validation \& Error Handling](#input-validation--error-handling)

\- \[File Structure](#file-structure)

\- \[Getting Started](#getting-started)

\- \[Usage Guide](#usage-guide)

\- \[Security Features](#security-features)

\- \[Known Limitations](#known-limitations)

\- \[Future Improvements](#future-improvements)



\---



\## 📌 Overview



ABC Bank Management System simulates real-world banking operations with three distinct user roles:



| Role | Capabilities |

|---|---|

| \*\*User Client\*\* | Deposit, Withdraw, Transfer, View History |

| \*\*Company Client\*\* | All user operations + Loan Requests + Employee Payroll Transfers |

| \*\*Banking Employee (Admin)\*\* | Approve/Reject accounts, Freeze/Unfreeze, Issue Cards, Manage Loans |



All data is \*\*persisted to flat `.txt` files\*\* between sessions — no database required.



\---



\## ✨ Features



\### 👤 User Client Features



\#### Account Registration

\- Collects full name, address, phone number (10–15 digits), CNIC (exactly 13 digits), login ID, and password

\- Validates each field before accepting — loops until valid input is provided

\- Detects duplicate CNICs and duplicate login IDs across the entire system

\- Assigns account type automatically based on daily withdrawal limit:

&#x20; - \*\*Bronze\*\*: up to 100,000 PKR/day

&#x20; - \*\*Gold\*\*: 100,001 – 500,000 PKR/day

\- Account starts with `is\_approved = false` — must be approved by a banking employee before any transactions



\#### Login

\- Requires correct user ID and password

\- Maximum \*\*3 login attempts\*\* before lockout

\- Checks approval status and frozen status before granting access

\- All login events are logged to `system\_log.txt`



\#### Deposit

\- Accepts any amount from 1 to 10,000,000 PKR per transaction

\- Validates that the account is approved and not frozen

\- Guards against integer overflow on balance

\- Records every deposit to `transactions.txt` with date, amount, and type

\- Immediately updates `users.txt` after successful deposit



\#### Withdrawal

\- Requires a valid, non-fraudulent card

\- Prompts for \*\*4-digit PIN\*\* — 3 failed attempts marks the card as fraudulent and freezes the account

\- Enforces \*\*minimum balance\*\* of 500 PKR after withdrawal

\- Enforces \*\*daily withdrawal limit\*\* (resets automatically at midnight based on date comparison)

\- Validates amount is within the single-transaction cap (10,000,000 PKR)

\- Saves transaction to file and updates balance immediately



\#### Fund Transfer

\- Requires card selection and PIN verification (same fraud/freeze protections as withdrawal)

\- Validates the recipient exists, is approved, and is not frozen

\- Prevents self-transfers

\- Deducts from sender and credits to recipient atomically

\- Records transfer in `transactions.txt` with both sender and recipient IDs



\#### Transaction History

\- Reads `transactions.txt` and filters by the logged-in account ID

\- Displays date, amount, type, and recipient (if transfer) for every matching record

\- Shows total transaction count



\---



\### 🏢 Company Client Features



All user features plus:



\#### Business Account Types

\- Limit up to 100,000 → \*\*Bronze\*\*

\- Limit up to 500,000 → \*\*Gold\*\*

\- Limit up to 20,000,000 → \*\*Business\*\*



\#### Employee Management

\- During registration, a company can associate existing user account IDs as employees

\- Validates that each employee ID exists in the system before adding

\- Employee associations are saved to `companies\_employees.txt`

\- Employees can log in through "Company Client Login" and access the company account menu



\#### Employee Payroll Transfer

\- Company can transfer funds directly to any registered, approved, non-frozen user account

\- Validates recipient before processing

\- Logs the transfer with both the company ID and recipient ID



\#### Loan Requests

\- Company submits a loan amount request (up to 100,000,000 PKR)

\- Request is saved to `loan\_requests.txt` with status `pending`

\- A banking employee can later approve or reject it

\- On approval: loan amount is credited directly to the company's balance and logged as a `Loan-Approved` transaction



\---



\### 🔑 Banking Employee (Admin) Features



The admin panel is protected by credentials stored in `admin.txt`. Supports up to 3 login attempts.



\#### View All Accounts

\- Displays full details for every user and company account in the system



\#### Approve / Reject User Accounts

\- Lists all accounts with `is\_approved = false`

\- On approval: generates a unique 16-digit card number, prompts for a 4-digit PIN, creates a card record, and saves to `cards.txt`

\- On rejection: removes the account from memory and updates `users.txt`



\#### Approve / Reject Company Accounts

\- Same flow as user approval, without card issuance

\- Updates `companies.txt` on decision



\#### Loan Management

\- Lists all `pending` loan requests from `loan\_requests.txt`

\- On approval: credits the amount to the company account, saves a transaction record, and updates the loan file status to `approved`

\- On rejection: updates status to `rejected` — uses a temp file swap pattern to safely rewrite the file



\#### Freeze / Unfreeze Accounts

\- Works on both user and company accounts

\- Frozen accounts cannot deposit, withdraw, or transfer

\- Change is immediately persisted to the relevant file



\#### Close Account

\- Warns if the account still has a balance above the minimum

\- Removes account from memory array, shifts remaining elements, and rewrites the file

\- Logs closure to `system\_log.txt`



\#### Issue New Card

\- Admin can issue additional cards to any approved user

\- Generates a new unique card number (no duplicates across all users)

\- Prompts for a 4-digit PIN, validates it, then saves to `cards.txt`



\---



\## 🏗️ OOP Architecture



\### Inheritance



```

person

&#x20; └── user\_client (also inherits from account)



account

&#x20; ├── user\_client

&#x20; └── company\_client

```



\- `person` stores personal information (name, address, phone)

\- `account` stores financial data (balance, limits, status flags) and provides virtual `deposit()`, `withdraw()`, and pure virtual `display\_info()`

\- `user\_client` inherits from \*\*both\*\* `person` and `account` (multiple inheritance)

\- `company\_client` inherits from `account` only



\### Composition



\- `user\_client` \*\*owns\*\* an array of `card\*` pointers — when the user is destroyed, all cards are freed



\### Aggregation



\- `company\_client` stores an array of employee IDs (integers) that reference existing `user\_client` objects — the users exist independently



\### Polymorphism



\- `display\_info()` is a \*\*pure virtual\*\* function in `account`, overridden in both `user\_client` and `company\_client`

\- `deposit()` and `withdraw()` are `virtual` — can be overridden by derived classes

\- The system could hold `account\*` pointers and call the correct `display\_info()` at runtime



\### Encapsulation



\- All class members are `private` or `protected`

\- Access only through dedicated getters and setters

\- No friend classes or direct member access from outside



\---



\## 📊 Class Hierarchy



```

┌──────────────────────────────────────────────────────────┐

│                     banking\_system                        │

│  - users\[]        (user\_client\*\*)                        │

│  - companies\[]    (company\_client\*\*)                     │

│  + register\_user\_client()                                │

│  + register\_company\_client()                             │

│  + banking\_employee\_menu()                               │

│  + user\_client\_menu()                                    │

│  + company\_client\_menu()                                 │

└────────────────┬──────────────────┬──────────────────────┘

&#x20;                │                  │

&#x20;        ┌───────▼──────┐   ┌───────▼──────┐

&#x20;        │ user\_client  │   │company\_client│

&#x20;        │ (person +    │   │ (account)    │

&#x20;        │  account)    │   │              │

&#x20;        │ - cards\[]    │   │ - emp\_ids\[]  │

&#x20;        └──────┬───────┘   └──────────────┘

&#x20;               │

&#x20;        ┌──────▼───────┐

&#x20;        │    card      │

&#x20;        │ - card\_num   │

&#x20;        │ - pin        │

&#x20;        │ - fraud\_flag │

&#x20;        └──────────────┘



&#x20;        ┌──────────────┐

&#x20;        │ transaction  │  (saved to file, not held in memory)

&#x20;        └──────────────┘

```



\---



\## 🔄 How It Works



\### Data Persistence

All data is stored in plain-text CSV files:



| File | Contents |

|---|---|

| `users.txt` | User account records |

| `companies.txt` | Company account records |

| `cards.txt` | Card numbers and PINs per user |

| `companies\_employees.txt` | Employee–company associations |

| `transactions.txt` | Full transaction log (append-only) |

| `loan\_requests.txt` | Loan requests with status |

| `admin.txt` | Admin credentials |

| `system\_log.txt` | Activity log with dates |



\### Daily Limit Reset

The system compares `last\_transaction\_date` with the current date every time a withdrawal or transfer is attempted. If the dates differ, `today\_withdrawn` is reset to 0 automatically — no cron job or scheduler needed.



\### Card Fraud Detection

After \*\*3 wrong PIN attempts\*\*, the card's `fraud\_trans` flag is set to `true` and the owning user account is frozen. The freeze is written to `users.txt` immediately.



\---



\## ✅ Input Validation \& Error Handling



Every single user input is validated before being accepted. The system \*\*never crashes on bad input\*\* — it always prompts again.



| Input | Validation Rules |

|---|---|

| \*\*Name / Company Name\*\* | Cannot be empty or whitespace-only |

| \*\*Address\*\* | Accepts any non-empty string |

| \*\*Phone\*\* | 10–15 characters, digits only |

| \*\*CNIC\*\* | Exactly 13 digits, digits only, no duplicates |

| \*\*Tax Number\*\* | Non-empty, no duplicates |

| \*\*Login ID\*\* | Non-empty, unique across all users and companies |

| \*\*Password\*\* | 6–30 characters, not whitespace-only |

| \*\*PIN\*\* | Exactly 4 digits |

| \*\*Amount\*\* | Greater than 0, does not exceed 10,000,000 PKR |

| \*\*Daily Limit\*\* | 1 to 500,000 PKR (users), 1 to 20,000,000 PKR (companies) |

| \*\*Menu Choice\*\* | Integer in valid range — non-numeric input is cleared and re-prompted |

| \*\*Long Long Input\*\* | Overflow-safe parsing with `cin` failure detection |



All string operations use custom implementations: `str\_len()`, `str\_c()`, `str\_compare()`, `trim\_whitespace()`, `is\_only\_whitespace()` — \*\*no `<string>`, `strlen`, `strcpy`, or `strcmp`\*\*.



\---



\## 📁 File Structure



```

project/

├── main.cpp                  # Entire source code

├── admin.txt                 # Admin credentials (create manually)

├── users.txt                 # Auto-generated on first registration

├── companies.txt             # Auto-generated on first registration

├── cards.txt                 # Auto-generated on account approval

├── companies\_employees.txt   # Auto-generated on company registration

├── transactions.txt          # Auto-generated on first transaction

├── loan\_requests.txt         # Auto-generated on first loan request

└── system\_log.txt            # Auto-generated on first event

```



\### Setting Up `admin.txt`

Create this file manually before running:

```

admin,password123

```

Format: `username,password` — one admin per line (multiple admins supported).



\---



\## 🚀 Getting Started



\### Prerequisites

\- A C++11 (or later) compatible compiler: `g++`, `clang++`, or MSVC



\### Compile



```bash

\# Linux / macOS

g++ -std=c++11 -o bank main.cpp



\# Windows (MinGW)

g++ -std=c++11 -o bank.exe main.cpp

```



\### Run



```bash

\# Linux / macOS

./bank



\# Windows

bank.exe

```



\### First-Time Setup

1\. Create `admin.txt` with your admin credentials (see above)

2\. Run the program

3\. Register a user from the main menu

4\. Log in as admin to approve the user

5\. Log back in as the user to start transacting



\---



\## 📖 Usage Guide



\### Registering a User

```

Main Menu → 1 (Register New User Client)

→ Enter name, address, phone, CNIC, login ID, password, daily limit

→ System assigns account type and ID

→ Account awaits approval

```



\### Approving a User (Admin)

```

Main Menu → 5 (Banking Employee Login)

→ Enter admin credentials

→ 2 (Approve/Reject User Accounts)

→ Enter the account ID

→ 1 (Approve) → Set a 4-digit PIN → Card is issued

```



\### Making a Withdrawal

```

Main Menu → 3 (User Client Login)

→ Enter credentials

→ 3 (Withdraw Funds)

→ Select card → Enter PIN → Enter amount

```



\### Transferring Funds

```

User Menu → 4 (Transfer Funds)

→ Select card → Enter PIN

→ Enter recipient account ID

→ Confirm recipient name → Enter amount

```



\---



\## 🔐 Security Features



\- \*\*Login lockout\*\*: 3 failed attempts on any login portal locks access for that session

\- \*\*Card fraud detection\*\*: 3 wrong PINs → card flagged + account frozen automatically

\- \*\*Minimum balance enforcement\*\*: Cannot withdraw below 500 PKR

\- \*\*Daily limit tracking\*\*: Resets daily, tracked per account with date comparison

\- \*\*Overflow protection\*\*: Balance checked against `LLONG\_MAX` before deposit

\- \*\*Freeze mechanism\*\*: Admin or automatic fraud detection can freeze any account

\- \*\*Activity logging\*\*: Every significant event written to `system\_log.txt` with date and user ID



\---



\## ⚠️ Known Limitations



\- Passwords are stored in \*\*plain text\*\* — not suitable for production use

\- No concurrency support — single-user, single-process only

\- Fixed array capacities (100 users, 50 companies, 5 cards per user)

\- Card numbers are randomly generated — not Luhn-algorithm compliant

\- No password change functionality after registration

\- `update\_user\_files()` has a bug: writes `password` instead of `last\_transaction\_date` in field 12 — existing data may lose the last transaction date on next load



\---



\## 🔮 Future Improvements



\- \[ ] Hash passwords with bcrypt or SHA-256

\- \[ ] Replace flat files with SQLite for reliable persistence

\- \[ ] Add password change / account recovery flow

\- \[ ] Implement Luhn algorithm for card number generation

\- \[ ] Add interest calculation for savings accounts

\- \[ ] Build a simple GUI with Qt or a web front-end

\- \[ ] Add multi-threaded support with mutex locks

\- \[ ] Fix the `last\_transaction\_date` serialization bug in `update\_user\_files()`

\- \[ ] Support multiple admins with role-based permissions



\---



\## 🛠️ Tech Stack



| Component | Technology |

|---|---|

| Language | C++11 |

| Data Storage | Plain text CSV files |

| String Operations | Custom implementations (no STL) |

| Build | g++ / clang++ / MSVC |

| Platform | Cross-platform (Linux, macOS, Windows) |



\---



\## 📄 License



This project is for educational purposes. Feel free to use, modify, and distribute with attribution.



\---



\*Built as a demonstration of OOP principles in C++ — inheritance, polymorphism, encapsulation, composition, and aggregation — without relying on the C++ Standard Library containers.\*



