#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include "ATM.h"
#include <windows.h> // For Sleep on Windows

using namespace std;

// Helper to safely read integers without crashing
int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            break;
        } else {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    return value;
}

long int readLongInt(const string& prompt) {
    long int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            break;
        } else {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    return value;
}

double readDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            break;
        } else {
            cout << "Invalid input. Please enter a valid number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    return value;
}

void loadUsers(vector<unique_ptr<Account>>& users, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string type;
    long int accNo;
    string name, mobile, pinHash;
    double balance;

    while (file >> type >> accNo >> name >> pinHash >> balance >> mobile) {
        if (type == "S") {
            auto acc = make_unique<SavingsAccount>();
            acc->setData(accNo, name, pinHash, balance, mobile);
            users.push_back(move(acc));
        } else if (type == "C") {
            auto acc = make_unique<CurrentAccount>();
            acc->setData(accNo, name, pinHash, balance, mobile);
            users.push_back(move(acc));
        }
    }
    file.close();
}

void saveUsers(const vector<unique_ptr<Account>>& users, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& user : users) {
        string typeCode = (user->getAccountType() == "Savings") ? "S" : "C";
        file << typeCode << " "
             << user->getAccountNo() << " "
             << user->getName() << " "
             << user->getPinHash() << " "
             << user->getBalance() << " "
             << user->getMobileNo() << "\n";
    }
    file.close();
}

Account* findAccount(const vector<unique_ptr<Account>>& users, long int accNo) {
    for (const auto& user : users) {
        if (user->getAccountNo() == accNo) {
            return user.get();
        }
    }
    return nullptr;
}

int main() {
    vector<unique_ptr<Account>> users;
    string dbFile = "users.txt";
    
    // Load existing users or add defaults
    loadUsers(users, dbFile);
    if (users.empty()) {
        auto u1 = make_unique<SavingsAccount>();
        u1->setData(987654321, "Hardik", Account::hashPIN(1234), 40000, "9986472841");
        auto u2 = make_unique<CurrentAccount>();
        u2->setData(111122223, "Alice", Account::hashPIN(5678), 10000, "9876543210");
        users.push_back(move(u1));
        users.push_back(move(u2));
        saveUsers(users, dbFile);
    }

    while (true) {
        system("cls"); // Clear screen for Windows

        cout << endl << "**** Advanced Secure ATM *****" << endl;
        long int enterAccountNo = readLongInt("\nEnter Your Account No (or 0 to exit): ");
        if (enterAccountNo == 0) {
            cout << "Exiting application." << endl;
            break;
        }
        
        Account* currentUser = findAccount(users, enterAccountNo);

        if (currentUser != nullptr) {
            int enterPIN = readInt("Enter PIN: ");
            string hashedInput = Account::hashPIN(enterPIN);

            if (currentUser->getPinHash() == hashedInput) {
                currentUser->resetPinAttempts();

                bool loggedIn = true;
                while (loggedIn) {
                    if (currentUser->getLockStatus()) {
                        cout << "Account is locked. Please contact support." << endl;
                        system("pause");
                        break;
                    }

                    system("cls");
                    cout << endl << "**** Welcome " << currentUser->getName() << " (" << currentUser->getAccountType() << ") *****" << endl;
                    cout << endl << "1. Check Balance";
                    cout << endl << "2. Cash Withdraw";
                    cout << endl << "3. Deposit Cash";
                    cout << endl << "4. Fund Transfer";
                    cout << endl << "5. Show User Details";
                    cout << endl << "6. Update Mobile No.";
                    cout << endl << "7. View Transaction History";
                    cout << endl << "8. Logout" << endl;
                    
                    int choice = readInt("\nOption: ");

                    switch (choice) {
                        case 1:
                            cout << endl << "Your balance is: " << currentUser->getBalance() << endl;
                            system("pause");
                            break;
                        case 2: {
                            double amount = readDouble("\nEnter withdrawal amount: ");
                            currentUser->cashWithDraw(amount);
                            saveUsers(users, dbFile); 
                            system("pause");
                            break;
                        }
                        case 3: {
                            double amount = readDouble("\nEnter deposit amount: ");
                            currentUser->deposit(amount);
                            saveUsers(users, dbFile); 
                            system("pause");
                            break;
                        }
                        case 4: {
                            long int destAccNo = readLongInt("\nEnter Destination Account No: ");
                            Account* destUser = findAccount(users, destAccNo);
                            
                            if (destUser == nullptr) {
                                cout << "Destination account not found." << endl;
                            } else if (destUser == currentUser) {
                                cout << "Cannot transfer to yourself!" << endl;
                            } else {
                                double amount = readDouble("Enter transfer amount: ");
                                double initialBalance = currentUser->getBalance();
                                currentUser->cashWithDraw(amount); 
                                
                                if (currentUser->getBalance() < initialBalance) {
                                    destUser->deposit(amount);
                                    currentUser->logTransaction("Transfer: -" + to_string(amount) + " to Acc " + to_string(destAccNo));
                                    destUser->logTransaction("Transfer: +" + to_string(amount) + " from Acc " + to_string(currentUser->getAccountNo()));
                                    cout << "Transfer successful!" << endl;
                                    saveUsers(users, dbFile);
                                } else {
                                    cout << "Transfer failed." << endl;
                                }
                            }
                            system("pause");
                            break;
                        }
                        case 5:
                            cout << endl << "*** User Details ***";
                            cout << endl << "Account No: " << currentUser->getAccountNo();
                            cout << endl << "Name: " << currentUser->getName();
                            cout << endl << "Type: " << currentUser->getAccountType();
                            cout << endl << "Balance: " << currentUser->getBalance();
                            cout << endl << "Mobile No.: " << currentUser->getMobileNo() << endl;
                            system("pause");
                            break;
                        case 6: {
                            string oldMobileNo, newMobileNo;
                            cout << "\nEnter Old Mobile No.: ";
                            cin >> oldMobileNo;
                            cout << "Enter New Mobile No.: ";
                            cin >> newMobileNo;
                            currentUser->setMobile(oldMobileNo, newMobileNo);
                            saveUsers(users, dbFile); 
                            system("pause");
                            break;
                        }
                        case 7: {
                            vector<string> transactions = currentUser->getTransactionHistory();
                            cout << endl << "*** Transaction History ***";
                            if (transactions.empty()) {
                                cout << "\nNo transactions yet in this session.";
                            } else {
                                for (const string& transaction : transactions) {
                                    cout << endl << transaction;
                                }
                            }
                            cout << endl;
                            system("pause");
                            break;
                        }
                        case 8:
                            saveUsers(users, dbFile);
                            cout << "Logging out..." << endl;
                            loggedIn = false;
                            break;
                        default:
                            cout << endl << "Invalid Option!" << endl;
                            system("pause");
                            break;
                    }
                }
            } else {
                cout << endl << "Incorrect PIN!" << endl;
                currentUser->trackPinAttempts();
                if (currentUser->getLockStatus()) {
                    cout << "Too many attempts. Locking account for 30 seconds..." << endl;
                    Sleep(30000);
                    currentUser->unlockAccount();
                }
                system("pause");
            }
        } else {
            cout << endl << "Account not found!" << endl;
            system("pause");
        }
    }
    return 0;
}
