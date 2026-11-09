#include "blockchain.h"
#include "net.h"
#include "wallet.h"
#include "mempool.h"
#include "validation.h"
#include "cli.h"
#include <iostream>

int main() {
    std::cout << "=== Naka Core ($NKC) Full Daemon Initializing ===\n";

    // 1. Iniciar servidor P2P
    NakaCore::P2PNode p2p(8333);
    if (!p2p.Start()) {
        std::cout << "[-] Warning: P2P network port busy.\n";
    }

    // 2. Inicializar componentes centrales
    NakaCore::Blockchain nakachain;
    NakaCore::Wallet localWallet;
    NakaCore::Mempool mempool;

    std::cout << "[+] All core subsystems online and synchronized.\n";

    // 3. Lanzar la consola CLI maestra
    NakaCore::CLI::Run(nakachain, localWallet, mempool);

    // 4. Apagado seguro
    p2p.Stop();
    return 0;
}
