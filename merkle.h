#ifndef MERKLE_H
#define MERKLE_H

#include <vector>
#include <string>
#include "mempool.h"

class MerkleTree {
public:
    static std::string calculateRoot(const std::vector<Transaction>& transactions);
};

#endif
