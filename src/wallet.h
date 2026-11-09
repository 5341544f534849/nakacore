#ifndef NAKACORE_WALLET_H
#define NAKACORE_WALLET_H

#include <string>
#include <vector>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>

namespace NakaCore {

class Wallet {
private:
    std::vector<uint8_t> private_key;
    std::vector<uint8_t> public_key;

public:
    Wallet() {
        // Generar clave privada aleatoria de 32 bytes (estándar cypherpunk)
        private_key.resize(32);
        RAND_bytes(private_key.data(), 32);

        // Derivar clave pública simple mediante SHA-256 de la privada
        public_key.resize(32);
        SHA256(private_key.data(), private_key.size(), public_key.data());
    }

    std::string GetAddressHex() const {
        std::stringstream ss;
        for (uint8_t b : public_key) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
        }
        return "nkc1" + ss.str().substr(0, 40); // Dirección nativa de Naka Core
    }
};

} // namespace NakaCore

#endif // NAKACORE_WALLET_H
