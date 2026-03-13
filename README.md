<<<<<<< HEAD
\#  ABC Bank Management System




A fully functional, console-based Banking Management System built in C++ using core Object-Oriented Programming principles — no STL containers, no external libraries. All data structures, string operations, and file I/O are implemented from scratch.

---

## Table of Contents

- Overview
- Features
- OOP Architecture
- Class Hierarchy
- How It Works
- Input Validation and Error Handling
- File Structure
- Getting Started
- Usage Guide
- Security Features
- Full Demo Link

---

## Overview

ABC Bank Management System simulates real-world banking operations with three distinct user roles:

 User Client — Deposit, Withdraw, Transfer, View History

Company Client — All user operations + Loan Requests + Employee Payroll Transfers

Banking Employee (Admin) — Approve/Reject accounts, Freeze/Unfreeze, Issue Cards, Manage Loans

All data is persisted to flat .txt files between sessions — no database required.

---

## Features

### User Client Features

#### Account Registration
- Collects full name, address, phone number (10–15 digits), CNIC (exactly 13 digits), login ID, and password
- Validates each field before accepting — loops until valid input is provided
- Detects duplicate CNICs and duplicate login IDs across the entire system
- Assigns account type automatically based on daily withdrawal limit:
  - Bronze: up to 100,000 PKR/day
  - Gold: 100,001 to 500,000 PKR/day
- Account starts as unapproved — must be approved by a banking employee before any transactions

#### Login
- Requires correct user ID and password
- Maximum 3 login attempts before lockout
- Checks approval status and frozen status before granting access
- All login events are logged to system_log.txt

#### Deposit
- Accepts any amount from 1 to 10,000,000 PKR per transaction
- Validates that the account is approved and not frozen
- Guards against integer overflow on balance
- Records every deposit to transactions.txt with date, amount, and type
- Immediately updates users.txt after successful deposit

#### Withdrawal
- Requires a valid, non-fraudulent card
- Prompts for 4-digit PIN — 3 failed attempts marks the card as fraudulent and freezes the account
- Enforces minimum balance of 500 PKR after withdrawal
- Enforces daily withdrawal limit (resets automatically at midnight based on date comparison)
- Validates amount is within the single-transaction cap of 10,000,000 PKR
- Saves transaction to file and updates balance immediately

#### Fund Transfer
- Requires card selection and PIN verification (same fraud/freeze protections as withdrawal)
- Validates the recipient exists, is approved, and is not frozen
- Prevents self-transfers
- Deducts from sender and credits to recipient
- Records transfer in transactions.txt with both sender and recipient IDs

#### Transaction History
- Reads transactions.txt and filters by the logged-in account ID
- Displays date, amount, type, and recipient (if transfer) for every matching record
- Shows total transaction count

---

### Company Client Features

All user features plus:

#### Business Account Types
- Limit up to 100,000 PKR → Bronze
- Limit up to 500,000 PKR → Gold
- Limit up to 20,000,000 PKR → Business

#### Employee Management
- During registration, a company can associate existing user account IDs as employees
- Validates that each employee ID exists in the system before adding
- Employee associations are saved to companies_employees.txt
- Employees can log in through Company Client Login and access the company account menu

#### Employee Payroll Transfer
- Company can transfer funds directly to any registered, approved, non-frozen user account
- Validates recipient before processing
- Logs the transfer with both the company ID and recipient ID

#### Loan Requests
- Company submits a loan amount request up to 100,000,000 PKR
- Request is saved to loan_requests.txt with status "pending"
- A banking employee can later approve or reject it
- On approval, the loan amount is credited directly to the company balance and logged as a Loan-Approved transaction

---

### Banking Employee (Admin) Features

The admin panel is protected by credentials stored in admin.txt that have to be stored manually in format:
 adminusername,adminpassword
 Supports up to 3 login attempts.

#### View All Accounts
- Displays full details for every user and company account in the system

#### Approve / Reject User Accounts
- Lists all accounts pending approval
- On approval: generates a unique 16-digit card number, prompts for a 4-digit PIN, creates a card record, and saves to cards.txt
- On rejection: removes the account from memory and updates users.txt

#### Approve / Reject Company Accounts
- Same flow as user approval, without card issuance
- Updates companies.txt on decision

#### Loan Management
- Lists all pending loan requests from loan_requests.txt
- On approval: credits the amount to the company account, saves a transaction record, and updates the loan file status to "approved"
- On rejection: updates status to "rejected" using a temp file swap to safely rewrite the file

#### Freeze / Unfreeze Accounts
- Works on both user and company accounts
- Frozen accounts cannot deposit, withdraw, or transfer
- Change is immediately persisted to the relevant file

#### Close Account
- Warns if the account still has a balance above the minimum
- Removes account from memory array, shifts remaining elements, and rewrites the file
- Logs closure to system_log.txt

#### Issue New Card
- Admin can issue additional cards to any approved user
- Generates a new unique card number with no duplicates across all users
- Prompts for a 4-digit PIN, validates it, then saves to cards.txt

---

## OOP Architecture

### Inheritance

person
  └── user_client (also inherits from account)

account
  ├── user_client
  └── company_client

- person stores personal information: name, address, phone
- account stores financial data: balance, limits, status flags, and provides virtual deposit(), withdraw(), and pure virtual display_info()
- user_client inherits from both person and account (multiple inheritance)
- company_client inherits from account only

### Composition

user_client owns an array of card pointers. When the user object is destroyed, all its cards are freed with it. This is true ownership — the cards cannot outlive the user.

### Aggregation

company_client stores an array of employee IDs (integers) that reference existing user_client objects. The users exist independently of the company. Deleting the company does not delete the employees.

### Polymorphism

display_info() is a pure virtual function in the base account class, overridden in both user_client and company_client. The correct version is called at runtime depending on the object type.

deposit() and withdraw() are virtual — derived classes can override them if needed.

### Encapsulation

All class members are private or protected. No public fields exist anywhere. All data access goes through dedicated getters and setters.

---

## Class Hierarchy

banking_system
  - users[]         (array of user_client pointers)
  - companies[]     (array of company_client pointers)
  - register_user_client()
  - register_company_client()
  - banking_employee_menu()
  - user_client_menu()
  - company_client_menu()

user_client  (inherits person + account)
  - cards[]         (array of card pointers — composition)

company_client  (inherits account)
  - employee_ids[]  (array of integers — aggregation)

card
  - card_number
  - pin
  - fraud_flag
  - failed_attempts

transaction  (saved to file, not held in memory long-term)

---

## How It Works

### Data Persistence

All data is stored in plain-text CSV files:
all files are created at runtime except admin.txt which have to be created manually . 
users.txt                  — User account records
companies.txt              — Company account records
cards.txt                  — Card numbers and PINs per user
companies_employees.txt    — Employee–company associations
transactions.txt           — Full transaction log (append-only)
loan_requests.txt          — Loan requests with status
admin.txt                  — Admin credentials
system_log.txt             — Activity log with dates

### Daily Limit Reset

The system compares last_transaction_date with the current date every time a withdrawal or transfer is attempted. If the dates differ, today_withdrawn is reset to 0 automatically — no cron job or scheduler needed.

### Card Fraud Detection

After 3 wrong PIN attempts, the card's fraud_trans flag is set to true and the owning user account is frozen. The freeze is written to users.txt immediately.

---

## Input Validation and Error Handling

Every single user input is validated before being accepted. The system never crashes on bad input — it always prompts again.

Name / Company Name    — Cannot be empty or whitespace-only
Address                — Accepts any non-empty string
Phone                  — 10 to 15 characters, digits only
CNIC                   — Exactly 13 digits, no duplicates allowed
Tax Number             — Non-empty, no duplicates allowed
Login ID               — Non-empty, unique across all users and companies
Password               — 6 to 30 characters, not whitespace-only
PIN                    — Exactly 4 digits
Amount                 — Greater than 0, does not exceed 10,000,000 PKR
Daily Limit (user)     — 1 to 500,000 PKR
Daily Limit (company)  — 1 to 20,000,000 PKR
Menu Choice            — Integer in valid range, non-numeric input is cleared and re-prompted

All string operations use custom implementations with no reliance on the standard library string functions.

---

## File Structure

project/
  main.cpp                    — Entire source code
  admin.txt                   — Admin credentials (create manually before running)
  users.txt                   — Auto-generated on first user registration
  companies.txt               — Auto-generated on first company registration
  cards.txt                   — Auto-generated on first account approval
  companies_employees.txt     — Auto-generated on first company registration
  transactions.txt            — Auto-generated on first transaction
  loan_requests.txt           — Auto-generated on first loan request
  system_log.txt              — Auto-generated on first event

### Setting Up admin.txt

Create this file manually in the same folder as the executable before running the program. Format is username,password on each line:

admin,password123

Multiple admin accounts are supported — add one per line.

---

### First-Time Setup

1. Create admin.txt with your admin credentials
2. Run the program
3. Register a user from the main menu
4. Log in as admin to approve the user
5. Log back in as the user to start transacting

---

## Usage Guide

### Registering a User
Main Menu → 1 (Register New User Client)
Enter name, address, phone, CNIC, login ID, password, daily limit
System assigns account type and ID automatically
Account waits for admin approval before any transactions are possible

### Approving a User (Admin)
Main Menu → 5 (Banking Employee Login)
Enter admin credentials
Option 2 (Approve/Reject User Accounts)
Enter the account ID → Option 1 (Approve)
Set a 4-digit PIN → Card is generated and issued

### Making a Withdrawal
Main Menu → 3 (User Client Login)
Enter credentials
Option 3 (Withdraw Funds)
Select card → Enter PIN → Enter amount

### Transferring Funds
User Menu → 4 (Transfer Funds)
Select card → Enter PIN
Enter recipient account ID
Confirm recipient name → Enter amount

---

## Security Features

Login lockout — 3 failed attempts on any login portal locks access for that session

Card fraud detection — 3 wrong PINs flags the card and automatically freezes the account

Minimum balance enforcement — Cannot withdraw below 500 PKR

Daily limit tracking — Resets daily using date comparison, tracked per account

Overflow protection — Balance is checked against LLONG_MAX before every deposit

Freeze mechanism — Admin or automatic fraud detection can freeze any account instantly

Activity logging — Every significant event is written to system_log.txt with date and user ID

---
### Full Demo Link
🎬 [Watch the 3-minute demo](https://drive.google.com/file/d/1bZ5xB2SoHkUQNQcswmKsk3k4v3QgC2_3/view?usp=sharing)

---


Built as a demonstration of OOP principles in C++ — inheritance, polymorphism, encapsulation, composition, and aggregation — without relying on standard library containers or string utilities.