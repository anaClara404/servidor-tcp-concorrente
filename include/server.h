#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <atomic>

class ClientHandler; // forward

class Server {
public:
    Server(int port);
    ~Server();

    // inicia o servidor (acepta conexões em thread separada)
    void start();

    // para o servidor e fecha conexões
    void stop();

    // envia uma mensagem para todos os clientes (broadcast)
    void broadcast(int from_fd, const std::string &message);

    // registra / remove handlers
    void add_client(int client_fd);
    void remove_client(int client_fd);

    // imprime estatísticas simples
    void print_stats();

private:
    int port_;
    int listen_fd_;
    std::thread accept_thread_;
    std::atomic<bool> running_{false};

    // map fd -> shared_ptr<ClientHandler>
    std::mutex clients_mtx_;
    std::unordered_map<int, std::shared_ptr<ClientHandler>> clients_;

    void accept_loop();
};

#endif // CHAT_SERVER_H
