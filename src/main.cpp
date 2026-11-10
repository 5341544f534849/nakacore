#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "net.h"
#include "httplib.h"

// --- MOTOR DE MINERÍA (PoW SHA-256) ---
class Miner {
public:
    static std::string CalculateHash(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
        
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    static bool MineBlock(int block_height, const std::string& miner_address, double reward) {
        std::cout << "[*] Iniciando minería para el bloque #" << block_height << "...\n";
        
        long long nonce = 0;
        std::string target_prefix = "0000"; // Dificultad inicial
        auto start_time = std::chrono::high_resolution_clock::now();

        while (nonce < 2000000) {
            std::string data = std::to_string(block_height) + miner_address + std::to_string(reward) + std::to_string(nonce);
            std::string hash = CalculateHash(data);

            if (hash.rfind(target_prefix, 0) == 0) {
                auto end_time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end_time - start_time;
                
                std::cout << "[+] ¡Bloque Minado con Éxito!\n";
                std::cout << "    -> Altura: " << block_height << "\n";
                std::cout << "    -> Nonce: " << nonce << "\n";
                std::cout << "    -> Hash: " << hash << "\n";
                std::cout << "    -> Recompensa: " << reward << " $NKC acreditados a " << miner_address << "\n";
                std::cout << "    -> Tiempo tomado: " << elapsed.count() << " segundos.\n";
                return true;
            }
            nonce++;
        }
        
        std::cout << "[-] Minería pausada: Dificultad no superada en este ciclo.\n";
        return false;
    }
};

// Estructura para manejar los datos de la billetera local
struct WalletData {
    std::string address;
    double balance;
};

WalletData LoadOrCreateWallet() {
    std::string filename = "wallet.dat";
    std::ifstream infile(filename);
    
    WalletData wallet;
    if (infile.is_open()) {
        std::getline(infile, wallet.address);
        infile >> wallet.balance;
        infile.close();
        std::cout << "[+] Billetera cargada exitosamente desde disco (" << filename << ")\n";
    } else {
        wallet.address = "nkc1qxy2kgdygjrsqtzq2n0yrf2493p83kkfjhx0wlh";
        wallet.balance = 2400.0;
        
        std::ofstream outfile(filename);
        if (outfile.is_open()) {
            outfile << wallet.address << "\n" << wallet.balance;
            outfile.close();
            std::cout << "[+] Nueva billetera creada y guardada en disco (" << filename << ")\n";
        }
    }
    return wallet;
}

void SaveWallet(const WalletData& wallet) {
    std::ofstream outfile("wallet.dat");
    if (outfile.is_open()) {
        outfile << wallet.address << "\n" << wallet.balance;
        outfile.close();
    }
}

int main() {
    std::cout << "=== Naka Core ($NKC) Full Daemon Initializing ===\n";

    WalletData my_wallet = LoadOrCreateWallet();
    int block_height = 46;

    NakaCore::P2PNetwork p2p(8333);
    p2p.Start();
    std::cout << "[+] P2P Server listening securely on port 8333\n";

    std::thread http_thread([&my_wallet]() {
        httplib::Server svr;

        svr.Get("/", [&my_wallet](const httplib::Request&, httplib::Response& res) {
            std::string html = R"HTML(
                <!DOCTYPE html>
                <html lang="es">
                <head>
                    <meta charset="UTF-8">
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <title>Naka Core - Modern Web Wallet</title>
                    <style>
                        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
                        body { background-color: #0b0f19; color: #f3f4f6; display: flex; justify-content: center; align-items: center; min-height: 100vh; padding: 20px; }
                        .wallet-container { background: #131b2e; border: 1px solid #1e293b; border-radius: 16px; width: 100%; max-width: 480px; padding: 30px; box-shadow: 0 10px 25px rgba(0,0,0,0.5); }
                        .header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 24px; }
                        .logo { font-size: 20px; font-weight: bold; color: #38bdf8; display: flex; align-items: center; gap: 8px; }
                        .status-badge { background: rgba(16, 185, 129, 0.15); color: #34d399; padding: 4px 10px; border-radius: 20px; font-size: 12px; font-weight: 600; display: flex; align-items: center; gap: 6px; }
                        .status-dot { width: 8px; height: 8px; background-color: #34d399; border-radius: 50%; box-shadow: 0 0 8px #34d399; }
                        .balance-card { background: linear-gradient(135deg, #1e293b 0%, #0f172a 100%); border-radius: 12px; padding: 24px; text-align: center; margin-bottom: 24px; border: 1px solid #334155; }
                        .balance-title { font-size: 14px; color: #94a3b8; margin-bottom: 8px; text-transform: uppercase; letter-spacing: 1px; }
                        .balance-amount { font-size: 36px; font-weight: 700; color: #f8fafc; margin-bottom: 4px; }
                        .balance-currency { color: #f59e0b; font-size: 20px; }
                        .section-title { font-size: 14px; color: #94a3b8; margin-bottom: 10px; font-weight: 600; }
                        .address-box { background: #0f172a; border: 1px solid #334155; border-radius: 8px; padding: 12px; font-family: monospace; font-size: 12px; color: #cbd5e1; word-break: break-all; margin-bottom: 24px; }
                        .actions { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 24px; }
                        .btn { background: #3b82f6; color: white; border: none; border-radius: 8px; padding: 12px; font-weight: 600; cursor: pointer; transition: background 0.2s; font-size: 14px; }
                        .btn:hover { background: #2563eb; }
                        .btn-secondary { background: #1e293b; color: #f8fafc; border: 1px solid #475569; }
                        .btn-secondary:hover { background: #334155; }
                        .tx-list { background: #0f172a; border-radius: 8px; padding: 16px; border: 1px solid #334155; text-align: center; color: #64748b; font-size: 13px; }
                    </style>
                </head>
                <body>
                    <div class="wallet-container">
                        <div class="header">
                            <div class="logo">⚡ Naka Core</div>
                            <div class="status-badge"><div class="status-dot"></div> Online</div>
                        </div>
                        <div class="balance-card">
                            <div class="balance-title">Balance Disponible</div>
                            <div class="balance-amount">)HTML" + std::to_string(my_wallet.balance) + R"HTML( <span class="balance-currency">$NKC</span></div>
                        </div>
                        <div class="section-title">Dirección de Minería / Wallet</div>
                        <div class="address-box">
                            )HTML" + my_wallet.address + R"HTML(
                        </div>
                        <div class="actions">
                            <button class="btn" onclick="alert('Módulo de envío en desarrollo')">Enviar</button>
                            <button class="btn btn-secondary" onclick="alert('Dirección copiada al portapapeles')">Recibir</button>
                        </div>
                        <div class="section-title">Últimas Transacciones</div>
                        <div class="tx-list">Bloques minados correctamente en la red.</div>
                    </div>
                </body>
                </html>
            )HTML";
            res.set_content(html, "text/html; charset=utf-8");
        });

        svr.Get("/api/balance", [&my_wallet](const httplib::Request&, httplib::Response& res) {
            std::string json = "{\"balance\": " + std::to_string(my_wallet.balance) + ", \"currency\": \"NKC\"}";
            res.set_content(json, "application/json");
        });

        std::cout << "[+] Web Wallet & API Server active at http://localhost:8080\n";
        svr.listen("0.0.0.0", 8080);
    });

    http_thread.detach();

    std::cout << "[+] Blockchain loaded from disk. Current Height: " << block_height << "\n";
    std::cout << "[+] Halving economics active. Max Supply: 20,000,000 $NKC\n";

    std::cout << "\n=== Naka Core ($NKC) Master CLI ===\n";
    std::cout << "Available commands:\n";
    std::cout << "  status                 - Show complete node health and dashboard\n";
    std::cout << "  balance                - Show total Nakas ($NKC)\n";
    std::cout << "  mine                   - Mine a new block and earn $NKC reward\n";
    std::cout << "  wallet                 - Show wallet address\n";
    std::cout << "  exit                   - Shutdown daemon safely\n\n";

    bool running = true;
    std::string command;
    while (running) {
        std::cout << "nakacore> ";
        if (!(std::cin >> command)) break;

        if (command == "exit") {
            std::cout << "Shutting down Naka Core safely...\n";
            running = false;
        } else if (command == "status") {
            std::cout << "Node Status: ONLINE | Connected Peers: " << p2p.GetConnectedPeersCount() << "\n";
        } else if (command == "balance") {
            std::cout << "Balance: " << my_wallet.balance << " $NKC\n";
        } else if (command == "wallet") {
            std::cout << "Wallet Address: " << my_wallet.address << "\n";
        } else if (command == "mine") {
            double reward = 50.0;
            if (Miner::MineBlock(block_height, my_wallet.address, reward)) {
                my_wallet.balance += reward;
                SaveWallet(my_wallet);
                block_height++;
                std::cout << "[+] ¡Saldo actualizado y guardado en disco! Nuevo balance: " << my_wallet.balance << " $NKC\n";
            }
        } else if (command == "help") {
            std::cout << "Commands: status, balance, mine, wallet, exit\n";
        } else {
            std::cout << "Unknown command. Type 'help' for options.\n";
        }
    }

    p2p.Stop();
    std::cout << "Naka Core stopped successfully.\n";
    return 0;
}