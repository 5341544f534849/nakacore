#include "mempool.h"
#include "wallet.h"
#include <iostream>

bool Mempool::addTransaction(const Transaction& tx) {
    std::string txData = tx.senderAddress + tx.receiverAddress + std::to_string(tx.amount);
    
    // Validamos matemáticamente la firma digital antes de aceptar la transacción
    if (!Wallet::verify(tx.publicKey, txData, tx.signature)) {
        std::cout << "[Mempool] ERROR: Transacción rechazada por firma criptográfica inválida.\n";
        return false;
    }
    
    transactions.push_back(tx);
    std::cout << "[Mempool] Transacción verificada criptográficamente y añadida.\n";
    return true;
}

std::vector<Transaction> Mempool::getPendingTransactions() {
    return transactions;
}

void Mempool::clear() {
    transactions.clear();
}

size_t Mempool::size() const {
    return transactions.size();
}
