#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char** argv) {
    string name = (argc > 1 ? string(argv[1]) : string("victim"));
    
    cout << "victim " << name << " pid=" << GetCurrentProcessId() << "\n";
    
    // Run until killed
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    return 0;
}
