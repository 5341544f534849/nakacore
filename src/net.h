#ifndef NAKACORE_NET_H
#define NAKACORE_NET_H

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

namespace NakaCore {

class P2PNetwork {
private:
    int server_fd;
    int port;
    std::atomic<bool> running;
    std::thread listener_thread;
    std::mutex peers_mutex;
    std::vector<int> connected_peers;

    void HandleClient(int client_socket) {
        char buffer[1024] = {0};
        int valread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (valread > 0) {
            std::string msg(buffer);
            std::cout << "[P2P] Mensaje recibido de nodo par: " << msg << "\n";
            
            std::string response = "NAKA_ACK_v1.0\n";
            send(client_socket, response.c_str(), response.length(), 0);
        }
        close(client_socket);
    }

public:
    P2PNetwork(int p) : port(p), running(false), server_fd(-1) {}

    void Start() {
        running = true;
        listener_thread = std::thread([this]() {
            server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd < 0) {
                std::cerr << "[-] Error creando socket P2P\n";
                return;
            }

            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);

            if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
                std::cerr << "[-] Error en bind del puerto P2P " << port << "\n";
                return;
            }

            if (listen(server_fd, 10) < 0) {
                std::cerr << "[-] Error en listen P2P\n";
                return;
            }

            std::cout << "[+] Servidor P2P descentralizado escuchando activamente en el puerto " << port << "\n";

            while (running) {
                sockaddr_in client_addr{};
                socklen_t addrlen = sizeof(client_addr);
                int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
                if (client_socket < 0) continue;

                {
                    std::lock_guard<std::mutex> lock(peers_mutex);
                    connected_peers.push_back(client_socket);
                }

                std::thread(&P2PNetwork::HandleClient, this, client_socket).detach();
            }
        });
    }

    void Stop() {
        running = false;
        if (server_fd >= 0) {
            close(server_fd);
        }
        if (listener_thread.joinable()) {
            listener_thread.join();
        }
    }

    size_t GetConnectedPeersCount() {
        std::lock_guard<std::mutex> lock(peers_mutex);
        return connected_peers.size();
    }
};

} // namespace NakaCore

#endif // NAKACORE_NET_H
