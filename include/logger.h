#ifndef TSLOG_LOGGER_H
#define TSLOG_LOGGER_H

#include <string>
#include <memory>

namespace tslog {

enum class Level { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
    // inicializa o logger (abre arquivo) --> deve ser chamado antes de usar
    static void init(const std::string &filepath, bool consoleOutput = true);

    // obtem instancia unica (padrao singleton)
    static Logger &instance();

    // log: thread-safe e nao bloqueante (enqueue rapido)
    void log(Level level, const std::string &message);

    // for convenience
    void debug(const std::string &msg);
    void info(const std::string &msg);
    void warn(const std::string &msg);
    void error(const std::string &msg);

    // finaliza o logger (garante flush e encerra thread de escrita)
    static void shutdown();

private:
    Logger(); 
    ~Logger();

    // não copiavel
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace tslog

#endif // TSLOG_LOGGER_H
