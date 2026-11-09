#ifndef NAKACORE_BLOCKCHAIN_H
#define NAKACORE_BLOCKCHAIN_H

#include "block.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace NakaCore {

class Blockchain {
private:
    std::vector<Block> chain;
    const std::string db_path = "./chain.dat";

    void SaveToDisk() {
        std::filesystem::create_directories("./data");
        std::ofstream file("./data/chain.dat", std::ios::binary);
        if (file.is_open()) {
            size_t size = chain.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (const auto& block : chain) {
                file.write(reinterpret_cast<const char*>(&block.header.version), sizeof(block.header.version));
                file.write(reinterpret_cast<const char*>(&block.header.timestamp), sizeof(block.header.timestamp));
                file.write(reinterpret_cast<const char*>(&block.header.nonce), sizeof(block.header.nonce));
            }
            file.close();
        }
    }

    void LoadFromDisk() {
        std::string path = "./data/chain.dat";
        if (!std::filesystem::exists(path)) return;
        std::ifstream file(path, std::ios::binary);
        if (file.is_open()) {
            size_t size = 0;
            if (!file.read(reinterpret_cast<char*>(&size), sizeof(size))) return;
            // Protección contra basura o archivos corruptos (bad_alloc)
            if (size > 1000000) return; 
            
            chain.clear();
            for (size_t i = 0; i < size; ++i) {
                Block block;
                if (!file.read(reinterpret_cast<char*>(&block.header.version), sizeof(block.header.version))) break;
                if (!file.read(reinterpret_cast<char*>(&block.header.timestamp), sizeof(block.header.timestamp))) break;
                if (!file.read(reinterpret_cast<char*>(&block.header.nonce), sizeof(block.header.nonce))) break;
                chain.push_back(block);
            }
            file.close();
        }
    }

public:
    Blockchain() {
        LoadFromDisk();
        if (chain.empty()) {
            Block genesis;
            genesis.header.version = 1;
            genesis.header.timestamp = 1710000000;
            genesis.header.nonce = 0;
            chain.push_back(genesis);
            SaveToDisk();
        }
    }

    void AddBlock(const Block& block) {
        chain.push_back(block);
        SaveToDisk();
        std::cout << "[+] Block successfully added and persisted to disk.\n";
    }

    int GetHeight() const {
        return static_cast<int>(chain.size()) - 1;
    }
};

} // namespace NakaCore

#endif // NAKACORE_BLOCKCHAIN_H
