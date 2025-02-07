#include "Lox/Lox.h"
#include <iostream>
#include <vector>

void printVector(const std::vector<int>& vec) {
    for (int iter: vec) { std::cout << iter << std::endl; }
}

auto main() -> int {
    Lox::runPrompt();
    return 0;
}
