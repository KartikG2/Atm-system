#include "ATM.h"
#include <sstream>

using namespace std;

// -- Account Base Class --
Account::Account(string type) : accountType(type), isLocked(false), pinAttempts(0) {}

void Account::setData(long int account_No_a, string name_a, string pinHash_a, double balance_a, string mobile_No_a) {
    account_No = account_No_a;
    name = name_a;
    pinHash = pinHash_a;
    balance = balance_a;
    mobile_No = mobile_No_a;
}

long int Account::getAccountNo() const { return account_No; }
string Account::getName() const { return name; }
string Account::getPinHash() const { return pinHash; }
double Account::getBalance() const { return balance; }
string Account::getMobileNo() const { return mobile_No; }
string Account::getAccountType() const { return accountType; }
vector<string> Account::getTransactionHistory() const { return transactionHistory; }
bool Account::getLockStatus() const { return isLocked; }

void Account::logTransaction(string transaction) {
    transactionHistory.push_back(transaction);
}

void Account::setMobile(string mob_prev, string mob_new) {
    if (mob_prev == mobile_No) {
        mobile_No = mob_new;
        cout << endl << "Successfully Updated Mobile no." << endl;
    } else {
        cout << endl << "Incorrect Old Mobile no!" << endl;
    }
}

void Account::lockAccount() {
    isLocked = true;
    cout << endl << "Your account has been locked due to multiple incorrect attempts." << endl;
}

void Account::trackPinAttempts() {
    pinAttempts++;
    if (pinAttempts >= 3) {
        lockAccount();
    }
}

void Account::resetPinAttempts() {
    pinAttempts = 0;
}

void Account::unlockAccount() {
    isLocked = false;
    pinAttempts = 0;
    cout << endl << "Your account has been unlocked." << endl;
}

string Account::hashPIN(int pin) {
    // Custom hash formatted to look like bcrypt for the project
    unsigned long hash = 5381;
    string strPin = to_string(pin);
    for (char c : strPin) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    stringstream stream;
    stream << "$2a$12$" << hex << hash << "abcdef1234567890abcdef123456"; 
    // Mocks standard bcrypt length and prefix
    return stream.str();
}

// -- Savings Account --
SavingsAccount::SavingsAccount() : Account("Savings") {}

void SavingsAccount::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        logTransaction("Deposit: +" + to_string(amount) + " (Savings)");
        cout << "Successfully deposited. New Balance: " << balance << endl;
    } else {
        cout << "Invalid deposit amount." << endl;
    }
}

void SavingsAccount::cashWithDraw(double amount) {
    if (amount > 0 && amount <= balance && amount <= 20000) {
        balance -= amount;
        logTransaction("Withdraw: -" + to_string(amount) + " (Savings)");
        cout << "Please Collect Your Cash." << endl;
        cout << "Available Balance: " << balance << endl;
    } else if (amount > 20000) {
        cout << "Withdrawal limit for Savings is 20000." << endl;
    } else {
        cout << "Invalid Input or Insufficient Balance." << endl;
    }
}

// -- Current Account --
CurrentAccount::CurrentAccount() : Account("Current"), overdraftLimit(10000) {}

void CurrentAccount::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        logTransaction("Deposit: +" + to_string(amount) + " (Current)");
        cout << "Successfully deposited. New Balance: " << balance << endl;
    } else {
        cout << "Invalid deposit amount." << endl;
    }
}

void CurrentAccount::cashWithDraw(double amount) {
    if (amount > 0 && (balance - amount) >= -overdraftLimit) {
        balance -= amount;
        logTransaction("Withdraw: -" + to_string(amount) + " (Current)");
        cout << "Please Collect Your Cash." << endl;
        cout << "Available Balance: " << balance << endl;
    } else {
        cout << "Overdraft limit exceeded or Invalid amount." << endl;
    }
}
