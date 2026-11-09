#ifndef UTXO_H
#define UTXO_H

#include <string>
#include <vector>
#include <map>

const long long COIN = 100000000LL; // 1 NKC = 100,000,000 Nakas

struct TransactionOutput {
    std::string address;
    long long amount; // Cantidad exacta en Nakas
    int index;
};

class UTXOSet {
private:
    std::map<std::string, std::vector<TransactionOutput>> utxos;

public:
    void addUTXO(const std::string& address, long long amount, int index);
    long long getBalance(const std::string& address) const;
    void printUTXOs() const;
};

#endif
