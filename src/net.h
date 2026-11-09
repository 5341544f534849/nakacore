#ifndef NAKACORE_NET_H
#define NAKACORE_NET_H

#include <iostream>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace NakaCore {

class P2PNode {
private:
    int server_fd;
    uint16_t port;
    bool running;
    std::thread listener_thread;

public:
    P2PNode(uint16_t p) : port(p), server_fd(-1), running(false) {}

    ~P2PNode() {
        Stop();
    }

    bool Start() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
            std::cout << "[-] P2P Error: Socket creation failed.\n";
            return false;
        }

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cout << "[-] P2P Error: Bind failed on port " << port << "\n";
            close(server_fd);
            return false;
        }

        if (listen(server_fd, 5) < 0) {
            std::cout << "[-] P2P Error: Listen failed.\n";
            close(server_fd);
            return false;
        }

        running = true;
        std::cout << "[+] P2P Server listening securely on port " << port << "\n";

        // Iniciar bucle de escucha en un hilo independiente
        listener_thread = std::thread(&P2PNode::ListenLoop, this);
        return true;
    }

    void Stop() {
        if (running) {
            running = false;
            if (server_fd != -1) {
                close(server_fd);
                server_fd = -1;
            }
            if (listener_thread.joinable()) {
                listener_thread.join();
            }
            std::cout << "[+] P2P Server stopped.\n";
        }
    }

private:
    void ListenLoop() {
        while (running) {
            sockaddr_in client_addr{};
            socklen_t addrlen = sizeof(client_addr);
            int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
            if (new_socket < 0) {
                if (!running) break;
                continue;
            }

            std::cout << "[+] Peer connected to Naka Core network node.\n";
            close(new_socket);
        }
    }
};

} // namespace NakaCore

#endif // NAKACORE_NET_H
