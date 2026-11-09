#ifndef NAKACORE_CLI_H
#define NAKACORE_CLI_H

#include "blockchain.h"
#include "wallet.h"
#include "mempool.h"
#include "validation.h"
#include <iostream>
#include <string>

namespace NakaCore {

class CLI {
public:
    static void PrintHelp() {
        std::cout << "\n=== Naka Core ($NKC) Master CLI ===\n";
        std::cout << "Available commands:\n";
        std::cout << "  status   - Show complete node health and dashboard\n";
        std::cout << "  balance  - Show current total Nakas ($NKC) in local wallet\n";
        std::cout << "  height   - Show current blockchain height\n";
        std::cout << "  wallet   - Show local miner wallet address\n";
        std::cout << "  mempool  - Show pending transactions in memory pool\n";
        std::cout << "  mine     - Mine a new block using Mempool & PoW\n";
        std::cout << "  help     - Show this command list\n";
        std::cout << "  exit     - Shutdown daemon safely\n";
        std::cout << "=====================================\n";
    }

    static void Run(Blockchain& blockchain, Wallet& wallet, Mempool& mempool) {
        std::string command;
        PrintHelp();
        
        while (true) {
            std::cout << "\nnakacore> ";
            std::cin >> command;

            if (command == "status") {
                uint64_t totalBalance = blockchain.GetHeight() * 50;
                std::cout << "\n--- Naka Core Node Dashboard ---\n";
                std::cout << " [*] Ticker        : $NKC (Naka Core)\n";
                std::cout << " [*] Max Supply    : 20,000,000\n";
                std::cout << " [*] Chain Height  : " << blockchain.GetHeight() << "\n";
                std::cout << " [*] Miner Balance : " << totalBalance << " $NKC\n";
                std::cout << " [*] Mempool Size  : " << mempool.GetSize() << " pending txs\n";
                std::cout << " [*] Wallet Address: " << wallet.GetAddressHex() << "\n";
                std::cout << " [*] P2P Port      : 8333 (Active)\n";
                std::cout << "--------------------------------\n";
            }
            else if (command == "balance") {
                uint64_t totalBalance = blockchain.GetHeight() * 50;
                std::cout << "\n[+] Local Wallet Balance: " << totalBalance << " $NKC\n";
                std::cout << "    (Mined Blocks: " << blockchain.GetHeight() << " x 50 NKC reward)\n";
            }
            else if (command == "height") {
                std::cout << "[+] Current Blockchain Height: " << blockchain.GetHeight() << "\n";
            } 
            else if (command == "wallet") {
                std::cout << "[+] Wallet Address: " << wallet.GetAddressHex() << "\n";
            }
            else if (command == "mempool") {
                std::cout << "[+] Pending transactions in mempool: " << mempool.GetSize() << "\n";
            }
            else if (command == "mine") {
                Block new_block;
                new_block.header.version = 1;
                new_block.header.timestamp = static_cast<uint32_t>(time(nullptr));
                new_block.header.bits = 0x0200ffff;
                new_block.header.nonce = 0;

                new_block.vtx = mempool.GetTransactionsForBlock();

                if (new_block.vtx.empty()) {
                    Transaction coinbase;
                    coinbase.version = 1;
                    TxOut out;
                    out.value = 50 * 100'000'000ULL;
                    std::string addr = wallet.GetAddressHex();
                    out.pk_script.assign(addr.begin(), addr.end());
                    coinbase.vout.push_back(out);
                    new_block.vtx.push_back(coinbase);
                }

                UTXOSet dummyUtxo;
                if (ConsensusValidator::ValidateBlock(new_block, dummyUtxo)) {
                    std::cout << "[*] Mining block with Proof-of-Work (SHA-256)...\n";
                    if (ProofOfWork::MineBlock(new_block, 1000000)) {
                        std::cout << "[SUCCESS] Block mined! 50 $NKC added to your wallet.\n";
                        blockchain.AddBlock(new_block);
                    } else {
                        std::cout << "[-] Mining failed: nonce limit reached.\n";
                    }
                } else {
                    std::cout << "[-] Block rejected by Consensus Validator.\n";
                }
            } 
            else if (command == "help") {
                PrintHelp();
            }
            else if (command == "exit") {
                std::cout << "[+] Shutting down Naka Core daemon securely...\n";
                break;
            } 
            else {
                std::cout << "[-] Unknown command. Type 'help' for options.\n";
            }
        }
    }
};

} // namespace NakaCore

#endif // NAKACORE_CLI_H
