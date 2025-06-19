#include "singleton.hpp"
#include <thread>

class Logger {
public:
    void Log(const std::string& msg) {
        std::cout << "[LOG]: " << msg << std::endl;
    }
private:
    friend class CreateUsingNew<Logger>;
    Logger() = default;
};

using ThreadSafeLogger = Singleton<Logger, CreateUsingNew, MultiThreaded, NoDestroy>;

void ThreadFunc(int id) {
    auto& logger = ThreadSafeLogger::Instance();
    logger.Log("Поток #" + std::to_string(id));
}

int main() {
    std::thread t1(ThreadFunc, 1);
    std::thread t2(ThreadFunc, 2);
    t1.join();
    t2.join();
    return 0;
}
