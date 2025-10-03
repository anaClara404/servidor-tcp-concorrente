#include "server.h"
#include "logger.h"

#include <signal.h>
#include <iostream>
#include <string>

using namespace tslog;

static Server *g_server = nullptr;

void handle_sigint(int) {
    if (g_server) g_server->stop();
    Logger::instance().info("Signal SIGINT recebido, encerrando servidor.");
    Logger::shutdown();
    std::exit(0);
}

int main(int argc, char **argv) {
    int port = 3333;
    std::string logfile = "server.log";

    if (argc >= 2) port = std::stoi(argv[1]);
    if (argc >= 3) logfile = argv[2];

    Logger::init(logfile, true);
    Logger::instance().info("Inicializando servidor (Etapa 2)");

    Server server(port);
    g_server = &server;

    signal(SIGINT, handle_sigint);

    try {
        server.start();
        std::string cmd;
        while (true) {
            std::cout << "server> ";
            if (!std::getline(std::cin, cmd)) break;
            if (cmd == "quit" || cmd == "exit") break;
            if (cmd == "stats") server.print_stats();
            // outras opções podem ser adicionadas
        }
    } catch (const std::exception &ex) {
        Logger::instance().error(std::string("Exceção: ") + ex.what());
    }

    server.stop();
    Logger::shutdown();
    return 0;
}
