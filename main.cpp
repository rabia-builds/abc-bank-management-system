#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <climits>

using namespace std;

//forward declarations
class transaction;
class card;
class account;
class user_client;
class company_client;
class banking_employee;

// COLOR FUNCTIONS

// Color codes
const char* COLOR_RESET = "\033[0m";
const char* COLOR_RED = "\033[31m";
const char* COLOR_GREEN = "\033[32m";
const char* COLOR_YELLOW = "\033[33m";
const char* COLOR_BLUE = "\033[34m";
const char* COLOR_MAGENTA = "\033[35m";
const char* COLOR_CYAN = "\033[36m";
const char* COLOR_WHITE = "\033[37m";
const char* COLOR_BOLD = "\033[1m";

// CONSTANTS
const long long MAX_SINGLE_TRANSACTION = 10000000;  
const long long MIN_BALANCE = 500;  
const int MAX_LOGIN_ATTEMPTS = 3;

// UTILITY FUNCTIONS

// Function to calculate length of a string & returns: number of characters in the string
int str_len(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Function to copy one string to another
void str_c(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Function to compare two strings & returns: true if strings are equal, false otherwise
bool str_compare(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return str1[i] == str2[i];
}

// Function to convert string to integer & handles negative numbers as well
int string_to_int(const char* str) {
    int result = 0;
    int i = 0;
    bool negative = false;

    if (str[0] == '-') {
        negative = true;
        i = 1;
    }

    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
        i++;
    }
    if (negative) {
        return -result;
    }
    else {
        return result;
    }
}

// Function to convert integer to string
void int_to_string(int num, char* str) {
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    int i = 0;
    bool negative = false;
    if (num < 0) {
        negative = true;
        num = -num;
    }
    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num = num / 10;
    }
    if (negative) {
        str[i++] = '-';
    }
    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

// Function to convert string to long long (for large numbers like balance)
long long string_to_long_long(const char* str) {
    long long result = 0;
    int i = 0;
    bool negative = false;
    if (str[0] == '-') {
        negative = true;
        i = 1;
    }

    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
        i++;
    }
    if (negative) {
        return -result;
    }
    else {
        return result;
    }
}

// Function to convert long long to string
void long_long_to_string(long long num, char* str) {
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    int i = 0;
    bool negative = false;

    if (num < 0) {
        negative = true;
        num = -num;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (negative) {
        str[i++] = '-';
    }
    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

// Function to get current date in dd/mm/yyyy format
void get_current_date(char* date) {
    time_t now = time(0);
    tm ltm;

    // Platform specific time conversion
#ifdef _WIN32
    localtime_s(&ltm, &now);
#else
    tm* temp = localtime(&now);
    ltm = *temp;
#endif

    char day[10], month[10], year[10];
    int_to_string(ltm.tm_mday, day);
    int_to_string(1 + ltm.tm_mon, month);
    int_to_string(1900 + ltm.tm_year, year);

    // Concatenate day/month/year
    int idx = 0;
    for (int i = 0; day[i] != '\0'; i++) {
        date[idx++] = day[i];
    }
    date[idx++] = '/';
    for (int i = 0; month[i] != '\0'; i++) {
        date[idx++] = month[i];
    }
    date[idx++] = '/';
    for (int i = 0; year[i] != '\0'; i++) {
        date[idx++] = year[i];
    }
    date[idx] = '\0';
}

// Function to check if string contains only whitespace
bool is_only_whitespace(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            return false;
        }
    }
    return true;
}

// Function to trim whitespace from string
void trim_whitespace(char* str) {
    // Trim leading whitespace
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r') {
        start++;
    }

    // Shift string left
    int i = 0;
    while (str[start] != '\0') {
        str[i++] = str[start++];
    }
    str[i] = '\0';

    // Trim trailing whitespace
    i--;
    while (i >= 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r')) {
        str[i--] = '\0';
    }
}

// Function to safely get integer input with validation
bool get_int_input(int& value, int min_val = INT_MIN, int max_val = INT_MAX) {
    while (!(cin >> value)) {
        cout << COLOR_RED << "    ----> Invalid input! Please enter a number." << COLOR_RESET << endl;
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "    Enter choice: ";
    }
    cin.ignore(10000, '\n');

    if (value < min_val || value > max_val) {
        cout << COLOR_RED << "    ----> Number out of range! (" << min_val << " to " << max_val << ")" << COLOR_RESET << endl;
        return false;
    }
    return true;
}

// Function to safely get long long input with validation
bool get_long_long_input(long long& value, long long min_val = LLONG_MIN, long long max_val = LLONG_MAX) {
    while (!(cin >> value)) {
        cout << COLOR_RED << "    ----> Invalid input! Please enter a number." << COLOR_RESET << endl;
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "    Enter amount: ";
    }
    cin.ignore(10000, '\n');

    if (value < min_val || value > max_val) {
        cout << COLOR_RED << "    ----> Amount out of range!" << COLOR_RESET << endl;
        return false;
    }
    return true;
}

// Function to log system activities
void log_activity(const char* activity, int user_id = -1) {
    ofstream log_file("system_log.txt", ios::app);
    if (log_file.is_open()) {
        char date[20];
        get_current_date(date);
        log_file << date << " - ";
        if (user_id != -1) {
            log_file << "User " << user_id << ": ";
        }
        log_file << activity << endl;
        log_file.close();
    }
}

// VALIDATION FUNCTIONS

// Function to validate CNIC (must be exactly 13 digits)
bool validate_cnic(const char* cnic) {
    int len = str_len(cnic);
    if (len != 13) {
        cout << COLOR_RED << "    ----> CNIC must be exactly 13 digits" << endl << COLOR_RESET;
        return false;
    }
    // Check all characters are digits
    for (int i = 0; i < len; i++) {
        if (cnic[i] < '0' || cnic[i] > '9') {
            cout << COLOR_RED << "    ----> CNIC must contain only digits!" << endl << COLOR_RESET;
            return false;
        }
    }
    cout << COLOR_GREEN << "    ----> CNIC validation successful!" << endl << COLOR_RESET;
    return true;
}

// Function to validate phone number (10-15 characters, digits only)
bool validate_phone(const char* phone) {
    int len = str_len(phone);
    if (len < 10 || len > 15) {
        cout << COLOR_RED << "    ----> Phone number must be 10-15 digits" << endl << COLOR_RESET;
        return false;
    }

    // Check all characters are digits
    for (int i = 0; i < len; i++) {
        if (phone[i] < '0' || phone[i] > '9') {
            cout << COLOR_RED << "    ----> Phone number must contain only digits!" << endl << COLOR_RESET;
            return false;
        }
    }

    cout << COLOR_GREEN << "    ----> Phone validation successful" << endl << COLOR_RESET;
    return true;
}

// Password validation 
bool validate_password(const char* password) {
    int len = str_len(password);
    if (password == NULL || is_only_whitespace(password)) {
        cout << COLOR_RED << "    ----> Password cannot be empty" << endl << COLOR_RESET;
        return false;
    }
    else if (len < 6) {
        cout << COLOR_RED << "    ----> Password must be at least 6 characters" << endl << COLOR_RESET;
        return false;
    }
    else if (len > 30) {
        cout << COLOR_RED << "    ----> Password too long (max 30 characters)" << endl << COLOR_RESET;
        return false;
    }

    cout << COLOR_GREEN << "    ----> Password validation successful" << endl << COLOR_RESET;
    return true;
}

// Function to validate PIN (must be exactly 4 digits)
bool validate_pin(const char* pin) {
    int len = str_len(pin);
    if (len != 4) {
        cout << COLOR_RED << "    ----> PIN must be exactly 4 digits!" << endl << COLOR_RESET;
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (pin[i] < '0' || pin[i] > '9') {
            cout << COLOR_RED << "    ----> PIN must contain only digits!" << endl << COLOR_RESET;
            return false;
        }
    }

    return true;
}

// Function to validate amount (positive and within limits)
bool validate_amount(long long amount, long long max_amount = MAX_SINGLE_TRANSACTION) {
    if (amount <= 0) {
        cout << COLOR_RED << "    ----> Amount must be greater than zero!" << COLOR_RESET << endl;
        return false;
    }

    if (amount > max_amount) {
        cout << COLOR_RED << "    ----> Amount exceeds maximum transaction limit ("
            << max_amount << " PKR)!" << COLOR_RESET << endl;
        return false;
    }

    return true;
}

// BASE CLASSES
// 
// Person class - Base class for storing personal information
// Used by: user_client
class person {
protected:
    char name[100];      // person's full name
    char address[200];   // Person's address
    char phone[20];      // Person's phone number

public:
    person() {
        name[0] = '\0';
        address[0] = '\0';
        phone[0] = '\0';
    }
    // Setter functions
    void set_name(const char* n) { str_c(name, n); }
    void set_address(const char* a) { str_c(address, a); }
    void set_phone(const char* p) { str_c(phone, p); }

    // Getter functions
    const char* get_name() const { return name; }
    const char* get_address() const { return address; }
    const char* get_phone() const { return phone; }
    virtual ~person() {};
};

// Account class - Base class for all types of bank accounts
// Used by: user_client, company_client

class account {
protected:
    int account_id;                      // Unique account ID
    char user_id[50];                    // Login username
    char password[50];                   // Login password
    long long balance;                   // Account balance
    long long daily_withdrawal_limit;    // Maximum withdrawal per day
    long long today_withdrawn;           // Amount withdrawn today
    char account_type[20];               // Bronze/Gold/Business
    bool is_approved;                    // Account approval status
    bool is_frozen;                      // Account frozen status
    char last_transaction_date[20];     // Date of last transaction

public:
    account() {
        account_id = 0;
        user_id[0] = '\0';
        password[0] = '\0';
        balance = 0;
        daily_withdrawal_limit = 0;
        today_withdrawn = 0;
        account_type[0] = '\0';
        is_approved = false;
        is_frozen = false;
        last_transaction_date[0] = '\0';
    }

    // Setter functions
    void set_account_id(int id) { account_id = id; }
    void set_user_id(const char* uid) { str_c(user_id, uid); }
    void set_password(const char* pwd) { str_c(password, pwd); }
    void set_balance(long long b) { balance = b; }
    void set_daily_limit(long long limit) { daily_withdrawal_limit = limit; }
    void set_account_type(const char* type) { str_c(account_type, type); }
    void set_approved(bool status) { is_approved = status; }
    void set_frozen(bool status) { is_frozen = status; }
    void set_last_transaction_date(const char* date) { str_c(last_transaction_date, date); }
    void set_today_withdrawn(long long amount) { today_withdrawn = amount; }

    // Getter functions
    int get_account_id() const { return account_id; }
    const char* get_user_id() const { return user_id; }
    const char* get_password() const { return password; }
    long long get_balance() const { return balance; }
    long long get_daily_limit() const { return daily_withdrawal_limit; }
    const char* get_account_type() const { return account_type; }
    bool get_approved() const { return is_approved; }
    bool get_frozen() const { return is_frozen; }
    long long get_today_withdrawn() const { return today_withdrawn; }

    //function to check if day has changed so reset daily withdrawal
    void check_and_reset_daily_limit() {
        char current_date[20];
        get_current_date(current_date);

        //if date is different, reset withdrawal amount
        if (!str_compare(last_transaction_date, current_date)) {
            today_withdrawn = 0;
            str_c(last_transaction_date, current_date);
        }
    }

    //function to deposit money & returns: true if successful, false otherwise
    virtual bool deposit(long long amount) {
        // Validate amount
        if (!validate_amount(amount)) {
            return false;
        }

        if (is_frozen) {
            cout << COLOR_RED << "    ----> Deposit failed! Account is frozen!" << COLOR_RESET << endl;
            return false;
        }

        // Check for overflow
        if (balance > LLONG_MAX - amount) {
            cout << COLOR_RED << "    ----> Deposit failed! Would cause balance overflow!" << COLOR_RESET << endl;
            return false;
        }

        balance = balance + amount;
        check_and_reset_daily_limit();
        cout << COLOR_GREEN << "    ----> Deposit successful! " << COLOR_RESET;
        cout << "Amount: " << COLOR_CYAN << amount << " PKR" << COLOR_RESET << endl;
        return true;
    }

    //  function to withdraw money & checks: frozen status, balance, daily limit
    virtual bool withdraw(long long amount) {
        check_and_reset_daily_limit();

        // Validate amount
        if (!validate_amount(amount)) {
            return false;
        }

        if (is_frozen) {
            cout << COLOR_RED << "    ----> Withdrawal failed! Account is frozen!" << COLOR_RESET << endl;
            return false;
        }

        // Check minimum balance
        if (balance - amount < MIN_BALANCE) {
            cout << COLOR_RED << "    ----> Withdrawal failed! Minimum balance of "
                << MIN_BALANCE << " PKR required!" << COLOR_RESET << endl;
            cout << "    ----> Current balance: " << COLOR_YELLOW << balance << " PKR" << COLOR_RESET << endl;
            cout << "    ----> After withdrawal: " << COLOR_YELLOW << (balance - amount) << " PKR" << COLOR_RESET << endl;
            return false;
        }

        if (amount > balance) {
            cout << COLOR_RED << "    ----> Withdrawal failed! Insufficient balance!" << COLOR_RESET << endl;
            return false;
        }

        if (today_withdrawn + amount > daily_withdrawal_limit) {
            cout << COLOR_RED << "    ----> Withdrawal failed! Daily limit exceeded!" << COLOR_RESET << endl;
            cout << "    ----> Today withdrawn: " << COLOR_YELLOW << today_withdrawn << " PKR" << COLOR_RESET << endl;
            cout << "    ----> Daily limit: " << COLOR_YELLOW << daily_withdrawal_limit << " PKR" << COLOR_RESET << endl;
            cout << "    ----> Remaining: " << COLOR_YELLOW << (daily_withdrawal_limit - today_withdrawn) << " PKR" << COLOR_RESET << endl;
            return false;
        }

        balance = balance - amount;
        today_withdrawn = today_withdrawn + amount;
        cout << COLOR_GREEN << "    ----> Withdrawal successful " << COLOR_RESET;
        cout << "Amount: " << COLOR_CYAN << amount << " PKR" << COLOR_RESET << endl;
        return true;
    }

    // Pure virtual function
    virtual void display_info() const = 0;
    virtual ~account() {}
};


// Transaction class - Records all transactions

class transaction {
private:
    char date[20];           // Transaction date
    int client_id;           // Client who made transaction
    long long amount;        // Transaction amount
    char type[20];           // Deposit/Withdrawal/Transfer/Loan
    int recipient_id;        // For transfers, -1 if not applicable

public:
    transaction() {
        date[0] = '\0';
        client_id = 0;
        amount = 0;
        type[0] = '\0';
        recipient_id = -1;
    }

    //parameterized constructor
    transaction(const char* d, int cid, long long amt, const char* t, int rid = -1) {
        str_c(date, d);
        client_id = cid;
        amount = amt;
        str_c(type, t);
        recipient_id = rid;
    }

    // Function to save transaction to file
    bool save_to_file() {
        ofstream file("transactions.txt", ios::app);
        if (file.is_open()) {
            file << date << "," << client_id << "," << amount << "," << type;
            if (recipient_id != -1) {
                file << "," << recipient_id;
            }
            file << endl;
            file.close();
            return true;
        }
        cout << COLOR_RED << "    ----> Error: Could not save transaction!" << COLOR_RESET << endl;
        return false;
    }

    //function to display transaction details
    void display() const {
        cout << COLOR_BLUE << "Date: " << date << " | Client ID: " << client_id
            << " | Amount: " << amount << " PKR | Type: " << type;
        if (recipient_id != -1) {
            cout << " | Recipient ID: " << recipient_id;
        }
        cout << COLOR_RESET << endl;
    }
};

// Card class - ATM/Debit card for users

class card {
private:
    int client_id;              // Owner of the card
    char card_number[17];       // 16-digit card number
    char pin[5];                // 4-digit PIN
    bool fraud_trans;         // Fraud detection flag
    int failed_attempts;        // Number of wrong PIN attempts

public:
    card() {
        client_id = 0;
        card_number[0] = '\0';
        pin[0] = '\0';
        fraud_trans = false;
        failed_attempts = 0;
    }

    // Parameterized constructor
    card(int cid, const char* cn, const char* p) {
        client_id = cid;
        str_c(card_number, cn);
        str_c(pin, p);
        fraud_trans = false;
        failed_attempts = 0;
    }

    // Getter functions
    int get_client_id() const { return client_id; }
    const char* get_card_number() const { return card_number; }
    bool get_fraud_trans() const { return fraud_trans; }
    int get_failed_attempts() const { return failed_attempts; }

    // Function to verify PIN & returns: true if PIN is correct
    // After 3 failed attempts, marks card as fraudulent
    bool verify_pin(const char* input_pin) {
        if (str_compare(pin, input_pin)) {
            failed_attempts = 0;
            return true;
        }
        failed_attempts++;
        cout << COLOR_YELLOW << "    ----> Incorrect PIN! Attempts remaining: "
            << (3 - failed_attempts) << COLOR_RESET << endl;

        if (failed_attempts >= 3) {
            fraud_trans = true;
            cout << COLOR_CYAN << " *** SECURITY ALERT *** " << endl << COLOR_RESET;
            cout << COLOR_RED << " Card marked as fraudulent due to multiple failed attempts!" << endl << COLOR_RESET;
            log_activity("Card marked as fraudulent due to failed PIN attempts", client_id);
        }
        return false;
    }

    // Reset failed attempts
    void reset_failed_attempts() {
        failed_attempts = 0;
    }

    // Function to save card to file
    bool save_to_file() {
        ofstream file("cards.txt", ios::app);
        if (file.is_open()) {
            file << client_id << "," << card_number << "," << pin << endl;
            file.close();
            return true;
        }
        cout << COLOR_RED << "    ----> Error: Could not save card!" << COLOR_RESET << endl;
        return false;
    }

    //function to display card information
    void display() const {
        cout << COLOR_BLUE << "Card Number: " << card_number << " | Client ID: "
            << client_id << COLOR_RESET << endl;
    }
};

// Forward declaration for banking_system
class banking_system;

// User Client class - Individual customer account
// Inheritance: Inherits from both person and account
// Composition: Contains multiple card objects
class user_client : public person, public account {
private:
    char cnic[20];           // national ID card number
    card** cards;            // Array of pointers to cards
    int card_count;          // Number of cards issued
    int card_capacity;       // Maximum cards allowed

public:
    user_client() : person(), account() {
        cnic[0] = '\0';
        card_count = 0;
        card_capacity = 5;
        cards = new card * [card_capacity];
        for (int i = 0; i < card_capacity; i++) {
            cards[i] = nullptr;
        }
    }

    // Setter and getter for CNIC
    void set_cnic(const char* c) { str_c(cnic, c); }
    const char* get_cnic() const { return cnic; }

    // Function to add a new card
    bool add_card(card* c) {
        if (card_count < card_capacity) {
            cards[card_count++] = c;
            return true;
        }
        cout << COLOR_RED << "    ----> Maximum card limit reached!" << COLOR_RESET << endl;
        return false;
    }

    int get_card_count() const { return card_count; }

    // Get a card by desired index
    card* get_card(int index) {
        if (index >= 0 && index < card_count) {
            return cards[index];
        }
        return nullptr;
    }

    // Override display_info to show user details
    void display_info() const override {
        cout << "========================================" << endl;
        cout << COLOR_YELLOW << "     USER CLIENT INFORMATION            " << endl << COLOR_RESET;
        cout << "========================================" << endl;
        cout << COLOR_BLUE << "Account ID      : " << account_id << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Name            : " << name << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Address         : " << address << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Phone           : " << phone << endl << COLOR_RESET;
        cout << COLOR_BLUE << "CNIC            : " << cnic << endl << COLOR_RESET;
        cout << COLOR_BLUE << "User ID         : " << user_id << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Balance         : " << balance << " PKR" << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Account Type    : " << account_type << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Daily Limit     : " << daily_withdrawal_limit << " PKR" << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Today Withdrawn : " << today_withdrawn << " PKR" << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Status          : " << COLOR_RESET;
        if (is_approved == true) {
            cout << COLOR_GREEN << "Approved" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_YELLOW << "Pending Approval" << COLOR_RESET << endl;
        }
        cout << COLOR_BLUE << "Frozen          : " << COLOR_RESET;
        if (is_frozen == true) {
            cout << COLOR_RED << "Yes" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_GREEN << "No" << COLOR_RESET << endl;
        }
        cout << COLOR_BLUE << "Number of Cards : " << card_count << endl << COLOR_RESET;
        cout << "========================================" << endl;
    }

    // Function to save user data to file
    bool save_to_file() {
        ofstream file("users.txt", ios::app);
        if (file.is_open()) {
            string approvedFlag = is_approved ? "1" : "0";
            string frozenFlag = is_frozen ? "1" : "0";

            file << account_id << "," << name << "," << address << "," << cnic << ","
                << user_id << "," << password << "," << balance << "," << account_type << ","
                << daily_withdrawal_limit << "," << approvedFlag << "," << frozenFlag << ","
                << today_withdrawn << "," << last_transaction_date << endl;
            file.close();
            return true;
        }
        cout << COLOR_RED << "    ----> Error: Could not save user data!" << COLOR_RESET << endl;
        return false;
    }

    //function to transfer money to another user
    bool transfer(int recipient_id, long long amount, const char* card_pin) {
        if (withdraw(amount)) {
            char date[20];
            get_current_date(date);
            transaction t(date, account_id, amount, "Transfer", recipient_id);
            t.save_to_file();
            log_activity("Transfer completed", account_id);
            return true;
        }
        return false;
    }

    // Destructor
    ~user_client() {
        for (int i = 0; i < card_count; i++) {
            delete cards[i];
        }
        delete[] cards;
    }
};

// Company Client class - Business account
// Inheritance: Inherits from account
// Aggregation: Associated with multiple employees (user accounts)
class company_client : public account {
private:
    char company_name[100];        // Company's registered name
    char company_address[200];     // Company's address
    char tax_number[50];           // Company's tax ID
    int* employee_ids;             // Array of employee account IDs
    int employee_count;            // Number of employees
    int employee_capacity;         // Maximum employees

public:
    company_client() : account() {
        company_name[0] = '\0';
        company_address[0] = '\0';
        tax_number[0] = '\0';
        employee_count = 0;
        employee_capacity = 50;
        employee_ids = new int[employee_capacity];
    }

    // Setter functions
    void set_company_name(const char* name) { str_c(company_name, name); }
    void set_company_address(const char* addr) { str_c(company_address, addr); }
    void set_tax_number(const char* tax) { str_c(tax_number, tax); }

    // Getter functions
    const char* get_company_name() const { return company_name; }
    const char* get_company_address() const { return company_address; }
    const char* get_tax_number() const { return tax_number; }
    int get_employee_count() const { return employee_count; }

    // Function to add employee to company
    bool add_employee(int emp_id) {
        if (employee_count < employee_capacity) {
            employee_ids[employee_count++] = emp_id;
            return true;
        }
        cout << COLOR_RED << "    ----> Maximum employee limit reached!" << COLOR_RESET << endl;
        return false;
    }

    // Function to check if a user is employee
    bool is_employee(int user_id) const {
        for (int i = 0; i < employee_count; i++) {
            if (employee_ids[i] == user_id) {
                return true;
            }
        }
        return false;
    }

    // Override display_info for company details
    void display_info() const override {
        cout << "========================================" << endl;
        cout << COLOR_YELLOW << "     COMPANY CLIENT INFORMATION         " << endl << COLOR_RESET;
        cout << "========================================" << endl;

        cout << COLOR_BLUE << "Company ID        : " << account_id << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Company Name      : " << company_name << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Address           : " << company_address << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Tax Number        : " << tax_number << endl << COLOR_RESET;
        cout << COLOR_BLUE << "User ID           : " << user_id << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Balance           : " << balance << " PKR" << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Account Type      : " << account_type << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Daily Limit       : " << daily_withdrawal_limit << " PKR" << endl << COLOR_RESET;
        cout << COLOR_BLUE << "Today Withdrawn   : " << today_withdrawn << " PKR" << endl << COLOR_RESET;

        cout << COLOR_BLUE << "Status            : " << COLOR_RESET;
        if (is_approved == true) {
            cout << COLOR_GREEN << "Approved" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_YELLOW << "Pending Approval" << COLOR_RESET << endl;
        }

        cout << COLOR_BLUE << "Frozen            : " << COLOR_RESET;
        if (is_frozen == true) {
            cout << COLOR_RED << "Yes" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_GREEN << "No" << COLOR_RESET << endl;
        }
        cout << COLOR_BLUE << "Number of Employees: " << employee_count << endl << COLOR_RESET;
        cout << "========================================" << endl;
    }


    // Function to save company data to file
    bool save_to_file() {
        ofstream file("companies.txt", ios::app);
        if (file.is_open()) {
            string approvedFlag = is_approved ? "1" : "0";
            string frozenFlag = is_frozen ? "1" : "0";

            file << account_id << "," << company_name << "," << company_address << ","
                << tax_number << "," << user_id << "," << password << "," << balance << ","
                << account_type << "," << daily_withdrawal_limit << "," << approvedFlag << ","
                << frozenFlag << "," << today_withdrawn << "," << last_transaction_date << endl;
            file.close();
            return true;
        }
        cout << COLOR_RED << "    ----> Error: Could not save company data!" << COLOR_RESET << endl;
        return false;
    }


    // Function to save employee associations
    bool save_employees_to_file() {
        ofstream file("companies_employees.txt", ios::app);
        if (file.is_open()) {
            for (int i = 0; i < employee_count; i++) {
                file << employee_ids[i] << "," << account_id << endl;
            }
            file.close();
            return true;
        }
        cout << COLOR_RED << "    ----> Error: Could not save employee data!" << COLOR_RESET << endl;
        return false;
    }

    // Destructor
    ~company_client() {
        delete[] employee_ids;
    }
};

// Banking System class - Main system controller
// Manages all users, companies, and operations
class banking_system {
private:
    user_client** users;           // Dynamic array of user
    int user_count;                // Current number of users
    int user_capacity;             // Maximum users

    company_client** companies;    // Dynamic array of company
    int company_count;             // Current number of companies
    int company_capacity;          // Maximum companies

    int next_user_id;              // Next available user ID
    int next_company_id;           // Next available company ID

public:
    banking_system() {
        user_capacity = 100;
        users = new user_client * [user_capacity];
        user_count = 0;

        company_capacity = 50;
        companies = new company_client * [company_capacity];
        company_count = 0;

        next_user_id = 1;
        next_company_id = 1;

        load_data();
    }

    // Destructor
    ~banking_system() {
        for (int i = 0; i < user_count; i++) {
            delete users[i];
        }
        delete[] users;

        for (int i = 0; i < company_count; i++) {
            delete companies[i];
        }
        delete[] companies;
    }

    // Function to check if CNIC already exists
    bool cnic_exists(const char* cnic) {
        for (int i = 0; i < user_count; i++) {
            if (str_compare(users[i]->get_cnic(), cnic)) {
                return true;
            }
        }
        return false;
    }

    // Function to check if tax number already exists
    bool tax_number_exists(const char* tax_num) {
        for (int i = 0; i < company_count; i++) {
            if (str_compare(companies[i]->get_tax_number(), tax_num)) {
                return true;
            }
        }
        return false;
    }

    // Function to check if card number already exists
    bool card_number_exists(const char* card_num) {
        for (int i = 0; i < user_count; i++) {
            for (int j = 0; j < users[i]->get_card_count(); j++) {
                if (str_compare(users[i]->get_card(j)->get_card_number(), card_num)) {
                    return true;
                }
            }
        }
        return false;
    }

    // Function to check if user ID already exists
    bool user_id_exists(const char* uid) {
        for (int i = 0; i < user_count; i++) {
            if (str_compare(users[i]->get_user_id(), uid)) {
                return true;
            }
        }
        for (int i = 0; i < company_count; i++) {
            if (str_compare(companies[i]->get_user_id(), uid)) {
                return true;
            }
        }
        return false;
    }

    // Function to generate unique card number
    void generate_unique_card_number(char* card_num) {
        do {
            for (int i = 0; i < 16; i++) {
                card_num[i] = '0' + (rand() % 10);
            }
            card_num[16] = '\0';
        } while (card_number_exists(card_num));
    }

    // Function to load all data from files 
    void load_data() {
        cout << COLOR_CYAN << "LOADING ALL DATA..." << COLOR_RESET << endl;

        //loading users from users.txt
        ifstream user_file("users.txt");
        if (user_file.is_open()) {
            char line[600];
            while (user_file.getline(line, 600)) {
                if (str_len(line) == 0) {
                    continue;
                }
                user_client* user = new user_client();
                int field = 0;
                int start = 0;

                // Parse CSV line
                for (int i = 0; i <= str_len(line); i++) {
                    if (line[i] == ',' || line[i] == '\0') {
                        char temp[250];
                        int idx = 0;
                        for (int j = start; j < i; j++) {
                            temp[idx++] = line[j];
                        }
                        temp[idx] = '\0';

                        // Assign to appropriate field
                        switch (field) {
                        case 0: user->set_account_id(string_to_int(temp)); break;
                        case 1: user->set_name(temp); break;
                        case 2: user->set_address(temp); break;
                        case 3: user->set_cnic(temp); break;
                        case 4: user->set_user_id(temp); break;
                        case 5: user->set_password(temp); break;
                        case 6: user->set_balance(string_to_long_long(temp)); break;
                        case 7: user->set_account_type(temp); break;
                        case 8: user->set_daily_limit(string_to_long_long(temp)); break;
                        case 9: user->set_approved(string_to_int(temp) == 1); break;
                        case 10: user->set_frozen(string_to_int(temp) == 1); break;
                        case 11: user->set_today_withdrawn(string_to_long_long(temp)); break;
                        case 12: user->set_last_transaction_date(temp); break;
                        }

                        field++;
                        start = i + 1;
                    }
                }

                if (user_count < user_capacity) {
                    users[user_count++] = user;
                    if (user->get_account_id() >= next_user_id) {
                        next_user_id = user->get_account_id() + 1;
                    }
                }
                else {
                    delete user;
                }
            }
            user_file.close();
            cout << COLOR_GREEN << "--> Data from users.txt loaded (" << user_count << " users)" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_YELLOW << "--> users.txt not found, starting fresh" << COLOR_RESET << endl;
        }

        // Load cards from cards.txt
        ifstream card_file("cards.txt");
        if (card_file.is_open()) {
            char line[100];
            while (card_file.getline(line, 100)) {
                if (str_len(line) == 0) continue;

                int client_id = 0;
                char card_num[17], pin[5];
                int field = 0;
                int start = 0;

                for (int i = 0; i <= str_len(line); i++) {
                    if (line[i] == ',' || line[i] == '\0') {
                        char temp[50];
                        int idx = 0;
                        for (int j = start; j < i; j++) {
                            temp[idx++] = line[j];
                        }
                        temp[idx] = '\0';

                        if (field == 0) client_id = string_to_int(temp);
                        else if (field == 1) str_c(card_num, temp);
                        else if (field == 2) str_c(pin, temp);

                        field++;
                        start = i + 1;
                    }
                }

                // Find user and add card
                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == client_id) {
                        card* c = new card(client_id, card_num, pin);
                        users[i]->add_card(c);
                        break;
                    }
                }
            }
            card_file.close();
            cout << COLOR_GREEN << "--> Data from cards.txt loaded" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_YELLOW << "--> cards.txt not found, starting fresh" << COLOR_RESET << endl;
        }

        // Load companies from companies.txt
        ifstream comp_file("companies.txt");
        if (comp_file.is_open()) {
            char line[600];
            while (comp_file.getline(line, 600)) {
                if (str_len(line) == 0) {
                    continue;
                }

                company_client* company = new company_client();
                int field = 0;
                int start = 0;
                for (int i = 0; i <= str_len(line); i++) {
                    if (line[i] == ',' || line[i] == '\0') {
                        char temp[250];
                        int idx = 0;
                        for (int j = start; j < i; j++) {
                            temp[idx++] = line[j];
                        }
                        temp[idx] = '\0';

                        switch (field) {
                        case 0: company->set_account_id(string_to_int(temp)); break;
                        case 1: company->set_company_name(temp); break;
                        case 2: company->set_company_address(temp); break;
                        case 3: company->set_tax_number(temp); break;
                        case 4: company->set_user_id(temp); break;
                        case 5: company->set_password(temp); break;
                        case 6: company->set_balance(string_to_long_long(temp)); break;
                        case 7: company->set_account_type(temp); break;
                        case 8: company->set_daily_limit(string_to_long_long(temp)); break;
                        case 9: company->set_approved(string_to_int(temp) == 1); break;
                        case 10: company->set_frozen(string_to_int(temp) == 1); break;
                        case 11: company->set_today_withdrawn(string_to_long_long(temp)); break;
                        case 12: company->set_last_transaction_date(temp); break;
                        }

                        field++;
                        start = i + 1;
                    }
                }

                if (company_count < company_capacity) {
                    companies[company_count++] = company;
                    if (company->get_account_id() >= next_company_id) {
                        next_company_id = company->get_account_id() + 1;
                    }
                }
                else {
                    delete company;
                }
            }
            comp_file.close();
            cout << COLOR_GREEN << "--> Data from companies.txt loaded (" << company_count << " companies)" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_YELLOW << "--> companies.txt not found, starting fresh" << COLOR_RESET << endl;
        }

        //load company employees from companies_employees.txt
        ifstream emp_file("companies_employees.txt");
        if (emp_file.is_open()) {
            char line[100];
            while (emp_file.getline(line, 100)) {
                if (str_len(line) == 0) continue;

                int emp_id = 0, comp_id = 0;
                int field = 0;
                int start = 0;

                for (int i = 0; i <= str_len(line); i++) {
                    if (line[i] == ',' || line[i] == '\0') {
                        char temp[50];
                        int idx = 0;
                        for (int j = start; j < i; j++) {
                            temp[idx++] = line[j];
                        }
                        temp[idx] = '\0';

                        if (field == 0) emp_id = string_to_int(temp);
                        else if (field == 1) comp_id = string_to_int(temp);

                        field++;
                        start = i + 1;
                    }
                }
                // Find company and add employee
                for (int i = 0; i < company_count; i++) {
                    if (companies[i]->get_account_id() == comp_id) {
                        companies[i]->add_employee(emp_id);
                        break;
                    }
                }
            }
            emp_file.close();
            cout << COLOR_GREEN << "--> Data from companies_employees.txt loaded" << COLOR_RESET << endl;
        }
        else {
            cout << COLOR_YELLOW << "--> companies_employees.txt not found, starting fresh" << COLOR_RESET << endl;
        }

        cout << COLOR_GREEN << "Data loading complete!" << COLOR_RESET << endl;
    }

    // Function to update all user data to file 
    void update_user_files() {
        ofstream file("users.txt");
        if (file.is_open()) {
            for (int i = 0; i < user_count; i++) {
                string approvedFlag = users[i]->get_approved() ? "1" : "0";
                string frozenFlag = users[i]->get_frozen() ? "1" : "0";

                file << users[i]->get_account_id() << "," << users[i]->get_name() << ","
                    << users[i]->get_address() << "," << users[i]->get_cnic() << ","
                    << users[i]->get_user_id() << "," << users[i]->get_password() << ","
                    << users[i]->get_balance() << "," << users[i]->get_account_type() << ","
                    << users[i]->get_daily_limit() << "," << approvedFlag << "," << frozenFlag << ","
                    << users[i]->get_today_withdrawn() << "," << users[i]->get_password() << endl;
            }
            file.close();
        }
        else {
            cout << COLOR_RED << "Error: Could not update user files!" << COLOR_RESET << endl;
        }
    }

    // Function to update all company data to file 
    void update_company_files() {
        ofstream file("companies.txt");
        if (file.is_open()) {
            for (int i = 0; i < company_count; i++) {
                string approvedFlag = companies[i]->get_approved() ? "1" : "0";
                string frozenFlag = companies[i]->get_frozen() ? "1" : "0";

                file << companies[i]->get_account_id() << "," << companies[i]->get_company_name() << ","
                    << companies[i]->get_company_address() << "," << companies[i]->get_tax_number() << ","
                    << companies[i]->get_user_id() << "," << companies[i]->get_password() << ","
                    << companies[i]->get_balance() << "," << companies[i]->get_account_type() << ","
                    << companies[i]->get_daily_limit() << "," << approvedFlag << "," << frozenFlag << ","
                    << companies[i]->get_today_withdrawn() << "," << companies[i]->get_password() << endl;
            }
            file.close();
        }
        else {
            cout << COLOR_RED << "Error: Could not update company files!" << COLOR_RESET << endl;
        }
    }

    // Function to register a new user client
    void register_user_client() {
        cout << "\n========================================" << endl;
        cout << COLOR_YELLOW << "   REGISTER NEW USER CLIENT             " << endl << COLOR_RESET;
        cout << "========================================" << endl;

        if (user_count >= user_capacity) {
            cout << COLOR_RED << "System capacity reached! Cannot add more users." << COLOR_RESET << endl;
            return;
        }

        user_client* user = new user_client();
        char input[250];

        // Get full name
        cout << "Enter full name: ";
        cin.ignore();
        cin.getline(input, 250);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
            delete user;
            return;
        }

        trim_whitespace(input);

        if (str_len(input) == 0 || is_only_whitespace(input)) {
            cout << COLOR_RED << "Name cannot be empty!" << COLOR_RESET << endl;
            delete user;
            return;
        }
        user->set_name(input);

        // Get address
        cout << "Enter address: ";
        cin.getline(input, 250);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
            delete user;
            return;
        }

        trim_whitespace(input);
        user->set_address(input);

        // Get and validate phone
        do {
            cout << "Enter phone (10-15 digits): ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            trim_whitespace(input);

            if (validate_phone(input)) {
                user->set_phone(input);
                break;
            }
            cout << COLOR_RED << "Invalid phone format, try again." << COLOR_RESET << endl;
        } while (true);

        // Get and validate CNIC
        do {
            cout << "Enter CNIC (13 digits without dashes): ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            trim_whitespace(input);

            if (!validate_cnic(input)) {
                cout << COLOR_RED << "Invalid CNIC, must be exactly 13 digits" << COLOR_RESET << endl;
                continue;
            }

            if (cnic_exists(input)) {
                cout << COLOR_RED << "CNIC already registered!" << COLOR_RESET << endl;
                continue;
            }

            user->set_cnic(input);
            break;
        } while (true);

        // Get login ID
        do {
            cout << "Enter login ID: ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            trim_whitespace(input);

            if (str_len(input) == 0 || is_only_whitespace(input)) {
                cout << COLOR_RED << "Login ID cannot be empty!" << COLOR_RESET << endl;
                continue;
            }

            if (user_id_exists(input)) {
                cout << COLOR_MAGENTA << "User ID already exists! Please choose a different one." << endl << COLOR_RESET;
                continue;
            }

            user->set_user_id(input);
            break;
        } while (true);

        // Get and validate password
        do {
            cout << "Enter password (minimum 6 characters): ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            if (validate_password(input)) {
                user->set_password(input);
                break;
            }
            cout << COLOR_RED << "Password validation failed, try again." << COLOR_RESET << endl;
        } while (true);

        // Get daily withdrawal limit
        long long limit;
        do {
            cout << "Enter estimated daily withdrawal limit (max 500,000 PKR): ";
            if (!get_long_long_input(limit, 1, 500000)) {
                cout << COLOR_RED << "Invalid limit! Try again." << COLOR_RESET << endl;
                continue;
            }
            break;
        } while (true);

        user->set_daily_limit(limit);

        // Assign account type based on limit
        if (limit <= 100000) {
            user->set_account_type("Bronze");
        }
        else if (limit <= 500000) {
            user->set_account_type("Gold");
        }

        // Set initial values
        user->set_account_id(next_user_id++);
        user->set_balance(0);
        user->set_approved(false);
        user->set_frozen(false);

        char current_date[20];
        get_current_date(current_date);
        user->set_last_transaction_date(current_date);

        // Add to system
        users[user_count++] = user;

        if (user->save_to_file()) {
            cout << COLOR_GREEN << "\nUser registered successfully!" << endl << COLOR_RESET;
            cout << COLOR_CYAN << "Account ID: " << user->get_account_id() << endl << COLOR_RESET;
            cout << COLOR_CYAN << "Account Type: " << user->get_account_type() << endl << COLOR_RESET;
            cout << COLOR_BLUE << "Status: Waiting for approval from banking employee." << endl << COLOR_RESET;
            log_activity("New user registered", user->get_account_id());
        }
    }

    // Function to register a new company client
    void register_company_client() {
        cout << "\n========================================" << endl;
        cout << COLOR_YELLOW << "   REGISTER NEW COMPANY CLIENT          " << endl << COLOR_RESET;
        cout << "========================================" << endl;

        if (company_count >= company_capacity) {
            cout << COLOR_RED << "System capacity reached! Cannot add more companies." << COLOR_RESET << endl;
            return;
        }

        company_client* company = new company_client();
        char input[250];

        // Get company name
        cout << "Enter company name: ";
        cin.ignore();
        cin.getline(input, 250);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
            delete company;
            return;
        }

        trim_whitespace(input);

        if (str_len(input) == 0 || is_only_whitespace(input)) {
            cout << COLOR_RED << "Company name cannot be empty" << COLOR_RESET << endl;
            delete company;
            return;
        }
        company->set_company_name(input);

        // Get company address
        cout << "Enter company address: ";
        cin.getline(input, 250);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
            delete company;
            return;
        }

        trim_whitespace(input);
        company->set_company_address(input);

        // Get tax number
        do {
            cout << "Enter tax number: ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            trim_whitespace(input);

            if (str_len(input) == 0 || is_only_whitespace(input)) {
                cout << COLOR_RED << "Tax number cannot be empty!" << COLOR_RESET << endl;
                continue;
            }

            if (tax_number_exists(input)) {
                cout << COLOR_RED << "Tax number already registered!" << COLOR_RESET << endl;
                continue;
            }

            company->set_tax_number(input);
            break;
        } while (true);

        // Get daily withdrawal limit
        long long limit;
        do {
            cout << "Enter estimated daily withdrawal limit (max 20,000,000 PKR): ";
            if (!get_long_long_input(limit, 1, 20000000)) {
                cout << COLOR_RED << "Invalid limit! Try again." << COLOR_RESET << endl;
                continue;
            }
            break;
        } while (true);

        company->set_daily_limit(limit);

        // Assign account type based on limit
        if (limit <= 100000) {
            company->set_account_type("Bronze");
        }
        else if (limit <= 500000) {
            company->set_account_type("Gold");
        }
        else if (limit <= 20000000) {
            company->set_account_type("Business");
        }

        // Get company account ID
        do {
            cout << "Enter company account ID: ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            trim_whitespace(input);

            if (str_len(input) == 0 || is_only_whitespace(input)) {
                cout << COLOR_RED << "Account ID cannot be empty!" << COLOR_RESET << endl;
                continue;
            }

            if (user_id_exists(input)) {
                cout << COLOR_MAGENTA << "Company ID already exists! Please choose a different one." << endl << COLOR_RESET;
                continue;
            }

            company->set_user_id(input);
            break;
        } while (true);

        // Get and validate password
        do {
            cout << "Enter account password (minimum 6 characters): ";
            cin.getline(input, 250);

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << COLOR_RED << "Input too long!" << COLOR_RESET << endl;
                continue;
            }

            if (validate_password(input)) {
                company->set_password(input);
                break;
            }
            cout << COLOR_RED << "Password validation failed, try again." << COLOR_RESET << endl;
        } while (true);

        // Get employees
        int emp_count;
        do {
            cout << "Enter number of employees to add (0 to skip): ";
            if (!get_int_input(emp_count, 0, 50)) {
                continue;
            }
            break;
        } while (true);

        if (emp_count > 0) {
            cout << "Enter employee user IDs (these users must already be registered):" << endl;
            for (int i = 0; i < emp_count; i++) {
                int emp_id;
                do {
                    cout << "Employee " << (i + 1) << " ID: ";
                    if (!get_int_input(emp_id, 1, 100000)) {
                        continue;
                    }
                    break;
                } while (true);

                // Check if user exists
                bool found = false;
                for (int j = 0; j < user_count; j++) {
                    if (users[j]->get_account_id() == emp_id) {
                        found = true;
                        break;
                    }
                }

                if (found) {
                    company->add_employee(emp_id);
                    cout << COLOR_GREEN << "  Employee added successfully" << COLOR_RESET << endl;
                }
                else {
                    cout << COLOR_RED << "User ID " << emp_id << " not found, skipping..." << COLOR_RESET << endl;
                }
            }
        }

        // Set initial values
        company->set_account_id(next_company_id++);
        company->set_balance(0);
        company->set_approved(false);
        company->set_frozen(false);

        char current_date[20];
        get_current_date(current_date);
        company->set_last_transaction_date(current_date);

        // Add to system
        companies[company_count++] = company;

        if (company->save_to_file()) {
            company->save_employees_to_file();
            cout << COLOR_BLUE << "\nCompany registered successfully!" << endl << COLOR_RESET;
            cout << COLOR_BLUE << "Company ID: " << company->get_account_id() << endl << COLOR_RESET;
            cout << COLOR_BLUE << "Account Type: " << company->get_account_type() << endl << COLOR_RESET;
            cout << COLOR_BLUE << "Status: Waiting for approval from banking employee." << endl << COLOR_RESET;
            log_activity("New company registered", company->get_account_id());
        }
    }

    // Function to find user by credentials (login)
    user_client* find_user_by_credentials(const char* uid, const char* pwd) {
        for (int i = 0; i < user_count; i++) {
            if (str_compare(users[i]->get_user_id(), uid) &&
                str_compare(users[i]->get_password(), pwd)) {
                return users[i];
            }
        }
        return nullptr;
    }

    // Function to find company by credentials (login)
    company_client* find_company_by_credentials(const char* uid, const char* pwd) {
        for (int i = 0; i < company_count; i++) {
            if (str_compare(companies[i]->get_user_id(), uid) &&
                str_compare(companies[i]->get_password(), pwd)) {
                return companies[i];
            }
        }
        return nullptr;
    }

    // Function to display user client menu and handle operations
    void user_client_menu(user_client* user) {
        while (true) {
            cout << "\n========================================" << endl;
            cout << COLOR_YELLOW << "        USER CLIENT MENU                " << endl << COLOR_RESET;
            cout << "========================================" << endl;
            cout << "1. View Account Information" << endl;
            cout << "2. Deposit Funds" << endl;
            cout << "3. Withdraw Funds" << endl;
            cout << "4. Transfer Funds" << endl;
            cout << "5. View Transaction History" << endl;
            cout << "6. Logout" << endl;
            cout << "========================================" << endl;
            cout << "Enter choice: ";

            int choice;
            if (!get_int_input(choice, 1, 6)) {
                continue;
            }

            if (choice == 1) {
                // Display account information
                user->display_info();
            }
            else if (choice == 2) {
                // Deposit funds
                if (!user->get_approved()) {
                    cout << COLOR_MAGENTA << "Account not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- DEPOSIT FUNDS ---" << endl;
                long long amount;
                cout << "Enter amount to deposit: ";
                if (!get_long_long_input(amount, 1, MAX_SINGLE_TRANSACTION)) {
                    continue;
                }

                if (user->deposit(amount)) {
                    char date[20];
                    get_current_date(date);
                    transaction t(date, user->get_account_id(), amount, "Deposit");
                    t.save_to_file();
                    update_user_files();
                    cout << COLOR_GREEN << "Deposit successful!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "New balance: " << user->get_balance() << " PKR" << endl << COLOR_RESET;
                }
            }
            else if (choice == 3) {
                // Withdraw funds
                if (!user->get_approved()) {
                    cout << COLOR_RED << "Account not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                if (user->get_frozen()) {
                    cout << COLOR_RED << "Account is frozen! Contact bank employee." << endl << COLOR_RESET;
                    continue;
                }

                if (user->get_card_count() == 0) {
                    cout << COLOR_MAGENTA << "No cards available! Contact bank to issue a card." << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- WITHDRAW FUNDS ---" << endl;
                cout << "Select card:" << endl;
                for (int i = 0; i < user->get_card_count(); i++) {
                    cout << (i + 1) << ". ";
                    user->get_card(i)->display();
                }

                int card_idx;
                if (!get_int_input(card_idx, 1, user->get_card_count())) {
                    continue;
                }
                card_idx--;

                card* c = user->get_card(card_idx);

                if (c->get_fraud_trans()) {
                    cout << COLOR_RED << "This card has been marked as fraudulent! Contact bank." << COLOR_RESET << endl;
                    continue;
                }

                cout << "Enter PIN: ";
                char pin[10];
                cin >> pin;
                cin.ignore(10000, '\n');

                if (!c->verify_pin(pin)) {
                    if (c->get_fraud_trans()) {
                        user->set_frozen(true);
                        update_user_files();
                        cout << COLOR_RED << "Account frozen due to suspicious activity!" << endl << COLOR_RESET;
                        cout << COLOR_RED << "Please contact the bank immediately." << endl << COLOR_RESET;
                        log_activity("Account frozen - multiple failed PIN attempts", user->get_account_id());
                    }
                    continue;
                }

                long long amount;
                cout << "Enter amount to withdraw: ";
                if (!get_long_long_input(amount, 1, MAX_SINGLE_TRANSACTION)) {
                    continue;
                }

                if (user->withdraw(amount)) {
                    char date[20];
                    get_current_date(date);
                    transaction t(date, user->get_account_id(), amount, "Withdrawal");
                    t.save_to_file();
                    update_user_files();
                    cout << COLOR_CYAN << "Withdrawal successful!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "New balance: " << user->get_balance() << " PKR" << endl << COLOR_RESET;
                }
            }
            else if (choice == 4) {
                // Transfer funds
                if (!user->get_approved()) {
                    cout << COLOR_RED << "Account not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                if (user->get_frozen()) {
                    cout << COLOR_RED << "Account is frozen! Contact bank employee." << endl << COLOR_RESET;
                    continue;
                }

                if (user->get_card_count() == 0) {
                    cout << COLOR_RED << "No cards available!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- TRANSFER FUNDS ---" << endl;
                cout << "Select card:" << endl;
                for (int i = 0; i < user->get_card_count(); i++) {
                    cout << (i + 1) << ". ";
                    user->get_card(i)->display();
                }

                int card_idx;
                if (!get_int_input(card_idx, 1, user->get_card_count())) {
                    continue;
                }
                card_idx--;

                card* c = user->get_card(card_idx);

                if (c->get_fraud_trans()) {
                    cout << COLOR_RED << "This card has been marked as fraudulent! Contact bank." << COLOR_RESET << endl;
                    continue;
                }

                cout << "Enter PIN: ";
                char pin[10];
                cin >> pin;
                cin.ignore(10000, '\n');

                if (!c->verify_pin(pin)) {
                    if (c->get_fraud_trans()) {
                        user->set_frozen(true);
                        update_user_files();
                        cout << COLOR_RED << "Account frozen due to suspicious activity!" << endl << COLOR_RESET;
                        log_activity("Account frozen - multiple failed PIN attempts", user->get_account_id());
                    }
                    continue;
                }

                int recipient_id;
                cout << "Enter recipient account ID: ";
                if (!get_int_input(recipient_id, 1, 100000)) {
                    continue;
                }

                // Check for self-transfer
                if (recipient_id == user->get_account_id()) {
                    cout << COLOR_RED << "Cannot transfer to your own account!" << COLOR_RESET << endl;
                    continue;
                }

                user_client* recipient = nullptr;
                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == recipient_id) {
                        recipient = users[i];
                        break;
                    }
                }

                if (!recipient) {
                    cout << COLOR_YELLOW << "Recipient not found!" << endl << COLOR_RESET;
                    continue;
                }

                if (!recipient->get_approved()) {
                    cout << COLOR_YELLOW << "Recipient account not approved!" << endl << COLOR_RESET;
                    continue;
                }

                if (recipient->get_frozen()) {
                    cout << COLOR_YELLOW << "Recipient account is frozen!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "Recipient: " << COLOR_CYAN << recipient->get_name() << COLOR_RESET << endl;

                long long amount;
                cout << "Enter amount to transfer: ";
                if (!get_long_long_input(amount, 1, MAX_SINGLE_TRANSACTION)) {
                    continue;
                }

                if (user->transfer(recipient_id, amount, pin)) {
                    recipient->deposit(amount);
                    update_user_files();
                    cout << COLOR_BLUE << "Transfer successful!" << endl << COLOR_RESET;
                    cout << COLOR_BLUE << "New balance: " << user->get_balance() << " PKR" << endl << COLOR_RESET;
                    cout << COLOR_BLUE << "Transferred to: " << recipient->get_name() << endl << COLOR_RESET;
                }
            }
            else if (choice == 5) {
                // View transaction history
                view_transaction_history(user->get_account_id());
            }
            else if (choice == 6) {
                // Logout
                cout << COLOR_GREEN << "Logging out..." << COLOR_RESET << endl;
                log_activity("User logged out", user->get_account_id());
                break;
            }
        }
    }


    // Function to display company client menu and handle operations
    void company_client_menu(company_client* company) {
        while (true) {
            cout << "\n========================================" << endl;
            cout << COLOR_YELLOW << "      COMPANY CLIENT MENU               " << endl << COLOR_RESET;
            cout << "========================================" << endl;
            cout << "1. View Account Information" << endl;
            cout << "2. Deposit Funds" << endl;
            cout << "3. Withdraw Funds" << endl;
            cout << "4. Transfer Funds to Employee" << endl;
            cout << "5. View Transaction History" << endl;
            cout << "6. Request Loan" << endl;
            cout << "7. Logout" << endl;
            cout << "========================================" << endl;
            cout << "Enter choice: ";

            int choice;
            if (!get_int_input(choice, 1, 7)) {
                continue;
            }

            if (choice == 1) {
                // Display company information
                company->display_info();
            }
            else if (choice == 2) {
                // Deposit funds
                if (!company->get_approved()) {
                    cout << COLOR_MAGENTA << "\nAccount not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- DEPOSIT FUNDS ---" << endl;
                long long amount;
                cout << "Enter amount to deposit: ";
                if (!get_long_long_input(amount, 1, MAX_SINGLE_TRANSACTION)) {
                    continue;
                }

                if (company->deposit(amount)) {
                    char date[20];
                    get_current_date(date);
                    transaction t(date, company->get_account_id(), amount, "Deposit");
                    t.save_to_file();
                    update_company_files();
                    cout << COLOR_GREEN << "\nDeposit successful!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "New balance: " << company->get_balance() << " PKR" << endl << COLOR_RESET;
                }
            }
            else if (choice == 3) {
                // Withdraw funds
                if (!company->get_approved()) {
                    cout << COLOR_RED << "\nAccount not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                if (company->get_frozen()) {
                    cout << COLOR_RED << "\nAccount is frozen!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- WITHDRAW FUNDS ---" << endl;
                long long amount;
                cout << "Enter amount to withdraw: ";
                if (!get_long_long_input(amount, 1, MAX_SINGLE_TRANSACTION)) {
                    continue;
                }

                if (company->withdraw(amount)) {
                    char date[20];
                    get_current_date(date);
                    transaction t(date, company->get_account_id(), amount, "Withdrawal");
                    t.save_to_file();
                    update_company_files();
                    cout << COLOR_CYAN << "\nWithdrawal successful!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "New balance: " << company->get_balance() << " PKR" << endl << COLOR_RESET;
                }
            }
            else if (choice == 4) {
                // Transfer to employee
                if (!company->get_approved()) {
                    cout << COLOR_RED << "\nAccount not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                if (company->get_frozen()) {
                    cout << COLOR_RED << "\nAccount is frozen!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- TRANSFER FUNDS ---" << endl;
                int recipient_id;
                cout << "Enter recipient user account ID: ";
                if (!get_int_input(recipient_id, 1, 100000)) {
                    continue;
                }

                user_client* recipient = nullptr;
                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == recipient_id) {
                        recipient = users[i];
                        break;
                    }
                }

                if (!recipient) {
                    cout << COLOR_YELLOW << "\nRecipient not found!" << endl << COLOR_RESET;
                    continue;
                }

                if (!recipient->get_approved()) {
                    cout << COLOR_YELLOW << "\nRecipient account not approved!" << endl << COLOR_RESET;
                    continue;
                }

                if (recipient->get_frozen()) {
                    cout << COLOR_YELLOW << "\nRecipient account is frozen!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "Recipient: " << COLOR_CYAN << recipient->get_name() << COLOR_RESET << endl;

                long long amount;
                cout << "Enter amount to transfer: ";
                if (!get_long_long_input(amount, 1, MAX_SINGLE_TRANSACTION)) {
                    continue;
                }

                if (company->withdraw(amount)) {
                    recipient->deposit(amount);
                    char date[20];
                    get_current_date(date);
                    transaction t(date, company->get_account_id(), amount, "Transfer", recipient_id);
                    t.save_to_file();
                    update_company_files();
                    update_user_files();
                    cout << COLOR_BLUE << "\nTransfer successful!" << endl << COLOR_RESET;
                    cout << COLOR_BLUE << "New balance: " << company->get_balance() << " PKR" << endl << COLOR_RESET;
                }
            }
            else if (choice == 5) {
                // View transaction history
                view_transaction_history(company->get_account_id());
            }
            else if (choice == 6) {
                // Request loan
                if (!company->get_approved()) {
                    cout << COLOR_RED << "\nAccount not yet approved!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n--- REQUEST LOAN ---" << endl;
                long long amount;
                cout << "Enter loan amount: ";
                if (!get_long_long_input(amount, 1, 100000000)) {
                    continue;
                }

                ofstream loan_file("loan_requests.txt", ios::app);
                if (loan_file.is_open()) {
                    loan_file << company->get_account_id() << ","
                        << company->get_company_name() << ","
                        << amount << ",pending" << endl;
                    loan_file.close();
                    cout << COLOR_GREEN << "\nLoan request submitted successfully!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "Amount requested: " << amount << " PKR" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "Status: Pending approval" << endl << COLOR_RESET;
                    log_activity("Loan request submitted", company->get_account_id());
                }
                else {
                    cout << COLOR_RED << "Error: Could not submit loan request!" << COLOR_RESET << endl;
                }
            }
            else if (choice == 7) {
                // Logout
                cout << COLOR_GREEN << "\nLogging out..." << COLOR_RESET << endl;
                log_activity("Company logged out", company->get_account_id());
                break;
            }
        }
    }

    // Function to view transaction history for an account
    void view_transaction_history(int account_id) {
        cout << "\n========================================" << endl;
        cout << COLOR_YELLOW << "      TRANSACTION HISTORY               " << endl << COLOR_RESET;
        cout << "========================================" << endl;

        ifstream file("transactions.txt");
        if (!file.is_open()) {
            cout << COLOR_YELLOW << "No transactions found!" << endl << COLOR_RESET;
            return;
        }

        char line[200];
        bool found = false;
        int count = 0;

        while (file.getline(line, 200)) {
            if (str_len(line) == 0) continue;

            int field = 0;
            int start = 0;
            int client_id = 0;

            // Parse to get client_id
            for (int i = 0; i <= str_len(line); i++) {
                if (line[i] == ',' || line[i] == '\0') {
                    if (field == 1) {
                        char temp[50];
                        int idx = 0;
                        for (int j = start; j < i; j++) {
                            temp[idx++] = line[j];
                        }
                        temp[idx] = '\0';
                        client_id = string_to_int(temp);
                        break;
                    }
                    field++;
                    start = i + 1;
                }
            }

            if (client_id == account_id) {
                cout << COLOR_CYAN << ++count << ". " << line << COLOR_RESET << endl;
                found = true;
            }
        }

        if (!found) {
            cout << COLOR_YELLOW << "No transactions found for this account!" << endl << COLOR_RESET;
        }
        else {
            cout << COLOR_GREEN << "\nTotal transactions: " << count << COLOR_RESET << endl;
        }

        file.close();
    }

    // Function for banking employee menu
    void banking_employee_menu() {
        cout << "\n========================================" << endl;
        cout << COLOR_YELLOW << "     BANKING EMPLOYEE LOGIN             " << endl << COLOR_RESET;
        cout << "========================================" << endl;

        char uid[50];
        char pwd[50];
        int login_attempts = 0;
        bool authenticated = false;

        while (login_attempts < MAX_LOGIN_ATTEMPTS && !authenticated) {
            cout << "\nEnter admin user ID: ";
            cin >> uid;
            cin.ignore(10000, '\n');

            cout << "Enter admin password: ";
            cin >> pwd;
            cin.ignore(10000, '\n');

            // Admin verification
            ifstream admin_file("admin.txt");

            if (admin_file.is_open()) {
                char line[200];
                while (admin_file.getline(line, 200)) {
                    char file_uid[50], file_pwd[50];
                    int field = 0;
                    int start = 0;

                    for (int i = 0; i <= str_len(line); i++) {
                        if (line[i] == ',' || line[i] == '\0') {
                            char temp[50];
                            int idx = 0;
                            for (int j = start; j < i; j++) {
                                temp[idx++] = line[j];
                            }
                            temp[idx] = '\0';

                            if (field == 0) str_c(file_uid, temp);
                            else if (field == 1) str_c(file_pwd, temp);

                            field++;
                            start = i + 1;
                        }
                    }

                    if (str_compare(file_uid, uid) && str_compare(file_pwd, pwd)) {
                        authenticated = true;
                        break;
                    }
                }
                admin_file.close();
            }

            if (!authenticated) {
                login_attempts++;
                cout << COLOR_RED << "\nInvalid credentials! Attempts remaining: "
                    << (MAX_LOGIN_ATTEMPTS - login_attempts) << COLOR_RESET << endl;

                if (login_attempts >= MAX_LOGIN_ATTEMPTS) {
                    cout << COLOR_RED << "Maximum login attempts exceeded!" << COLOR_RESET << endl;
                    log_activity("Failed admin login attempts exceeded");
                    return;
                }
            }
        }

        cout << COLOR_GREEN << "\nLogin successful!" << COLOR_RESET << endl;
        log_activity("Admin logged in");

        while (true) {
            cout << "\n========================================" << endl;
            cout << COLOR_YELLOW << "     BANKING EMPLOYEE MENU              " << endl << COLOR_RESET;
            cout << "========================================" << endl;
            cout << "1. View All Accounts" << endl;
            cout << "2. Approve/Reject User Accounts" << endl;
            cout << "3. Approve/Reject Company Accounts" << endl;
            cout << "4. Approve/Reject Loan Requests" << endl;
            cout << "5. View Transaction History" << endl;
            cout << "6. Freeze/Unfreeze Account" << endl;
            cout << "7. Close Account" << endl;
            cout << "8. Issue New Card to User" << endl;
            cout << "9. Logout" << endl;
            cout << "========================================" << endl;
            cout << "Enter choice: ";

            int choice;
            if (!get_int_input(choice, 1, 9)) {
                continue;
            }

            if (choice == 1) {
                // View all accounts
                cout << "\n" << COLOR_CYAN << "--- USER ACCOUNTS ---" << COLOR_RESET << endl;
                cout << "Total: " << user_count << endl;
                for (int i = 0; i < user_count; i++) {
                    users[i]->display_info();
                }

                cout << "\n" << COLOR_CYAN << "--- COMPANY ACCOUNTS ---" << COLOR_RESET << endl;
                cout << "Total: " << company_count << endl;
                for (int i = 0; i < company_count; i++) {
                    companies[i]->display_info();
                }
            }
            else if (choice == 2) {
                // Approve/reject user accounts
                cout << "\n" << COLOR_CYAN << "--- PENDING USER ACCOUNTS ---" << COLOR_RESET << endl;
                bool has_pending = false;
                for (int i = 0; i < user_count; i++) {
                    if (!users[i]->get_approved()) {
                        users[i]->display_info();
                        has_pending = true;
                    }
                }

                if (!has_pending) {
                    cout << COLOR_YELLOW << "No pending user accounts!" << endl << COLOR_RESET;
                    continue;
                }

                int acc_id;
                cout << "\nEnter account ID to approve/reject (0 to cancel): ";
                if (!get_int_input(acc_id, 0, 100000)) {
                    continue;
                }

                if (acc_id == 0) continue;

                user_client* user = nullptr;
                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == acc_id) {
                        user = users[i];
                        break;
                    }
                }

                if (!user) {
                    cout << COLOR_RED << "Account not found!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n1. Approve\n2. Reject\nChoice: ";
                int decision;
                if (!get_int_input(decision, 1, 2)) {
                    continue;
                }

                if (decision == 1) {
                    user->set_approved(true);

                    // Generate card
                    char card_num[17];
                    generate_unique_card_number(card_num);

                    char pin[5];
                    do {
                        cout << "Enter 4-digit PIN for card: ";
                        cin >> pin;
                        cin.ignore(10000, '\n');

                        if (!validate_pin(pin)) {
                            cout << COLOR_RED << "Invalid PIN! Try again." << COLOR_RESET << endl;
                            continue;
                        }
                        break;
                    } while (true);

                    card* c = new card(user->get_account_id(), card_num, pin);
                    user->add_card(c);
                    c->save_to_file();
                    update_user_files();

                    cout << COLOR_GREEN << "\nAccount approved and card issued!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "Card number: " << card_num << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "PIN: " << pin << endl << COLOR_RESET;
                    log_activity("User account approved", acc_id);
                }
                else if (decision == 2) {
                    // Remove user from system
                    for (int i = 0; i < user_count; i++) {
                        if (users[i]->get_account_id() == acc_id) {
                            delete users[i];
                            for (int j = i; j < user_count - 1; j++) {
                                users[j] = users[j + 1];
                            }
                            user_count--;
                            break;
                        }
                    }
                    update_user_files();
                    cout << COLOR_GREEN << "\nAccount rejected and removed!" << endl << COLOR_RESET;
                    log_activity("User account rejected", acc_id);
                }
            }
            else if (choice == 3) {
                // Approve/reject company accounts
                cout << "\n" << COLOR_CYAN << "--- PENDING COMPANY ACCOUNTS ---" << COLOR_RESET << endl;
                bool has_pending = false;
                for (int i = 0; i < company_count; i++) {
                    if (!companies[i]->get_approved()) {
                        companies[i]->display_info();
                        has_pending = true;
                    }
                }

                if (!has_pending) {
                    cout << COLOR_YELLOW << "No pending company accounts!" << endl << COLOR_RESET;
                    continue;
                }

                int comp_id;
                cout << "\nEnter company ID to approve/reject (0 to cancel): ";
                if (!get_int_input(comp_id, 0, 100000)) {
                    continue;
                }

                if (comp_id == 0) continue;

                company_client* company = nullptr;
                for (int i = 0; i < company_count; i++) {
                    if (companies[i]->get_account_id() == comp_id) {
                        company = companies[i];
                        break;
                    }
                }

                if (!company) {
                    cout << COLOR_RED << "Company not found!" << endl << COLOR_RESET;
                    continue;
                }

                cout << "\n1. Approve\n2. Reject\nChoice: ";
                int decision;
                if (!get_int_input(decision, 1, 2)) {
                    continue;
                }

                if (decision == 1) {
                    company->set_approved(true);
                    update_company_files();
                    cout << COLOR_GREEN << "\nCompany account approved!" << endl << COLOR_RESET;
                    log_activity("Company account approved", comp_id);
                }
                else if (decision == 2) {
                    // Remove company
                    for (int i = 0; i < company_count; i++) {
                        if (companies[i]->get_account_id() == comp_id) {
                            delete companies[i];
                            for (int j = i; j < company_count - 1; j++) {
                                companies[j] = companies[j + 1];
                            }
                            company_count--;
                            break;
                        }
                    }
                    update_company_files();
                    cout << COLOR_GREEN << "\nCompany account rejected and removed!" << endl << COLOR_RESET;
                    log_activity("Company account rejected", comp_id);
                }
            }
            else if (choice == 4) {
                // Approve/reject loan requests
                cout << "\n" << COLOR_CYAN << "--- PENDING LOAN REQUESTS ---" << COLOR_RESET << endl;

                ifstream loan_file("loan_requests.txt");
                if (!loan_file.is_open()) {
                    cout << COLOR_YELLOW << "No loan requests found!" << endl << COLOR_RESET;
                    continue;
                }

                char line[200];
                bool found = false;
                int loan_count = 0;

                while (loan_file.getline(line, 200)) {
                    if (str_len(line) > 0) {
                        // Parse: company_id,company_name,amount,status
                        char status[20];
                        int field = 0;
                        int start = 0;

                        for (int i = 0; i <= str_len(line); i++) {
                            if (line[i] == ',' || line[i] == '\0') {
                                if (field == 3) {
                                    char temp[20];
                                    int idx = 0;
                                    for (int j = start; j < i; j++) {
                                        temp[idx++] = line[j];
                                    }
                                    temp[idx] = '\0';
                                    str_c(status, temp);
                                }
                                field++;
                                start = i + 1;
                            }
                        }

                        if (str_compare(status, "pending")) {
                            cout << COLOR_CYAN << ++loan_count << ". " << line << COLOR_RESET << endl;
                            found = true;
                        }
                    }
                }
                loan_file.close();

                if (!found) {
                    cout << COLOR_YELLOW << "No pending loan requests!" << endl << COLOR_RESET;
                    continue;
                }

                int comp_id;
                cout << "\nEnter company ID to process loan (0 to cancel): ";
                if (!get_int_input(comp_id, 0, 100000)) {
                    continue;
                }

                if (comp_id == 0) continue;

                cout << "1. Approve\n2. Reject\nChoice: ";
                int decision;
                if (!get_int_input(decision, 1, 2)) {
                    continue;
                }

                if (decision == 1) {
                    // Find the company
                    company_client* company = nullptr;
                    for (int i = 0; i < company_count; i++) {
                        if (companies[i]->get_account_id() == comp_id) {
                            company = companies[i];
                            break;
                        }
                    }

                    if (!company) {
                        cout << COLOR_RED << "Company not found!" << endl << COLOR_RESET;
                        continue;
                    }

                    // Find loan amount from file
                    ifstream read_loan("loan_requests.txt");
                    long long loan_amount = 0;

                    while (read_loan.getline(line, 200)) {
                        if (str_len(line) > 0) {
                            int read_comp_id = 0;
                            int field = 0;
                            int start = 0;

                            for (int i = 0; i <= str_len(line); i++) {
                                if (line[i] == ',' || line[i] == '\0') {
                                    char temp[50];
                                    int idx = 0;
                                    for (int j = start; j < i; j++) {
                                        temp[idx++] = line[j];
                                    }
                                    temp[idx] = '\0';

                                    if (field == 0) read_comp_id = string_to_int(temp);
                                    else if (field == 2) loan_amount = string_to_long_long(temp);

                                    field++;
                                    start = i + 1;
                                }
                            }

                            if (read_comp_id == comp_id) {
                                break;
                            }
                        }
                    }
                    read_loan.close();

                    // Credit the loan amount to company account
                    company->deposit(loan_amount);
                    update_company_files();

                    // Record transaction
                    char date[20];
                    get_current_date(date);
                    transaction t(date, company->get_account_id(), loan_amount, "Loan-Approved");
                    t.save_to_file();

                    // Update loan status in file
                    ifstream in_file("loan_requests.txt");
                    ofstream out_file("loan_requests_temp.txt");

                    while (in_file.getline(line, 200)) {
                        if (str_len(line) > 0) {
                            int read_comp_id = 0;
                            int field = 0;
                            int start = 0;

                            for (int i = 0; i <= str_len(line); i++) {
                                if (line[i] == ',') {
                                    if (field == 0) {
                                        char temp[50];
                                        int idx = 0;
                                        for (int j = start; j < i; j++) {
                                            temp[idx++] = line[j];
                                        }
                                        temp[idx] = '\0';
                                        read_comp_id = string_to_int(temp);
                                        break;
                                    }
                                    field++;
                                    start = i + 1;
                                }
                            }

                            if (read_comp_id == comp_id) {
                                // Replace pending with approved
                                char new_line[200];
                                int idx = 0;
                                for (int i = 0; line[i] != '\0'; i++) {
                                    new_line[idx++] = line[i];
                                }
                                // Remove "pending"
                                idx -= 7;
                                str_c(new_line + idx, "approved");
                                out_file << new_line << endl;
                            }
                            else {
                                out_file << line << endl;
                            }
                        }
                    }

                    in_file.close();
                    out_file.close();

                    remove("loan_requests.txt");
                    rename("loan_requests_temp.txt", "loan_requests.txt");

                    cout << COLOR_GREEN << "\nLoan approved successfully!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "Amount credited: " << loan_amount << " PKR" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "New company balance: " << company->get_balance() << " PKR" << endl << COLOR_RESET;
                    log_activity("Loan approved", comp_id);
                }
                else if (decision == 2) {
                    // Update loan status to rejected
                    ifstream in_file("loan_requests.txt");
                    ofstream out_file("loan_requests_temp.txt");

                    while (in_file.getline(line, 200)) {
                        if (str_len(line) > 0) {
                            int read_comp_id = 0;
                            int field = 0;
                            int start = 0;

                            for (int i = 0; i <= str_len(line); i++) {
                                if (line[i] == ',') {
                                    if (field == 0) {
                                        char temp[50];
                                        int idx = 0;
                                        for (int j = start; j < i; j++) {
                                            temp[idx++] = line[j];
                                        }
                                        temp[idx] = '\0';
                                        read_comp_id = string_to_int(temp);
                                        break;
                                    }
                                    field++;
                                    start = i + 1;
                                }
                            }

                            if (read_comp_id == comp_id) {
                                char new_line[200];
                                int idx = 0;
                                for (int i = 0; line[i] != '\0'; i++) {
                                    new_line[idx++] = line[i];
                                }
                                idx -= 7;
                                str_c(new_line + idx, "rejected");
                                out_file << new_line << endl;
                            }
                            else {
                                out_file << line << endl;
                            }
                        }
                    }

                    in_file.close();
                    out_file.close();

                    remove("loan_requests.txt");
                    rename("loan_requests_temp.txt", "loan_requests.txt");

                    cout << COLOR_GREEN << "\nLoan rejected!" << endl << COLOR_RESET;
                    log_activity("Loan rejected", comp_id);
                }
            }
            else if (choice == 5) {
                // View transaction history
                int acc_id;
                cout << "\nEnter account ID: ";
                if (!get_int_input(acc_id, 1, 100000)) {
                    continue;
                }
                view_transaction_history(acc_id);
            }
            else if (choice == 6) {
                // Freeze/unfreeze account
                int acc_id;
                cout << "\nEnter account ID: ";
                if (!get_int_input(acc_id, 1, 100000)) {
                    continue;
                }

                cout << "1. Freeze\n2. Unfreeze\nChoice: ";
                int decision;
                if (!get_int_input(decision, 1, 2)) {
                    continue;
                }

                bool found = false;

                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == acc_id) {
                        users[i]->set_frozen(decision == 1);
                        update_user_files();
                        cout << COLOR_GREEN << "\nUser account " << (decision == 1 ? "frozen" : "unfrozen")
                            << "!" << endl << COLOR_RESET;
                        log_activity(decision == 1 ? "Account frozen" : "Account unfrozen", acc_id);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    for (int i = 0; i < company_count; i++) {
                        if (companies[i]->get_account_id() == acc_id) {
                            companies[i]->set_frozen(decision == 1);
                            update_company_files();
                            cout << COLOR_GREEN << "\nCompany account " << (decision == 1 ? "frozen" : "unfrozen")
                                << "!" << endl << COLOR_RESET;
                            log_activity(decision == 1 ? "Account frozen" : "Account unfrozen", acc_id);
                            found = true;
                            break;
                        }
                    }
                }

                if (!found) {
                    cout << COLOR_RED << "Account not found!" << endl << COLOR_RESET;
                }
            }
            else if (choice == 7) {
                // Close account
                cout << "\n--- CLOSE ACCOUNT ---" << endl;
                int acc_id;
                cout << "Enter account ID to close: ";
                if (!get_int_input(acc_id, 1, 100000)) {
                    continue;
                }

                cout << COLOR_RED << "WARNING: This will permanently delete the account!" << endl << COLOR_RESET;
                cout << "1. Confirm\n2. Cancel\nChoice: ";
                int confirm;
                if (!get_int_input(confirm, 1, 2)) {
                    continue;
                }

                if (confirm != 1) {
                    cout << COLOR_YELLOW << "Account closure cancelled." << endl << COLOR_RESET;
                    continue;
                }

                bool found = false;

                // Try to find and remove user account
                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == acc_id) {
                        if (users[i]->get_balance() > MIN_BALANCE) {
                            cout << COLOR_RED << "\nWARNING: Account has balance of "
                                << users[i]->get_balance() << " PKR" << endl << COLOR_RESET;
                            cout << "Please withdraw all funds before closing." << endl;
                            found = true;
                            break;
                        }

                        cout << "Closing account for: " << users[i]->get_name() << endl;

                        delete users[i];

                        // Shift array
                        for (int j = i; j < user_count - 1; j++) {
                            users[j] = users[j + 1];
                        }
                        user_count--;

                        update_user_files();
                        cout << COLOR_GREEN << "\nUser account closed successfully!" << endl << COLOR_RESET;
                        log_activity("User account closed", acc_id);
                        found = true;
                        break;
                    }
                }

                // Try to find and remove company account
                if (!found) {
                    for (int i = 0; i < company_count; i++) {
                        if (companies[i]->get_account_id() == acc_id) {
                            if (companies[i]->get_balance() > MIN_BALANCE) {
                                cout << COLOR_RED << "\nWARNING: Account has balance of "
                                    << companies[i]->get_balance() << " PKR" << endl << COLOR_RESET;
                                cout << "Please withdraw all funds before closing." << endl;
                                found = true;
                                break;
                            }

                            cout << "Closing account for: " << companies[i]->get_company_name() << endl;

                            delete companies[i];

                            // Shift array
                            for (int j = i; j < company_count - 1; j++) {
                                companies[j] = companies[j + 1];
                            }
                            company_count--;

                            update_company_files();
                            cout << COLOR_GREEN << "\nCompany account closed successfully!" << endl << COLOR_RESET;
                            log_activity("Company account closed", acc_id);
                            found = true;
                            break;
                        }
                    }
                }

                if (!found) {
                    cout << COLOR_RED << "Account not found!" << endl << COLOR_RESET;
                }
            }
            else if (choice == 8) {
                // Issue new card to user
                cout << "\n--- ISSUE NEW CARD ---" << endl;
                int acc_id;
                cout << "Enter user account ID: ";
                if (!get_int_input(acc_id, 1, 100000)) {
                    continue;
                }

                user_client* user = nullptr;
                for (int i = 0; i < user_count; i++) {
                    if (users[i]->get_account_id() == acc_id) {
                        user = users[i];
                        break;
                    }
                }

                if (!user) {
                    cout << COLOR_RED << "User not found!" << endl << COLOR_RESET;
                    continue;
                }

                if (!user->get_approved()) {
                    cout << COLOR_RED << "User account not approved yet!" << endl << COLOR_RESET;
                    continue;
                }

                // Generate card
                char card_num[17];
                generate_unique_card_number(card_num);

                char pin[5];
                do {
                    cout << "Enter 4-digit PIN for new card: ";
                    cin >> pin;
                    cin.ignore(10000, '\n');

                    if (!validate_pin(pin)) {
                        cout << COLOR_RED << "Invalid PIN! Try again." << COLOR_RESET << endl;
                        continue;
                    }
                    break;
                } while (true);

                card* c = new card(user->get_account_id(), card_num, pin);
                if (user->add_card(c)) {
                    c->save_to_file();
                    cout << COLOR_GREEN << "\nNew card issued successfully!" << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "Card number: " << card_num << endl << COLOR_RESET;
                    cout << COLOR_CYAN << "PIN: " << pin << endl << COLOR_RESET;
                    log_activity("New card issued", acc_id);
                }
                else {
                    delete c;
                }
            }
            else if (choice == 9) {
                // Logout
                cout << COLOR_GREEN << "\nLogging out..." << COLOR_RESET << endl;
                log_activity("Admin logged out");
                break;
            }
        }
    }

    // Main run function 
    void run() {
        while (true) {
            cout << "\n========================================" << endl;
            cout << COLOR_BOLD << COLOR_CYAN;
            cout << "                                        " << endl;
            cout << "     ABC BANK MANAGEMENT SYSTEM         " << endl;
            cout << "                                        " << endl;
            cout << COLOR_RESET;
            cout << "========================================" << endl;
            cout << "1. Register New User Client" << endl;
            cout << "2. Register New Company Client" << endl;
            cout << "3. User Client Login" << endl;
            cout << "4. Company Client Login" << endl;
            cout << "5. Banking Employee Login" << endl;
            cout << "6. Exit" << endl;
            cout << "========================================" << endl;
            cout << "Enter choice: ";

            int choice;
            if (!get_int_input(choice, 1, 6)) {
                continue;
            }

            if (choice == 1) {
                register_user_client();
            }
            else if (choice == 2) {
                register_company_client();
            }
            else if (choice == 3) {
                // User client login
                cout << "\n========================================" << endl;
                cout << COLOR_YELLOW << "        USER CLIENT LOGIN               " << endl << COLOR_RESET;
                cout << "========================================" << endl;

                char uid[50];
                char pwd[50];
                int login_attempts = 0;

                while (login_attempts < MAX_LOGIN_ATTEMPTS) {
                    cout << "\nEnter user ID: ";
                    cin >> uid;
                    cin.ignore(10000, '\n');

                    cout << "Enter password: ";
                    cin >> pwd;
                    cin.ignore(10000, '\n');

                    user_client* user = find_user_by_credentials(uid, pwd);
                    if (user) {
                        if (!user->get_approved()) {
                            cout << COLOR_MAGENTA << "\nAccount pending approval!" << endl << COLOR_RESET;
                            cout << "Please wait for bank employee approval." << endl;
                            break;
                        }
                        else if (user->get_frozen()) {
                            cout << COLOR_RED << "\nAccount is frozen!" << endl << COLOR_RESET;
                            cout << "Please contact the bank for assistance." << endl;
                            break;
                        }
                        else {
                            cout << COLOR_GREEN << "\nLogin successful!" << endl << COLOR_RESET;
                            cout << COLOR_CYAN << "Welcome, " << user->get_name() << "!" << endl << COLOR_RESET;
                            log_activity("User logged in", user->get_account_id());
                            user_client_menu(user);
                            break;
                        }
                    }
                    else {
                        login_attempts++;
                        cout << COLOR_RED << "\nInvalid credentials! Attempts remaining: "
                            << (MAX_LOGIN_ATTEMPTS - login_attempts) << COLOR_RESET << endl;

                        if (login_attempts >= MAX_LOGIN_ATTEMPTS) {
                            cout << COLOR_RED << "Maximum login attempts exceeded!" << COLOR_RESET << endl;
                            log_activity("Failed user login attempts exceeded");
                        }
                    }
                }
            }
            else if (choice == 4) {
                // Company client login
                cout << "\n========================================" << endl;
                cout << COLOR_YELLOW << "      COMPANY CLIENT LOGIN              " << endl << COLOR_RESET;
                cout << "========================================" << endl;

                char uid[50];
                char pwd[50];
                int login_attempts = 0;

                while (login_attempts < MAX_LOGIN_ATTEMPTS) {
                    cout << "\nEnter company user ID: ";
                    cin >> uid;
                    cin.ignore(10000, '\n');

                    cout << "Enter password: ";
                    cin >> pwd;
                    cin.ignore(10000, '\n');

                    company_client* company = find_company_by_credentials(uid, pwd);
                    if (company) {
                        if (!company->get_approved()) {
                            cout << COLOR_MAGENTA << "\nAccount pending approval!" << endl << COLOR_RESET;
                            break;
                        }
                        else if (company->get_frozen()) {
                            cout << COLOR_RED << "\nAccount is frozen!" << endl << COLOR_RESET;
                            break;
                        }
                        else {
                            cout << COLOR_GREEN << "\nLogin successful!" << endl << COLOR_RESET;
                            cout << COLOR_CYAN << "Welcome, " << company->get_company_name() << "!" << endl << COLOR_RESET;
                            log_activity("Company logged in", company->get_account_id());
                            company_client_menu(company);
                            break;
                        }
                    }
                    else {
                        // Check if user is employee
                        user_client* user = find_user_by_credentials(uid, pwd);
                        if (user) {
                            cout << COLOR_GREEN << "\nLogin successful!" << endl << COLOR_RESET;
                            cout << COLOR_CYAN << "Welcome, " << user->get_name() << "!" << endl << COLOR_RESET;
                            cout << "\nSelect account type:" << endl;
                            cout << "1. Personal Account" << endl;
                            cout << "2. Company Account (if employee)" << endl;
                            cout << "Choice: ";

                            int acc_type;
                            if (!get_int_input(acc_type, 1, 2)) {
                                break;
                            }

                            if (acc_type == 1) {
                                if (!user->get_approved()) {
                                    cout << COLOR_MAGENTA << "\nAccount pending approval!" << endl << COLOR_RESET;
                                }
                                else if (user->get_frozen()) {
                                    cout << COLOR_RED << "\nAccount is frozen!" << endl << COLOR_RESET;
                                }
                                else {
                                    log_activity("User logged in", user->get_account_id());
                                    user_client_menu(user);
                                }
                            }
                            else if (acc_type == 2) {
                                // Find company where user is employee
                                bool found_company = false;
                                for (int i = 0; i < company_count; i++) {
                                    if (companies[i]->is_employee(user->get_account_id())) {
                                        if (!companies[i]->get_approved()) {
                                            cout << COLOR_MAGENTA << "\nCompany account pending approval!" << endl << COLOR_RESET;
                                        }
                                        else if (companies[i]->get_frozen()) {
                                            cout << COLOR_RED << "\nCompany account is frozen!" << endl << COLOR_RESET;
                                        }
                                        else {
                                            cout << COLOR_CYAN << "\nAccessing: " << companies[i]->get_company_name() << endl << COLOR_RESET;
                                            log_activity("Employee accessing company account", companies[i]->get_account_id());
                                            company_client_menu(companies[i]);
                                        }
                                        found_company = true;
                                        break;
                                    }
                                }

                                if (!found_company) {
                                    cout << COLOR_YELLOW << "\nYou are not an employee of any company!" << endl << COLOR_RESET;
                                }
                            }
                            break;
                        }
                        else {
                            login_attempts++;
                            cout << COLOR_RED << "\nInvalid credentials! Attempts remaining: "
                                << (MAX_LOGIN_ATTEMPTS - login_attempts) << COLOR_RESET << endl;

                            if (login_attempts >= MAX_LOGIN_ATTEMPTS) {
                                cout << COLOR_RED << "Maximum login attempts exceeded!" << COLOR_RESET << endl;
                                log_activity("Failed company login attempts exceeded");
                            }
                        }
                    }
                }
            }
            else if (choice == 5) {
                banking_employee_menu();
            }
            else if (choice == 6) {
                cout << "\n========================================" << endl;
                cout << COLOR_GREEN << "  Thank you for using ABC Bank!         " << endl << COLOR_RESET;
                cout << COLOR_GREEN << "  Have a great day!                     " << endl << COLOR_RESET;
                cout << "========================================" << endl;
                log_activity("System shutdown");
                break;
            }
        }
    }
};

// MAIN FUNCTION 
int main() {
    // Seed random number generator for card generation
    srand(time(0));

    cout << COLOR_BLUE << "========================================" << endl << COLOR_RESET;
    cout << COLOR_BLUE << "                                        " << endl << COLOR_RESET;
    cout << COLOR_GREEN << COLOR_BOLD << "     WELCOME TO ABC BANK SYSTEM         " << endl << COLOR_RESET;
    cout << COLOR_BLUE << "                                        " << endl << COLOR_RESET;
    cout << COLOR_BLUE << "========================================" << endl << COLOR_RESET;

    banking_system system;

    cout << COLOR_GREEN << "System initialized successfully!" << endl << COLOR_RESET;
    cout << COLOR_YELLOW << "Press Enter to continue..." << COLOR_RESET << endl;
    cin.get();

    // Run the main banking system
    system.run();

    return 0;
}