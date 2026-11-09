#ifndef NAKACORE_STORAGE_H
#define NAKACORE_STORAGE_H

#include "block.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

namespace NakaCore {

class Storage {
private:
    std::string datadir;

public:
    Storage(std::string dir) : datadir(dir) {}

    // Serializar y guardar la cadena completa en disco (formato binario plano)
    bool SaveChain(const std::vector<Block>& chain) {
        std::string filepath = datadir + "/chain.dat";
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "[-] Error: Could not open file for writing: " << filepath << "\n";
            return false;
        }

        uint32_t chain_size = static_cast<uint32_t>(chain.size());
        file.write(reinterpret_cast<const char*>(&chain_size), sizeof(chain_size));

        for (const auto& block : chain) {
            // Guardar cabecera
            file.write(reinterpret_cast<const char*>(&block.header), sizeof(BlockHeader));
            
            // Guardar número de transacciones
            uint32_t tx_count = static_cast<uint32_t>(block.vtx.size());
            file.write(reinterpret_cast<const char*>(&tx_count), sizeof(tx_count));

            for (const auto& tx : block.vtx) {
                file.write(reinterpret_cast<const char*>(&tx.version), sizeof(tx.version));
                file.write(reinterpret_cast<const char*>(&tx.lock_time), sizeof(tx.lock_time));

                uint32_t vout_count = static_cast<uint32_t>(tx.vout.size());
                file.write(reinterpret_cast<const char*>(&vout_count), sizeof(vout_count));

                for (const auto& out : tx.vout) {
                    file.write(reinterpret_cast<const char*>(&out.value), sizeof(out.value));
                    uint32_t script_size = static_cast<uint32_t>(out.pk_script.size());
                    file.write(reinterpret_cast<const char*>(&script_size), sizeof(script_size));
                    if (script_size > 0) {
                        file.write(reinterpret_cast<const char*>(out.pk_script.data()), script_size);
                    }
                }
            }
        }

        file.close();
        std::cout << "[+] Blockchain successfully persisted to disk (" << filepath << ").\n";
        return true;
    }
};

} // namespace NakaCore

#endif // NAKACORE_STORAGE_H
