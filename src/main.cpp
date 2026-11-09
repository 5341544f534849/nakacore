#include "blockchain.h"
#include "net.h"
#include "wallet.h"
#include "transaction.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Naka Core ($NKC) Daemon Initializing ===\n";

    // 1. Iniciar servidor P2P en el puerto 8333
    NakaCore::P2PNode p2p(8333);
    if (!p2p.Start()) {
        std::cout << "[-] Failed to start P2P network layer.\n";
        return 1;
    }

    // 2. Inicializar Blockchain, Billetera y Estado
    NakaCore::Blockchain nakachain;
    std::cout << "[+] Blockchain active. Current Height: " << nakachain.GetHeight() << "\n";

    NakaCore::Wallet minerWallet;
    std::cout << "[+] Generated Miner Wallet Address: " << minerWallet.GetAddressHex() << "\n";

    // 3. Simular la creación de una transacción de transferencia
    NakaCore::OutPoint dummy_outpoint;
    std::fill(dummy_outpoint.txid.begin(), dummy_outpoint.txid.end(), 0x01);
    dummy_outpoint.index = 0;

    NakaCore::Transaction transferTx = NakaCore::TransactionBuilder::CreateTransaction(
        minerWallet, "nkc1recipientaddressdemo999999", 25 * 100'000'000ULL, dummy_outpoint, 50 * 100'000'000ULL
    );

    // 4. Crear y minar un nuevo bloque conteniendo la transacción
    NakaCore::Block block2;
    block2.header.version = 1;
    block2.header.timestamp = 1762700700;
    block2.header.bits = 0x1d00ffff;
    block2.header.nonce = 0;
    block2.vtx.push_back(transferTx);

    std::cout << "[*] Mining Block #1 with transaction payload (SHA-256 PoW)...\n";
    if (NakaCore::ProofOfWork::MineBlock(block2)) {
        std::cout << "[SUCCESS] Block #1 mined successfully and transaction verified!\n";
        nakachain.AddBlock(block2);
    }

    std::cout << "=== Naka Core Node Running Successfully (Ctrl+C to stop) ===\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    p2p.Stop();
    return 0;
}
