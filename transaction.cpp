#include "transaction.h"
#include <functional>
#include <sstream>

Transaction::Transaction(std::string s, std::string r, double amt, std::string privKey) {
    sender = s;
    recipient = r;
    amount = amt;
    
    // Simulación de firma digital basada en la clave privada del emisor
    std::hash<std::string> hasher;
    unsigned long hashValue = hasher(sender + recipient + std::to_string(amount) + privKey);
    std::stringstream ss;
    ss << "sig_" << std::hex << hashValue;
    signature = ss.str();
}

std::string Transaction::getTransactionHash() const {
    std::hash<std::string> hasher;
    std::string rawData = sender + recipient + std::to_string(amount) + signature;
    return std::to_string(hasher(rawData));
}

bool Transaction::isValid() const {
    // Verificación básica de que la firma no esté vacía y los montos sean válidos
    if (amount <= 0) return false;
    if (signature.rfind("sig_", 0) != 0) return false;
    return true;
}
