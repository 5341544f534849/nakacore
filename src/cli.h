#ifndef NAKACORE_CLI_H
#define NAKACORE_CLI_H

#include "blockchain.h"
#include "wallet.h"
#include "mempool.h"
#include <iostream>
#include <string>
#include <sstream>

namespace NakaCore {

class CLI {
public:
    static void Run(Blockchain& blockchain, Wallet& wallet, Mempool& mempool) {
        std::string command;
        
        std::cout << "\n=== Naka Core ($NKC) Master CLI ===\n";
        std::cout << "Available commands:\n";
        std::cout << "  status                - Show complete node health and dashboard\n";
        std::cout << "  balance               - Show current total Nakas ($NKC)\n";
        std::cout << "  height                - Show current blockchain height\n";
        std::cout << "  wallet                - Show local miner wallet address\n";
        std::cout << "  mempool               - Show pending transactions\n";
        std::cout << "  send <addr> <amount>  - Send $NKC to another address\n";
        std::cout << "  help                  - Show this command list\n";
        std::cout << "  exit                  - Shutdown daemon safely\n";
        std::cout << "======================================\n";

        while (true) {
            std::cout << "nakacore> ";
            if (!std::getline(std::cin, command)) break;

            if (command == "exit") {
                std::cout << "[+] Shutting down Naka Core daemon...\n";
                break;
            } else if (command == "status") {
                std::cout << "[i] Blockchain Height: " << blockchain.GetHeight() << "\n";
                std::cout << "[i] Mempool Transactions: " << mempool.GetSize() << "\n";
                std::cout << "[i] Wallet Address: " << wallet.GetAddressHex() << "\n";
                std::cout << "[i] Balance: " << (blockchain.GetHeight() + 1) * 50 << " $NKC\n";
            } else if (command == "balance") {
                uint64_t bal = (blockchain.GetHeight() + 1) * 50;
                std::cout << "[+] Total Balance: " << bal << " $NKC\n";
            } else if (command == "height") {
                std::cout << "[+] Current Height: " << blockchain.GetHeight() << "\n";
            } else if (command == "wallet") {
                std::cout << "[+] Address: " << wallet.GetAddressHex() << "\n";
            } else if (command == "mempool") {
                std::cout << "[+] Mempool contains " << mempool.GetSize() << " pending transactions.\n";
            } else if (command.rfind("send ", 0) == 0) {
                std::stringstream ss(command);
                std::string cmd, dest;
                uint64_t amount;
                ss >> cmd >> dest >> amount;

                if (dest.empty() || amount == 0) {
                    std::cout << "[-] Usage: send <address> <amount>\n";
                } else {
                    // Crear transacción simulada y añadirla al Mempool
                    Transaction tx;
                    tx.version = 1;
                    TxOut out;
                    out.value = amount * 100'000'000ULL; // Convertir a satoshis/nakas fraccionales
                    out.pk_script.assign(dest.begin(), dest.end());
                    tx.vout.push_back(out);

                    mempool.AddTransaction(tx);
                    std::cout << "[+] Transacción creada y añadida al Mempool con éxito. Será minada en el siguiente bloque.\n";
                }
            } else if (command == "help") {
                std::cout << "Commands: status, balance, height, wallet, mempool, send <addr> <amount>, help, exit\n";
            } else if (!command.empty()) {
                std::cout << "[-] Unknown command. Type 'help' for options.\n";
            }
        }
    }
};

} // namespace NakaCore

#endif // NAKACORE_CLI_H
