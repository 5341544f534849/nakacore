#include "utxo.h"
#include <iostream>
#include <iomanip>

void UTXOSet::addUTXO(const std::string& address, long long amount, int index) {
    utxos[address].push_back({address, amount, index});
}

long long UTXOSet::getBalance(const std::string& address) const {
    long long balance = 0;
    auto it = utxos.find(address);
    if (it != utxos.end()) {
        for (const auto& utxo : it->second) {
            balance += utxo.amount;
        }
    }
    return balance;
}

void UTXOSet::printUTXOs() const {
    std::cout << "\n==================== ESTADO DE UTXOS / SALDOS ====================\n";
    for (const auto& pair : utxos) {
        long long totalNakas = 0;
        for (const auto& u : pair.second) totalNakas += u.amount;
        
        double totalNKC = (double)totalNakas / COIN;
        std::cout << "Direccion : " << pair.first << "\n";
        std::cout << "Balance   : " << totalNKC << " NKC (" << totalNakas << " Nakas)\n";
        std::cout << "------------------------------------------------------------------\n";
    }
}
