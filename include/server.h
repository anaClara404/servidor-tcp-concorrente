#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>

class Server {
public:
    Server(int port);
    ~Server();

    // inicia o servidor (thread accept loop)
    void start();

    // para o servidor
    void stop();

    // mostra estatísticas simples
    void print_stats();

private:
    int port_;
    int listen_fd_;
    std::mutex clients_mtx_;
    // vetor de descriptors/handlers (a implementar)
    // std::vector<std::shared_ptr<ClientHandler>> clients_;
};

#endif // CHAT_SERVER_H
