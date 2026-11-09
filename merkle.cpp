#include "merkle.h"
#include <functional>

std::string MerkleTree::calculateRoot(const std::vector<Transaction>& transactions) {
    if (transactions.empty()) {
        return "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"; // Hash vacío (SHA-256 placeholder)
    }

    std::vector<std::string> layer;
    for (const auto& tx : transactions) {
        std::string txData = tx.senderAddress + tx.receiverAddress + std::to_string(tx.amount) + tx.signature;
        std::hash<std::string> hasher;
        layer.push_back(std::to_string(hasher(txData)));
    }

    while (layer.size() > 1) {
        std::vector<std::string> nextLayer;
        for (size_t i = 0; i < layer.size(); i += 2) {
            std::string combined;
            if (i + 1 < layer.size()) {
                combined = layer[i] + layer[i + 1];
            } else {
                combined = layer[i] + layer[i]; // Duplicar si es impar
            }
            std::hash<std::string> hasher;
            nextLayer.push_back(std::to_string(hasher(combined)));
        }
        layer = nextLayer;
    }

    return layer[0];
}
