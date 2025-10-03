#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>   // strerror()
#include <cerrno>    // errno

#include "logger.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace tslog;

int main(int argc, char **argv) {
    std::string host = "127.0.0.1";
    int port = 3333;
    int nmsgs = 10;
    int delay_ms = 100;
    std::string id = "sim";

    if (argc >= 2) id = argv[1];
    if (argc >= 3) nmsgs = std::stoi(argv[2]);
    if (argc >= 4) delay_ms = std::stoi(argv[3]);

    std::string logfile = "sim_client_" + id + ".log";
    Logger::init(logfile, false);

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

    for (int i = 0; i < nmsgs; ++i) {
        std::string msg = "[" + id + "] mensagem " + std::to_string(i) + "\n";
        ::send(sockfd, msg.c_str(), msg.size(), 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    Logger::shutdown();
    return 0;
}
