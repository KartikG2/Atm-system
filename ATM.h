#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>
#include <vector>

class Account {
protected:
    long int account_No;
    std::string name;
    std::string pinHash;
    double balance;
    std::string mobile_No;
    std::vector<std::string> transactionHistory;
    bool isLocked;
    int pinAttempts;
    std::string accountType;

public:
    Account(std::string type);
    virtual ~Account() = default;

    void setData(long int account_No_a, std::string name_a, std::string pinHash_a, double balance_a, std::string mobile_No_a);
    
    long int getAccountNo() const;
    std::string getName() const;
    std::string getPinHash() const;
    double getBalance() const;
    std::string getMobileNo() const;
    std::string getAccountType() const;
    std::vector<std::string> getTransactionHistory() const;
    bool getLockStatus() const;

    void setMobile(std::string mob_prev, std::string mob_new);
    void lockAccount();
    void trackPinAttempts();
    void resetPinAttempts();
    void unlockAccount();
    void logTransaction(std::string transaction);

    virtual void deposit(double amount) = 0;
    virtual void cashWithDraw(double amount) = 0;
    
    // Hash utility
    static std::string hashPIN(int pin);
};

class SavingsAccount : public Account {
public:
    SavingsAccount();
    void deposit(double amount) override;
    void cashWithDraw(double amount) override;
};

class CurrentAccount : public Account {
private:
    double overdraftLimit;
public:
    CurrentAccount();
    void deposit(double amount) override;
    void cashWithDraw(double amount) override;
};

#endif // ATM_H
