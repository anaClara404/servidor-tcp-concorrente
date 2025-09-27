#include "logger.h"

#include <thread>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstdlib>

using namespace tslog;

int main(int argc, char **argv) {
    int nthreads = 8;
    int nmsgs = 1000;
    std::string logfile = "tslog_output.log";
    bool console = true;

    if (argc >= 2) nthreads = std::atoi(argv[1]);
    if (argc >= 3) nmsgs = std::atoi(argv[2]);
    if (argc >= 4) logfile = argv[3];
    if (argc >= 5) console = std::atoi(argv[4]) != 0;

    std::cout << "Iniciando teste com " << nthreads << " threads, "
              << nmsgs << " mensagens cada. Log: " << logfile << "\n";

    Logger::init(logfile, console);

    std::vector<std::thread> threads;
    for (int t = 0; t < nthreads; ++t) {
        threads.emplace_back([t, nmsgs]{
            for (int i = 0; i < nmsgs; ++i) {
                std::ostringstream oss;
                oss << "Mensagem " << i << " da thread " << t;
                // alterna níveis só para teste
                if (i % 10 == 0) Logger::instance().debug(oss.str());
                else if (i % 5 == 0) Logger::instance().warn(oss.str());
                else Logger::instance().info(oss.str());
            }
        });
    }

    for (auto &th : threads) if (th.joinable()) th.join();

    // Envia algumas mensagens finais
    Logger::instance().error("Teste concluído - enviando mensagem final.");
    // Dá tempo para a thread de escrita processar (opcional)
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    Logger::shutdown();
    std::cout << "Logger finalizado. Verifique: " << logfile << "\n";
    return 0;
}
