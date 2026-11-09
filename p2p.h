#ifndef P2P_H
#define P2P_H

#include <string>
#include <vector>

class P2PNetwork {
private:
    int serverPort;
    int server_fd;
    std::vector<std::string> connectedPeers;

public:
    P2PNetwork(int port);
    ~P2PNetwork();
    void startServer();
    void connectToPeer(const std::string& address, int port);
    void broadcast(const std::string& message);
};

#endif
