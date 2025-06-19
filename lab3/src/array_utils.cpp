#include "array_utils.hpp"
#include <sstream>
#include <mutex>
#include <iostream>

std::mutex g_arrMutex;

void initializeArray(std::vector<int>& arr, int size) {
    arr.resize(size, 0);
}

void resetMarkedElements(std::vector<int>& arr, const std::vector<int>& indices) {
    std::lock_guard<std::mutex> lock(g_arrMutex);
    for (int idx : indices) {
        if (idx >= 0 && idx < static_cast<int>(arr.size()))
            arr[idx] = 0;
    }
}

std::string arrayToString(const std::vector<int>& arr) {
    std::ostringstream oss;
    oss << "[ ";
    for (int x : arr)
        oss << x << " ";
    oss << "]";
    return oss.str();
}
