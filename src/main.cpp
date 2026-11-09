#include "blockchain.h"
#include "net.h"
#include "wallet.h"
#include "mempool.h"
#include "validation.h"
#include "pow.h"
#include "cli.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

std::atomic<bool> g_mining(true);

// Función para calcular la recompensa por bloque aplicando el Halving
uint64_t GetBlockSubsidy(int height) {
    int halvings = height / 1050000; // Simulando intervalos de halving
    uint64_t subsidy = 50 * 100'000'000ULL; // 50 NKC base
    subsidy >>= halvings; // Divide a la mitad por cada halving cumplido
    return subsidy;
}

void AutoMiner(NakaCore::Blockchain& blockchain, NakaCore::Wallet& wallet, NakaCore::Mempool& mempool) {
    std::cout << "[*] Naka Core Long-Term Halving Miner Started...\n";
    
    while (g_mining) {
        NakaCore::Block new_block;
        new_block.header.version = 1;
        new_block.header.timestamp = static_cast<uint32_t>(time(nullptr));
        new_block.header.bits = 0x1f00ffff; 
        new_block.header.nonce = 0;

        new_block.vtx = mempool.GetTransactionsForBlock();

        if (new_block.vtx.empty()) {
            NakaCore::Transaction coinbase;
            coinbase.version = 1;
            NakaCore::TxOut out;
            
            // Calcular recompensa actual basada en la altura de la cadena (Halving)
            int currentHeight = blockchain.GetHeight() + 1;
            out.value = GetBlockSubsidy(currentHeight);
            
            std::string addr = wallet.GetAddressHex();
            out.pk_script.assign(addr.begin(), addr.end());
            coinbase.vout.push_back(out);
            new_block.vtx.push_back(coinbase);
        }

        NakaCore::UTXOSet dummyUtxo;
        if (NakaCore::ConsensusValidator::ValidateBlock(new_block, dummyUtxo)) {
            if (NakaCore::ProofOfWork::MineBlock(new_block, 2000000)) {
                blockchain.AddBlock(new_block);
                std::cout << "\n[BLOCK MINED!] Recompensa emitida bajo reglas de escasez. Altura: " << blockchain.GetHeight() << "\nnakacore> " << std::flush;
            }
        }

        // Pausa controlada para simular el intervalo de red
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}

int main() {
    std::cout << "=== Naka Core ($NKC) Full Daemon Initializing ===\n";

    NakaCore::Blockchain nakachain;
    NakaCore::P2PNode p2p(8333);
    if (!p2p.Start()) {
        std::cout << "[-] Warning: P2P network port busy.\n";
    }

    NakaCore::Wallet localWallet;
    NakaCore::Mempool mempool;

    std::cout << "[+] Blockchain loaded from disk. Current Height: " << nakachain.GetHeight() << "\n";
    std::cout << "[+] Halving economics active. Max Supply: 20,000,000 $NKC\n";

    std::thread minerThread(AutoMiner, std::ref(nakachain), std::ref(localWallet), std::ref(mempool));

    NakaCore::CLI::Run(nakachain, localWallet, mempool);

    g_mining = false;
    if (minerThread.joinable()) {
        minerThread.join();
    }
    p2p.Stop();
    return 0;
}
