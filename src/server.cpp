#include "server.h"
#include "client_handler.h"
#include "logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>

using namespace tslog;

Server::Server(int port)
    : port_(port), listen_fd_(-1) {}

Server::~Server() { stop(); }

void Server::start() {
    if (running_) return;

    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
        throw std::runtime_error("socket() failed: " + std::string(std::strerror(errno)));
    }

    int opt = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(listen_fd_);
        throw std::runtime_error("bind() failed: " + std::string(std::strerror(errno)));
    }

    if (listen(listen_fd_, SOMAXCONN) < 0) {
        close(listen_fd_);
        throw std::runtime_error("listen() failed: " + std::string(std::strerror(errno)));
    }

    running_ = true;
    Logger::instance().info("Servidor iniciado na porta " + std::to_string(port_));
    accept_thread_ = std::thread(&Server::accept_loop, this);
}

void Server::stop() {
    if (!running_) return;
    running_ = false;
    // fechar socket de escuta para acordar accept()
    if (listen_fd_ >= 0) {
        shutdown(listen_fd_, SHUT_RDWR);
        close(listen_fd_);
        listen_fd_ = -1;
    }
    if (accept_thread_.joinable()) accept_thread_.join();

    // parar todos handlers
    std::vector<std::shared_ptr<ClientHandler>> copy;
    {
        std::lock_guard<std::mutex> lk(clients_mtx_);
        for (auto &p : clients_) copy.push_back(p.second);
        clients_.clear();
    }
    for (auto &h : copy) {
        if (h) h->stop();
    }
    Logger::instance().info("Servidor parado.");
}

void Server::accept_loop() {
    while (running_) {
        sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);
        int client_fd = ::accept(listen_fd_, reinterpret_cast<sockaddr*>(&cliaddr), &clilen);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            if (!running_) break;
            Logger::instance().warn("accept() falhou: " + std::string(std::strerror(errno)));
            continue;
        }

        // opção: desligar Nagle
        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf));
        Logger::instance().info("Conexão aceita de " + std::string(buf) + ":" + std::to_string(ntohs(cliaddr.sin_port))
                              + " fd=" + std::to_string(client_fd));
        add_client(client_fd);
    }
}

void Server::add_client(int client_fd) {
    auto handler = std::make_shared<ClientHandler>(this, client_fd);
    {
        std::lock_guard<std::mutex> lk(clients_mtx_);
        clients_.emplace(client_fd, handler);
    }
    handler->start();
}

void Server::remove_client(int client_fd) {
    std::shared_ptr<ClientHandler> h;
    {
        std::lock_guard<std::mutex> lk(clients_mtx_);
        auto it = clients_.find(client_fd);
        if (it != clients_.end()) {
            h = it->second;
            clients_.erase(it);
        }
    }
    if (h) {
        try { h->stop(); } catch(...) {}
    }
    Logger::instance().info("Removido cliente fd " + std::to_string(client_fd));
}

void Server::broadcast(int from_fd, const std::string &message) {
    std::lock_guard<std::mutex> lk(clients_mtx_);
    for (auto &p : clients_) {
        int fd = p.first;
        auto handler = p.second;
        if (!handler) continue;
        // opcional: não reenviar para o próprio cliente
        if (fd == from_fd) continue;
        handler->send_message(message);
    }
    Logger::instance().info("Broadcast a partir de fd " + std::to_string(from_fd) + ": " + message);
}

void Server::print_stats() {
    std::lock_guard<std::mutex> lk(clients_mtx_);
    Logger::instance().info("Clientes conectados: " + std::to_string(clients_.size()));
    std::cout << "Clientes conectados: " << clients_.size() << "\n";
}
