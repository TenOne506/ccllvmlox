#include <iostream>
#include <vector>

void printVector(const std::vector<int>& vec) {
    for (int iter: vec) { std::cout << iter << std::endl; }
}

auto main() -> int {
    std::vector<int> myVec = {1, 2, 3, 4};
    printVector(myVec);
    return 0;
}
