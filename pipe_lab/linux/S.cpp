#include <iostream>
#include <string>
#include <sstream>

int main() {
    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    std::istringstream iss(line);
    long long x;
    long long sum = 0;
    while (iss >> x) {
        sum += x;
    }
    std::cout << sum << std::endl;
    return 0;
}
