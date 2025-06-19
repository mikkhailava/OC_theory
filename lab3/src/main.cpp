#include "array_utils.hpp"
#include "marker.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <future>
#include <utility>

std::vector<int> g_array;

extern std::mutex g_arrMutex;
std::mutex g_waitMutex;
std::condition_variable g_cvMain;
int g_blockedCount = 0;
std::vector<MarkerContext*> g_markerContexts;

struct MarkerThreadWrapper {
    int id;
    int arraySize;
    MarkerThreadWrapper(int _id, int _arraySize)
            : id(_id), arraySize(_arraySize) { }
    void operator()() const {
        markerThreadFunction(id, arraySize);
    }
};

int getTerminateId(int markerCount) {
    std::cout << "Введите номер marker-потока для завершения (0-"
              << markerCount - 1 << "): " << std::flush;
    auto fut = std::async(std::launch::async, []() -> int {
        int id;
        std::cin >> id;
        return id;
    });
    if (fut.wait_for(std::chrono::seconds(5)) == std::future_status::ready) {
        return fut.get();
    } else {
        std::cout << "\nТаймаут ввода. Будет автоматически выбран поток 0 для завершения.\n";
        return 0;
    }
}

void runApplication() {
    int arraySize = 0;
    std::cout << "Введите размер массива: ";
    std::cin >> arraySize;
    initializeArray(g_array, arraySize);

    int markerCount = 0;
    std::cout << "Введите количество потоков marker: ";
    std::cin >> markerCount;
    if (markerCount <= 0) {
        std::cerr << "Некорректное количество marker потоков.\n";
        return;
    }

    // Инициализируем контексты для marker-потоков
    g_markerContexts.resize(markerCount, nullptr);
    for (int i = 0; i < markerCount; ++i) {
        g_markerContexts[i] = new MarkerContext();
    }

    // Запускаем marker-потоки
    std::vector<std::thread> markerThreads;
    for (int i = 0; i < markerCount; ++i) {
        markerThreads.emplace_back(MarkerThreadWrapper(i, arraySize));
    }

    std::cout << "Запущены marker-потоки. Начинается работа.\n";

    int activeThreads = markerCount;
    while (activeThreads > 0) {
        {
            std::unique_lock<std::mutex> lock(g_waitMutex);
            // Ожидаем появления хотя бы одного блокирующего сигнала или таймаута
            g_cvMain.wait_for(lock, std::chrono::seconds(5), [](){ return g_blockedCount > 0; });
        }

        {
            std::lock_guard<std::mutex> lock(g_arrMutex);
            std::cout << "Состояние массива: " << arrayToString(g_array) << "\n";
        }

        int terminateId = getTerminateId(markerCount);
        if (terminateId < 0 || terminateId >= markerCount) {
            std::cout << "Неверный номер. Будет выбран поток 0 по умолчанию.\n";
            terminateId = 0;
        }
        {
            std::lock_guard<std::mutex> lock(g_markerContexts[terminateId]->mtx);
            g_markerContexts[terminateId]->terminate = true;
            g_markerContexts[terminateId]->resume = true;
        }
        // Пробуждаем выбранный поток, чтобы он завершился
        g_markerContexts[terminateId]->cv.notify_one();
        if (markerThreads[terminateId].joinable())
            markerThreads[terminateId].join();
        std::cout << "Marker " << terminateId << " завершён.\n";
        activeThreads--;

        // Сбрасываем счетчик блокировок
        {
            std::lock_guard<std::mutex> lock(g_waitMutex);
            g_blockedCount = 0;
        }
        g_cvMain.notify_all();

        // Пробуждаем остальные потоки для продолжения работы
        for (int i = 0; i < markerCount; ++i) {
            if (i == terminateId)
                continue;
            {
                std::lock_guard<std::mutex> lock(g_markerContexts[i]->mtx);
                g_markerContexts[i]->resume = true;
            }
            g_markerContexts[i]->cv.notify_one();
        }
        {
            std::lock_guard<std::mutex> lock(g_arrMutex);
            std::cout << "Состояние массива после возобновления: " << arrayToString(g_array) << "\n";
        }
    }

    // Освобождаем выделенные контексты
    for (size_t i = 0; i < g_markerContexts.size(); ++i) {
        delete g_markerContexts[i];
    }

    std::cout << "Работа main завершена, все marker-потоки завершены.\n";
}

#ifndef UNIT_TEST
int main() {
    runApplication();
    return 0;
}
#endif