#include "blockchain.h"
#include "net.h"
#include "wallet.h"
#include "cli.h"
#include <iostream>

int main() {
    std::cout << "=== Naka Core ($NKC) Daemon Starting ===\n";

    // 1. Iniciar red P2P en segundo plano
    NakaCore::P2PNode p2p(8333);
    if (!p2p.Start()) {
        std::cout << "[-] Warning: P2P network port busy or unavailable.\n";
    }

    // 2. Inicializar Blockchain y Billetera local
    NakaCore::Blockchain nakachain;
    NakaCore::Wallet localWallet;

    std::cout << "[+] Node fully initialized and operational.\n";

    // 3. Ejecutar consola interactiva CLI
    NakaCore::CLI::Run(nakachain, localWallet);

    // 4. Detener servicios al salir
    p2p.Stop();
    return 0;
}
