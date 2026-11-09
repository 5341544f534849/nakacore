#ifndef NAKACORE_BLOCKCHAIN_H
#define NAKACORE_BLOCKCHAIN_H

#include "block.h"
#include "utxo.h"
#include "pow.h"
#include "storage.h"
#include <vector>
#include <iostream>
#include <filesystem>

namespace NakaCore {

class Blockchain {
private:
    std::vector<Block> chain;
    UTXOSet utxoSet;
    Storage storage;

public:
    Blockchain(std::string data_path = "./data") : storage(data_path) {
        // Crear directorio de datos si no existe
        std::filesystem::create_directories(data_path);

        // Inicializar con el bloque génesis por defecto
        Block genesis = CreateGenesisBlock();
        chain.push_back(genesis);
        
        // Registrar la recompensa de coinbase del bloque génesis en el UTXO
        OutPoint genesis_op;
        std::fill(genesis_op.txid.begin(), genesis_op.txid.end(), 0x00);
        genesis_op.index = 0;
        utxoSet.AddUTXO(genesis_op, genesis.vtx[0].vout[0], true, 0);

        // Guardar estado inicial
        storage.SaveChain(chain);
    }

    bool AddBlock(Block new_block) {
        // Verificar la prueba de trabajo antes de aceptar el bloque
        if (!ProofOfWork::CheckProofOfWork(new_block.header)) {
            std::cout << "[-] Error: Invalid Proof-of-Work for new block.\n";
            return false;
        }

        chain.push_back(new_block);
        std::cout << "[+] Block successfully added to chain at height " << chain.size() - 1 << "\n";

        // Persistir cambios en disco automáticamente
        storage.SaveChain(chain);
        return true;
    }

    size_t GetHeight() const {
        return chain.size() - 1;
    }
};

} // namespace NakaCore

#endif // NAKACORE_BLOCKCHAIN_H
