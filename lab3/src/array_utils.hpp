#pragma once
#include <vector>
#include <string>

void initializeArray(std::vector<int>& arr, int size);
void resetMarkedElements(std::vector<int>& arr, const std::vector<int>& indices);
std::string arrayToString(const std::vector<int>& arr);
