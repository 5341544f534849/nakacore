#ifndef NAKACORE_CLI_H
#define NAKACORE_CLI_H

#include "blockchain.h"
#include "wallet.h"
#include <iostream>
#include <string>

namespace NakaCore {

class CLI {
public:
    static void PrintHelp() {
        std::cout << "\n=== Naka Core ($NKC) Interactive CLI ===\n";
        std::cout << "Commands available:\n";
        std::cout << "  height   - Show current blockchain height\n";
        std::cout << "  wallet   - Show local miner wallet address\n";
        std::cout << "  mine     - Mine a new block via Proof-of-Work\n";
        std::cout << "  exit     - Shutdown daemon\n";
        std::cout << "=========================================\n";
    }

    static void Run(Blockchain& blockchain, Wallet& wallet) {
        std::string command;
        PrintHelp();
        
        while (true) {
            std::cout << "\nnakacore> ";
            std::cin >> command;

            if (command == "height") {
                std::cout << "[+] Current Blockchain Height: " << blockchain.GetHeight() << "\n";
            } 
            else if (command == "wallet") {
                std::cout << "[+] Wallet Address: " << wallet.GetAddressHex() << "\n";
            } 
            else if (command == "mine") {
                Block new_block;
                new_block.header.version = 1;
                new_block.header.timestamp = static_cast<uint32_t>(time(nullptr));
                new_block.header.bits = 0x1d00ffff;
                new_block.header.nonce = 0;

                std::cout << "[*] Mining new block with SHA-256 PoW...\n";
                if (ProofOfWork::MineBlock(new_block)) {
                    std::cout << "[SUCCESS] Block mined successfully!\n";
                    blockchain.AddBlock(new_block);
                } else {
                    std::cout << "[-] Mining failed to find valid nonce.\n";
                }
            } 
            else if (command == "exit") {
                std::cout << "[+] Shutting down Naka Core daemon...\n";
                break;
            } 
            else {
                std::cout << "[-] Unknown command. Type 'help' or check options.\n";
            }
        }
    }
};

} // namespace NakaCore

#endif // NAKACORE_CLI_H
