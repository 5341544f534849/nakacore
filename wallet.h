#ifndef WALLET_H
#define WALLET_H

#include <string>

class Wallet {
private:
    std::string walletFile;
    void saveToFile() const;

public:
    std::string privateKey;
    std::string publicKey;
    std::string address;

    Wallet(std::string filename = "wallet.dat");
    std::string sign(const std::string& data) const;
    static bool verify(const std::string& pubKey, const std::string& data, const std::string& signature);
};

#endif
