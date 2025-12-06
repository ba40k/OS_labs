#include <iostream>
#include <string>
#include <sstream>

int main() {
    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    std::istringstream iss(line);
    long long x;
    bool first = true;
    while (iss >> x) {
        long long y = x * 7LL;
        if (!first) std::cout << ' ';
        std::cout << y;
        first = false;
    }
    std::cout << std::endl;
    return 0;
}
