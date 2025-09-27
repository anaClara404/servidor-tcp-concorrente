#include "logger.h"

#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <iostream>

namespace tslog {

struct Logger::Impl {
    std::ofstream ofs;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> q;
    std::thread worker;
    std::atomic<bool> running{false};
    bool consoleOutput{true};

    Impl() = default;

    void start() {
        running = true;
        worker = std::thread([this]{
            std::unique_lock<std::mutex> lock(mtx);
            while (running || !q.empty()) {
                cv.wait(lock, [this]{ return !running || !q.empty(); });
                while (!q.empty()) {
                    std::string line = std::move(q.front());
                    q.pop();
                    lock.unlock();
                    if (ofs.is_open()) ofs << line << '\n';
                    if (consoleOutput) std::cout << line << '\n';
                    lock.lock();
                }
                if (ofs.good()) ofs.flush();
            }
        });
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lk(mtx);
            running = false;
        }
        cv.notify_all();
        if (worker.joinable()) worker.join();
        if (ofs.is_open()) ofs.close();
    }

    void enqueue(std::string &&line) {
        {
            std::lock_guard<std::mutex> lk(mtx);
            q.push(std::move(line));
        }
        cv.notify_one();
    }
};

static Logger *global_logger = nullptr;
static std::mutex global_init_mtx;

static std::string level_to_string(Level lvl) {
    switch (lvl) {
        case Level::DEBUG: return "DEBUG";
        case Level::INFO:  return "INFO";
        case Level::WARN:  return "WARN";
        case Level::ERROR: return "ERROR";
        default: return "UNK";
    }
}

static std::string now_timestamp() {
    using namespace std::chrono;
    auto t = system_clock::now();
    auto tt = system_clock::to_time_t(t);
    auto ms = duration_cast<milliseconds>(t.time_since_epoch()) % 1000;

    std::tm buf;
    localtime_r(&tt, &buf);

    std::ostringstream oss;
    oss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

Logger::Logger() : pimpl(new Impl()) {}
Logger::~Logger() {
    if (pimpl && pimpl->running) pimpl->stop();
}

void Logger::init(const std::string &filepath, bool consoleOutput) {
    std::lock_guard<std::mutex> lk(global_init_mtx);
    if (!global_logger) {
        global_logger = new Logger();
        global_logger->pimpl->ofs.open(filepath, std::ios::out | std::ios::app);
        global_logger->pimpl->consoleOutput = consoleOutput;
        global_logger->pimpl->start();
    }
}

Logger &Logger::instance() {
    if (!global_logger) {
        throw std::runtime_error("Logger nao inicializado. Chame Logger::init(...) primeiro.");
    }
    return *global_logger;
}

void Logger::log(Level level, const std::string &message) {
    if (!pimpl) return;
    std::ostringstream oss;
    oss << '[' << now_timestamp() << ']'
        << '[' << level_to_string(level) << ']'
        << "[tid=" << std::this_thread::get_id() << "] "
        << message;
    pimpl->enqueue(oss.str());
}

void Logger::debug(const std::string &msg) { log(Level::DEBUG, msg); }
void Logger::info(const std::string &msg)  { log(Level::INFO, msg); }
void Logger::warn(const std::string &msg)  { log(Level::WARN, msg); }
void Logger::error(const std::string &msg) { log(Level::ERROR, msg); }

void Logger::shutdown() {
    std::lock_guard<std::mutex> lk(global_init_mtx);
    if (global_logger) {
        if (global_logger->pimpl) global_logger->pimpl->stop();
        delete global_logger;
        global_logger = nullptr;
    }
}

} // namespace tslog
