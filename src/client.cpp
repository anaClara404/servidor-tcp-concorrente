#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>   // para strerror()
#include <cerrno>    // para errno

#include "logger.h"

#include <thread>
#include <iostream>
#include <string>
#include <atomic>

using namespace tslog;

static std::atomic<bool> running{true};

void recv_thread_fn(int sockfd) {
    char buf[4096];
    while (running) {
        ssize_t n = ::recv(sockfd, buf, sizeof(buf)-1, 0);
        if (n > 0) {
            buf[n] = '\0';
            std::cout << buf;
            std::cout.flush();
        } else if (n == 0) {
            Logger::instance().info("Servidor fechou a conexão.");
            running = false;
            break;
        } else {
            if (errno == EINTR) continue;
            Logger::instance().warn("recv() erro no cliente: " + std::string(strerror(errno)));
            running = false;
            break;
        }
    }
}

int main(int argc, char **argv) {
    std::string host = "127.0.0.1";
    int port = 3333;
    std::string logfile = "client.log";

    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = std::stoi(argv[2]);
    if (argc >= 4) logfile = argv[3];

    Logger::init(logfile, true);

    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        Logger::instance().error("socket() failed");
        return 1;
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &servaddr.sin_addr);

    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        Logger::instance().error(std::string("connect() failed: ") + strerror(errno));
        close(sockfd);
        return 1;
    }

    Logger::instance().info("Conectado ao servidor " + host + ":" + std::to_string(port));
    std::thread recvthr(recv_thread_fn, sockfd);

    // loop de leitura do teclado
    std::string line;
    while (running && std::getline(std::cin, line)) {
        if (line == "/quit" || line == "/exit") break;
        // garante newline
        line.push_back('\n');
        ssize_t total = 0;
        const char *data = line.c_str();
        size_t len = line.size();
        while (total < (ssize_t)len) {
            ssize_t sent = ::send(sockfd, data + total, len - total, 0);
            if (sent <= 0) {
                if (errno == EINTR) continue;
                Logger::instance().warn("send() falhou: " + std::string(strerror(errno)));
                running = false;
                break;
            }
            total += sent;
        }
    }

    running = false;
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    if (recvthr.joinable()) recvthr.join();
    Logger::shutdown();
    return 0;
}
