#include "client_handler.h"
#include "server.h"
#include "logger.h"

#include <unistd.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>

using namespace tslog;

ClientHandler::ClientHandler(Server *server, int client_fd)
    : server_(server), client_fd_(client_fd) {}

ClientHandler::~ClientHandler() {
    if (thr_.joinable()) {
        if (std::this_thread::get_id() != thr_.get_id()) {
            thr_.join();
        } else {
            thr_.detach(); // evita terminate se for a própria thread
        }
    }
}

void ClientHandler::start() {
    running_ = true;
    thr_ = std::thread(&ClientHandler::run, this);
}

void ClientHandler::stop() {
    running_ = false;
    if (client_fd_ >= 0) {
        shutdown(client_fd_, SHUT_RDWR);
        close(client_fd_);
        client_fd_ = -1;
    }
    // evita join da própria thread
    if (thr_.joinable() && std::this_thread::get_id() != thr_.get_id()) {
        thr_.join();
    }
}


void ClientHandler::send_message(const std::string &msg) {
    std::lock_guard<std::mutex> lk(write_mtx_);
    if (client_fd_ < 0) return;
    size_t total = 0;
    const char *data = msg.c_str();
    size_t len = msg.size();
    while (total < len) {
        ssize_t sent = ::send(client_fd_, data + total, len - total, 0);
        if (sent <= 0) {
            if (errno == EINTR) continue;
            Logger::instance().warn("send() falhou para fd " + std::to_string(client_fd_) + ": " + strerror(errno));
            break;
        }
        total += sent;
    }
}

static const int BUF_SIZE = 4096;

std::string ClientHandler::read_line() {
    std::string accum;
    char buf[BUF_SIZE];
    while (running_) {
        ssize_t n = ::recv(client_fd_, buf, sizeof(buf), 0);
        if (n > 0) {
            accum.append(buf, buf + n);
            // verifica por newline
            auto pos = accum.find('\n');
            if (pos != std::string::npos) {
                std::string line = accum.substr(0, pos + 1);
                // remove consumed
                accum.erase(0, pos + 1);
                return line;
            }
            // continue read
        } else if (n == 0) {
            // conexão fechada
            return std::string();
        } else {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // sem dados no momento
                continue;
            }
            Logger::instance().warn("recv() erro fd " + std::to_string(client_fd_) + ": " + std::strerror(errno));
            return std::string();
        }
    }
    return std::string();
}

void ClientHandler::run() {
    Logger::instance().info("Handler iniciado para fd " + std::to_string(client_fd_));
    // informar servidor que está ativo já foi feito em add_client
    while (running_) {
        std::string line = read_line();
        if (line.empty()) break; // cliente desconectou ou erro
        // trim? manter newline
        Logger::instance().info("Recebido de fd " + std::to_string(client_fd_) + ": " + line);
        server_->broadcast(client_fd_, line);
    }
    Logger::instance().info("Cliente desconectou: fd " + std::to_string(client_fd_));
    server_->remove_client(client_fd_);
}
