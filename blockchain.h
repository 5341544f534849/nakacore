#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <string>
#include <vector>
#include "nakacore.h"
#include "mempool.h"

struct Block {
    int index;
    std::string previousHash;
    std::string hash;
    std::string merkleRoot;
    std::string data;
    long long timestamp;
    int nonce;
    int difficulty;
};

class Blockchain {
private:
    std::vector<Block> chain;
    std::string chainFile;
    int currentDifficulty;
    const int BLOCK_INTERVAL = 10; // Tiempo objetivo por bloque en segundos (ejemplo)
    const int RETARGET_INTERVAL = 5; // Cada 5 bloques ajustamos la dificultad

    std::string calculateHash(int index, const std::string& previousHash, const std::string& merkleRoot, const std::string& data, long long timestamp, int nonce) const;
    void adjustDifficulty();
    void saveToFile() const;
    void loadFromFile();

public:
    Blockchain(std::string filename = "chain_data.dat");
    void addBlock(const std::string& data, const std::vector<Transaction>& transactions);
    std::vector<Block> getChain() const;
    void printChain() const;
};

#endif
