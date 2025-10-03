#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <mutex>

class Server;

class ClientHandler : public std::enable_shared_from_this<ClientHandler> {
public:
    ClientHandler(Server *server, int client_fd);
    ~ClientHandler();

    void start();
    void stop();

    int fd() const { return client_fd_; }
    // envia mensagem segura para esse cliente
    void send_message(const std::string &msg);

private:
    Server *server_;
    int client_fd_;
    std::thread thr_;
    std::atomic<bool> running_{false};
    std::mutex write_mtx_;

    void run();
    std::string read_line();
};

#endif // CLIENT_HANDLER_H