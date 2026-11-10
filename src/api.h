#ifndef NAKACORE_API_H
#define NAKACORE_API_H

#include "blockchain.h"
#include "wallet.h"
#include "mempool.h"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <sstream>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

namespace NakaCore {

class APIServer {
private:
    int server_fd;
    int port;
    std::atomic<bool> running;
    std::thread server_thread;

    std::string GetHTMLDashboard() {
        return R"raw(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Naka Core ($NKC) Web Wallet & Node</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
        body { background-color: #0b0e14; color: #e1e6ed; padding: 20px; }
        .container { max-width: 800px; margin: 0 auto; }
        header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #1f293d; padding-bottom: 15px; margin-bottom: 25px; }
        .brand { display: flex; align-items: center; gap: 12px; }
        .logo { width: 45px; height: 45px; }
        h1 { color: #ffffff; font-size: 24px; font-weight: 800; }
        .ticker { color: #b08d57; font-weight: 400; font-size: 18px; margin-left: 4px; }
        .badge { background-color: #151c28; color: #00e676; padding: 5px 12px; border-radius: 20px; font-size: 13px; font-weight: bold; border: 1px solid #1f293d; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 15px; margin-bottom: 25px; }
        .card { background-color: #151c28; border: 1px solid #1f293d; border-radius: 12px; padding: 18px; box-shadow: 0 4px 12px rgba(0,0,0,0.3); }
        .card-title { color: #8b9bb4; font-size: 13px; font-weight: 600; text-transform: uppercase; margin-bottom: 8px; }
        .card-value { font-size: 28px; font-weight: bold; color: #ffffff; }
        .subtext { font-size: 11px; color: #5f7597; margin-top: 4px; }
        .panel { background-color: #151c28; border: 1px solid #1f293d; border-radius: 12px; padding: 20px; margin-bottom: 20px; }
        .panel h3 { color: #b08d57; margin-bottom: 15px; font-size: 18px; }
        .wallet-address { font-family: monospace; color: #b08d57; font-size: 14px; word-break: break-all; background: #0b0e14; padding: 10px; border-radius: 6px; border: 1px solid #1f293d; margin-top: 8px; }
        .form-group { margin-bottom: 12px; }
        label { display: block; font-size: 12px; color: #8b9bb4; margin-bottom: 5px; }
        input { width: 100%; padding: 10px; background: #0b0e14; border: 1px solid #1f293d; border-radius: 6px; color: #fff; font-size: 14px; }
        button { background: #b08d57; color: #0b0e14; border: none; padding: 10px 18px; font-weight: bold; border-radius: 6px; cursor: pointer; transition: background 0.2s; }
        button:hover { background: #c59f65; }
        .success-msg { color: #00e676; font-size: 13px; margin-top: 10px; display: none; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <div class="brand">
                <svg class="logo" viewBox="0 0 100 100" fill="none" xmlns="http://www.w3.org/2000/svg">
                    <path d="M50 10L85 30V70L50 90L15 70V30L50 10Z" fill="#151c28" stroke="#3a4a61" stroke-width="2"/>
                    <path d="M50 10L85 30L50 50L15 30L50 10Z" fill="#2a3649" stroke="#3a4a61" stroke-width="1"/>
                    <path d="M15 30L50 50V90L15 70V30Z" fill="#1f293d" stroke="#3a4a61" stroke-width="1"/>
                    <path d="M50 50L85 30V70L50 90V50Z" fill="#151c28" stroke="#3a4a61" stroke-width="1"/>
                    <path d="M42 45V65M42 45L58 65M42 45V65M58 45V65" stroke="#b08d57" stroke-width="6" stroke-linecap="round" stroke-linejoin="round"/>
                </svg>
                <h1>Naka Core<span class="ticker">$NKC</span></h1>
            </div>
            <div class="badge">● NODO ACTIVO</div>
        </header>

        <div class="grid">
            <div class="card">
                <div class="card-title">Balance Total</div>
                <div class="card-value" id="balance">0 NKC</div>
                <div class="subtext">Fondos disponibles</div>
            </div>
            <div class="card">
                <div class="card-title">Altura de Cadena</div>
                <div class="card-value" id="height">0</div>
                <div class="subtext">Bloques minados</div>
            </div>
            <div class="card">
                <div class="card-title">Mempool</div>
                <div class="card-value" id="mempool">0</div>
                <div class="subtext">Tx pendientes</div>
            </div>
        </div>

        <div class="panel">
            <h3>Billetera del Minero</h3>
            <div class="card-title">Tu Dirección Pública</div>
            <div class="wallet-address" id="address">Cargando...</div>
        </div>

        <div class="panel">
            <h3>Enviar Nakas ($NKC)</h3>
            <div class="form-group">
                <label>Dirección de Destino</label>
                <input type="text" id="destAddress" placeholder="Ej. NakaDestino12345XYZ">
            </div>
            <div class="form-group">
                <label>Cantidad ($NKC)</label>
                <input type="number" id="amount" placeholder="0.00">
            </div>
            <button onclick="sendTransaction()">Enviar Transacción</button>
            <div class="success-msg" id="successMsg">¡Transacción enviada al Mempool con éxito!</div>
        </div>
    </div>

    <script>
        async function updateDashboard() {
            try {
                const res = await fetch('/api/status');
                if (!res.ok) throw new Error('Network error');
                const data = await res.json();
                document.getElementById('balance').innerText = data.balance.toLocaleString() + ' NKC';
                document.getElementById('height').innerText = data.height.toLocaleString();
                document.getElementById('mempool').innerText = data.mempool;
                document.getElementById('address').innerText = data.address;
            } catch(e) {
                console.error('Error sincronizando con el nodo:', e);
            }
        }

        async function sendTransaction() {
            const dest = document.getElementById('destAddress').value;
            const amt = document.getElementById('amount').value;
            if (!dest || !amt) {
                alert('Por favor completa todos los campos.');
                return;
            }
            try {
                const res = await fetch(`/api/send?dest=${encodeURIComponent(dest)}&amount=${amt}`);
                const data = await res.json();
                if (data.status === 'success') {
                    document.getElementById('successMsg').style.display = 'block';
                    document.getElementById('destAddress').value = '';
                    document.getElementById('amount').value = '';
                    setTimeout(() => { document.getElementById('successMsg').style.display = 'none'; }, 4000);
                    updateDashboard();
                } else {
                    alert('Error al procesar la transacción.');
                }
            } catch(e) {
                alert('No se pudo conectar con el nodo.');
            }
        }

        setInterval(updateDashboard, 2000);
        updateDashboard();
    </script>
</body>
</html>
)raw";
    }

public:
    APIServer(int p) : port(p), running(false), server_fd(-1) {}

    void Start(Blockchain& blockchain, Wallet& wallet, Mempool& mempool) {
        running = true;
        server_thread = std::thread([this, &blockchain, &wallet, &mempool]() {
            server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd < 0) return;

            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);

            if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) return;
            if (listen(server_fd, 5) < 0) return;

            std::cout << "[+] Web Wallet & API Server active at http://localhost:" << port << "\n";

            while (running) {
                sockaddr_in client_addr{};
                socklen_t addrlen = sizeof(client_addr);
                int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
                if (client_socket < 0) continue;

                char buffer[4096] = {0};
                read(client_socket, buffer, sizeof(buffer) - 1);
                std::string request(buffer);

                std::string response;
                if (request.find("GET /api/status") != std::string::npos) {
                    uint64_t totalBalance = (blockchain.GetHeight() + 1) * 50;
                    std::ostringstream json;
                    json << "{"
                         << "\"height\":" << blockchain.GetHeight() << ","
                         << "\"balance\":" << totalBalance << ","
                         << "\"mempool\":" << mempool.GetSize() << ","
                         << "\"address\":\"" << wallet.GetAddressHex() << "\""
                         << "}";

                    std::string jsonStr = json.str();
                    response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Content-Length: " + std::to_string(jsonStr.length()) + "\r\n\r\n" + jsonStr;
                } else if (request.find("GET /api/send") != std::string::npos) {
                    // Procesar envío desde la web wallet
                    Transaction tx;
                    tx.version = 1;
                    TxOut out;
                    out.value = 25 * 100'000'000ULL; // Valor por defecto o simulado
                    out.pk_script.assign("NakaDestinoWeb", "NakaDestinoWeb" + 14);
                    mempool.AddTransaction(tx);

                    std::string jsonStr = "{\"status\":\"success\"}";
                    response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Content-Length: " + std::to_string(jsonStr.length()) + "\r\n\r\n" + jsonStr;
                } else {
                    std::string html = GetHTMLDashboard();
                    response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: " + std::to_string(html.length()) + "\r\n\r\n" + html;
                }

                send(client_socket, response.c_str(), response.length(), 0);
                close(client_socket);
            }
        });
    }

    void Stop() {
        running = false;
        if (server_fd >= 0) {
            close(server_fd);
        }
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }
};

} // namespace NakaCore

#endif // NAKACORE_API_H
