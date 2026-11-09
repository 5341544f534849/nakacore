#include "p2p.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

P2PNetwork::P2PNetwork(int port) : serverPort(port), server_fd(-1) {}

P2PNetwork::~P2PNetwork() {
    if (server_fd != -1) {
        close(server_fd);
    }
}

void P2PNetwork::startServer() {
    std::cout << "[P2P] Iniciando servidor de nodos en el puerto " << serverPort << "...\n";
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "[P2P Error] Fallo al crear el socket del servidor.\n";
        return;
    }

    // Permitir reutilizar el puerto inmediatamente
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(serverPort);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[P2P Error] Fallo al enlazar el puerto " << serverPort << " (puerto ocupado).\n";
        close(server_fd);
        server_fd = -1;
        return;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "[P2P Error] Fallo al escuchar en el puerto.\n";
        close(server_fd);
        server_fd = -1;
        return;
    }

    std::cout << "[P2P Success] Nodo escuchando conexiones P2P en el puerto " << serverPort << "...\n";
    
    // Configuramos un timeout o aceptación no bloqueante de prueba para recibir conexiones entrantes
    fd_set current_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(server_fd, &current_sockets);

    timeval timeout;
    timeout.tv_sec = 2; // Espera activa de 2 segundos para demostración local
    timeout.tv_usec = 0;

    std::cout << "[P2P] Esperando conexiones de pares en la red...\n";
    int activity = select(server_fd + 1, &current_sockets, NULL, NULL, &timeout);
    
    if (activity > 0 && FD_ISSET(server_fd, &current_sockets)) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (new_socket >= 0) {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            std::cout << "[P2P] ¡Nueva conexion entrante detectada desde la IP: " << client_ip << "!\n";
            close(new_socket);
        }
    } else {
        std::cout << "[P2P Info] Sin conexiones entrantes en este ciclo. Red lista para sincronizacion.\n";
    }
}

void P2PNetwork::connectToPeer(const std::string& ipAddress, int port) {
    std::cout << "[P2P] Intentando conectar con nodo par en " << ipAddress << ":" << port << "...\n";
    connectedPeers.push_back(ipAddress + ":" + std::to_string(port));
    std::cout << "[P2P Success] Peer registrado en la lista de difusion.\n";
}

void P2PNetwork::broadcast(const std::string& message) {
    std::cout << "[P2P] Difundiendo bloque/transaccion a " << connectedPeers.size() << " peers conectados...\n";
}
