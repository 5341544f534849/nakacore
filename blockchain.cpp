#include "blockchain.h"
#include "merkle.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <functional>
#include <fstream>

Blockchain::Blockchain(std::string filename) : chainFile(filename), currentDifficulty(1) {
    loadFromFile();
    if (!chain.empty()) {
        currentDifficulty = chain.back().difficulty;
    }

    if (chain.empty()) {
        std::cout << "[+] Creando Bloque Genesis...\n";
        std::vector<Transaction> genesisTxs;
        std::string genesisMerkle = MerkleTree::calculateRoot(genesisTxs);
        
        Block genesisBlock;
        genesisBlock.index = 0;
        genesisBlock.previousHash = "0";
        genesisBlock.merkleRoot = genesisMerkle;
        genesisBlock.data = "Bloque Genesis - Naka Core ($NKC)";
        genesisBlock.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        genesisBlock.nonce = 0;
        genesisBlock.difficulty = currentDifficulty;
        genesisBlock.hash = calculateHash(genesisBlock.index, genesisBlock.previousHash, genesisBlock.merkleRoot, genesisBlock.data, genesisBlock.timestamp, genesisBlock.nonce);
        
        chain.push_back(genesisBlock);
        saveToFile();
    }
}

std::string Blockchain::calculateHash(int index, const std::string& previousHash, const std::string& merkleRoot, const std::string& data, long long timestamp, int nonce) const {
    std::stringstream ss;
    ss << index << previousHash << merkleRoot << data << timestamp << nonce;
    std::hash<std::string> hasher;
    return std::to_string(hasher(ss.str()));
}

void Blockchain::adjustDifficulty() {
    if (chain.size() % RETARGET_INTERVAL == 0 && chain.size() > 0) {
        Block prevBlock = chain[chain.size() - RETARGET_INTERVAL];
        Block latestBlock = chain.back();

        long long actualTimeSpan = latestBlock.timestamp - prevBlock.timestamp;
        long long expectedTimeSpan = BLOCK_INTERVAL * RETARGET_INTERVAL;

        std::cout << "[Consenso] Evaluando ajuste de dificultad...\n";
        std::cout << " > Tiempo real transcurrido: " << actualTimeSpan << "s (Esperado: " << expectedTimeSpan << "s)\n";

        if (actualTimeSpan < expectedTimeSpan / 2) {
            currentDifficulty++;
            std::cout << " > ¡La red va muy rápido! Subiendo dificultad a: " << currentDifficulty << "\n";
        } else if (actualTimeSpan > expectedTimeSpan * 2 && currentDifficulty > 1) {
            currentDifficulty--;
            std::cout << " > La red va lenta. Bajando dificultad a: " << currentDifficulty << "\n";
        } else {
            std::cout << " > Ritmo óptimo. Manteniendo dificultad en: " << currentDifficulty << "\n";
        }
    }
}

void Blockchain::addBlock(const std::string& data, const std::vector<Transaction>& transactions) {
    Block newBlock;
    Block latestBlock = chain.back();

    newBlock.index = latestBlock.index + 1;
    newBlock.previousHash = latestBlock.hash;
    newBlock.merkleRoot = MerkleTree::calculateRoot(transactions);
    newBlock.data = data;
    newBlock.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    newBlock.nonce = 0;
    
    // Asignamos la dificultad actual antes de minar
    newBlock.difficulty = currentDifficulty;

    std::string target(newBlock.difficulty, '0');

    while (true) {
        newBlock.hash = calculateHash(newBlock.index, newBlock.previousHash, newBlock.merkleRoot, newBlock.data, newBlock.timestamp, newBlock.nonce);
        if (newBlock.hash.substr(0, newBlock.difficulty) == target || newBlock.nonce > 200000) {
            break;
        }
        newBlock.nonce++;
    }

    chain.push_back(newBlock);
    
    // Verificamos si toca ajustar la dificultad para los siguientes bloques
    adjustDifficulty();

    saveToFile();
    std::cout << "[+] Minado bloque #" << newBlock.index << " con dificultad " << newBlock.difficulty << ".\n";
}

std::vector<Block> Blockchain::getChain() const {
    return chain;
}

void Blockchain::printChain() const {
    std::cout << "\n=== ESTADO ACTUAL DE LA BLOCKCHAIN ($NKC) ===\n";
    for (const auto& block : chain) {
        std::cout << "------------------------------------------\n";
        std::cout << "Bloque Index    : " << block.index << "\n";
        std::cout << "Hash            : " << block.hash << "\n";
        std::cout << "Hash Anterior   : " << block.previousHash << "\n";
        std::cout << "Merkle Root     : " << block.merkleRoot << "\n";
        std::cout << "Dificultad      : " << block.difficulty << "\n";
        std::cout << "Nonce           : " << block.nonce << "\n";
    }
    std::cout << "------------------------------------------\n";
}

void Blockchain::saveToFile() const {
    std::ofstream outFile(chainFile);
    if (outFile.is_open()) {
        outFile << chain.size() << "\n";
        for (const auto& block : chain) {
            outFile << block.index << "\n"
                    << block.previousHash << "\n"
                    << block.hash << "\n"
                    << block.merkleRoot << "\n"
                    << block.data << "\n"
                    << block.timestamp << "\n"
                    << block.nonce << "\n"
                    << block.difficulty << "\n";
        }
        outFile.close();
    }
}

void Blockchain::loadFromFile() {
    std::ifstream inFile(chainFile);
    if (!inFile.is_open()) return;

    size_t size;
    if (!(inFile >> size)) return;
    chain.clear();

    for (size_t i = 0; i < size; ++i) {
        Block block;
        inFile >> block.index >> block.previousHash >> block.hash >> block.merkleRoot;
        inFile.ignore();
        std::getline(inFile, block.data);
        inFile >> block.timestamp >> block.nonce >> block.difficulty;
        chain.push_back(block);
    }
    inFile.close();
}
