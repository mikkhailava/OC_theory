#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>

struct MarkerContext {
    std::mutex mtx;
    std::condition_variable cv;
    bool resume = false;
    bool terminate = false;
};

void markerThreadFunction(int id, int arraySize);
