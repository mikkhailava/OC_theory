#include "marker.hpp"
#include "array_utils.hpp"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>

extern std::vector<int> g_array;
extern std::mutex g_arrMutex;

extern std::mutex g_waitMutex;
extern std::condition_variable g_cvMain;
extern int g_blockedCount;

extern std::vector<MarkerContext*> g_markerContexts;

void markerThreadFunction(int id, int arraySize) {
    std::srand(id);
    std::vector<int> markedIndices;
    MarkerContext* ctx = g_markerContexts[id];

    while (true) {
        int r = std::rand();
        int index = r % arraySize;

        {
            std::lock_guard<std::mutex> lock(g_arrMutex);
            if (g_array[index] == 0) {
                std::cout << "Marker " << id << " пометил индекс " << index << std::endl;
            } else {
                int markedCount = static_cast<int>(markedIndices.size());
                std::cout << "Marker " << id << " не смог пометить " << index << " (конфликт)\n";

                {
                    std::unique_lock<std::mutex> waitLock(g_waitMutex);
                    g_blockedCount++;
                    std::cout << "g_blockedCount увеличен: " << g_blockedCount << std::endl;
                    g_cvMain.notify_one();
                }
                {
                    std::unique_lock<std::mutex> lock(ctx->mtx);
                    std::cout << "Marker " << id << " ждет сигнала..." << std::endl;
                    ctx->cv.wait(lock, [ctx] { return ctx->resume || ctx->terminate; });
                    std::cout << "Marker " << id << " продолжил работу!" << std::endl;
                }
                if (ctx->terminate)
                    break;

                {
                    std::lock_guard<std::mutex> lock(ctx->mtx);
                    ctx->resume = false;
                }
                continue;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        {
            std::lock_guard<std::mutex> lock(g_arrMutex);
            if (g_array[index] == 0) {
                g_array[index] = id;
                markedIndices.push_back(index);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    resetMarkedElements(g_array, markedIndices);
    std::cout << "Marker " << id << " завершает работу." << std::endl;
}
