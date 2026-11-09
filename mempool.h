#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <string>
#include <vector>

struct Transaction {
    std::string senderAddress;
    std::string receiverAddress;
    long long amount;
    long long fee;
    std::string publicKey;
    std::string signature;
};

class Mempool {
private:
    std::vector<Transaction> transactions;

public:
    bool addTransaction(const Transaction& tx);
    std::vector<Transaction> getPendingTransactions();
    void clear();
    size_t size() const;
};

#endif
