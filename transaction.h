#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
public:
    std::string sender;
    std::string recipient;
    double amount;
    std::string signature;

    Transaction(std::string s, std::string r, double amt, std::string privKey);
    bool isValid() const;
    std::string getTransactionHash() const;
};

#endif
