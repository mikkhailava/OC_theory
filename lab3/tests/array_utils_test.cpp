#include "gtest/gtest.h"
#include "array_utils.hpp"
#include <vector>
#include <string>

TEST(ArrayUtilsTest, InitializeArray) {
    std::vector<int> arr;
    initializeArray(arr, 10);
    EXPECT_EQ(arr.size(), 10);
    for (int x : arr) {
    EXPECT_EQ(x, 0);
}
}

TEST(ArrayUtilsTest, ResetMarkedElements) {
    std::vector<int> arr = {1, 2, 3, 4, 5};
    std::vector<int> indices = {1, 3};
    resetMarkedElements(arr, indices);
    EXPECT_EQ(arr[1], 0);
    EXPECT_EQ(arr[3], 0);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[4], 5);
}

TEST(ArrayUtilsTest, ArrayToString) {
    std::vector<int> arr = {0, 1, 2};
    std::string s = arrayToString(arr);
    EXPECT_EQ(s, "[ 0 1 2 ]");
}
