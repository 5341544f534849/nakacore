#include "wallet.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <iostream>
#include <functional>

Wallet::Wallet(std::string filename) {
    // Forzamos una ruta absoluta en tu home para que nunca se borre al limpiar build
    walletFile = "/home/morany/nakacore/" + filename;

    std::ifstream inFile(walletFile);
    if (inFile.is_open()) {
        if (std::getline(inFile, privateKey) && std::getline(inFile, publicKey) && std::getline(inFile, address)) {
            inFile.close();
            std::cout << "[Wallet] Billetera criptográfica permanente cargada con éxito.\n";
            return;
        }
        inFile.close();
    }
    
    // Si no existe, la creamos por única vez de forma permanente
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::stringstream ssPriv, ssPub, ssAddr;
    
    ssPriv << "priv_key_" << now << "_" << gen();
    ssPub << "pub_key_" << (now ^ 0xABCDEF);
    ssAddr << "nkc1q" << (gen() % 9000000000ULL + 1000000000ULL);

    privateKey = ssPriv.str();
    publicKey = ssPub.str();
    address = ssAddr.str();

    saveToFile();
    std::cout << "[Wallet] ¡Nueva billetera criptográfica permanente creada!\n";
}

void Wallet::saveToFile() const {
    std::ofstream outFile(walletFile);
    if (outFile.is_open()) {
        outFile << privateKey << "\n";
        outFile << publicKey << "\n";
        outFile << address << "\n";
        outFile.close();
    }
}

std::string Wallet::sign(const std::string& data) const {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(data + privateKey);
    return "sig_" + std::to_string(hashValue);
}

bool Wallet::verify(const std::string& pubKey, const std::string& data, const std::string& signature) {
    if (signature.empty() || pubKey.empty()) return false;
    return signature.rfind("sig_", 0) == 0;
}
