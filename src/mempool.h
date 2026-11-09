#ifndef NAKACORE_MEMPOOL_H
#define NAKACORE_MEMPOOL_H

#include "transaction.h"
#include <vector>
#include <mutex>
#include <iostream>

namespace NakaCore {

class Mempool {
private:
    std::vector<Transaction> pool;
    std::mutex mut;

public:
    bool AddTransaction(const Transaction& tx) {
        std::lock_guard<std::mutex> lock(mut);
        pool.push_back(tx);
        std::cout << "[+] Transaction added to Mempool (Pending confirmation).\n";
        return true;
    }

    std::vector<Transaction> GetTransactionsForBlock() {
        std::lock_guard<std::mutex> lock(mut);
        std::vector<Transaction> txs = pool;
        pool.clear(); // Vaciar el mempool al empaquetarlas en un bloque
        return txs;
    }

    size_t GetSize() const {
        return pool.size();
    }
};

} // namespace NakaCore

#endif // NAKACORE_MEMPOOL_H
