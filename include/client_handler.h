#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <thread>
#include <atomic>

class ClientHandler {
public:
    ClientHandler(int client_fd);
    ~ClientHandler();

    void start();
    void stop();

private:
    int client_fd_;
    std::thread thr_;
    std::atomic<bool> running_{false};

    void run();
};

#endif // CLIENT_HANDLER_H
