#include <iostream>
#include "nakacore.h"
#include "blockchain.h"
#include "wallet.h"
#include "utxo.h"
#include "mempool.h"

int main() {
    std::cout << "=== NAKA CORE ($NKC) - MERKLE TREES & CRIPTOGRAFIA ===\n";
    std::cout << "Max Supply: 20000000 NKC | Arboles de Merkle Activos\n\n";

    Wallet minerWallet;
    std::cout << " > Direccion del minero   -> " << minerWallet.address << "\n";

    Blockchain nkcChain("chain_data.dat");
    UTXOSet utxoSet;
    Mempool mempool;

    long long blockReward = 50 * COIN;
    for (const auto& block : nkcChain.getChain()) {
        utxoSet.addUTXO(minerWallet.address, blockReward, block.index);
    }

    std::cout << " > Balance del minero: " << (double)utxoSet.getBalance(minerWallet.address) / COIN << " NKC\n\n";

    // Creamos una transacción firmada
    std::string receiver = "nkc1q_destino_seguro";
    long long amount = 15 * COIN;
    long long fee = 1 * COIN;
    
    std::string txData = minerWallet.address + receiver + std::to_string(amount);
    std::string signature = minerWallet.sign(txData);

    Transaction tx = {minerWallet.address, receiver, amount, fee, minerWallet.publicKey, signature};

    std::cout << "[+] Añadiendo transacción firmada a la Mempool...\n";
    mempool.addTransaction(tx);

    // Minamos el bloque pasando las transacciones de la Mempool para calcular la Merkle Root
    std::cout << "[+] Minando bloque empaquetando transacciones con Árbol de Merkle...\n";
    nkcChain.addBlock("Bloque con transacciones aseguradas por Merkle", mempool.getPendingTransactions());
    mempool.clear();

    auto chain = nkcChain.getChain();
    if (!chain.empty()) {
        utxoSet.addUTXO(minerWallet.address, blockReward, chain.back().index);
    }

    nkcChain.printChain();
    utxoSet.printUTXOs();

    std::cout << "\n[OK] ¡Árboles de Merkle integrados y ejecutados con éxito!\n";
    return 0;
}
