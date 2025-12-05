#include <iostream>
#include <string>
#include <windows.h>
#include <sstream>
#include <cctype>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <fstream>
#include <map>

using namespace std;

map<string, DWORD> pid_map;  // Маппинг имя -> PID

// Загружаем маппинг из файла
void load_pid_map() {
    ifstream pidFile("pid_map.txt");
    if (!pidFile.is_open()) {
        cout << "[killer] WARNING: Could not open pid_map.txt\n";
        return;
    }
    string name;
    DWORD pid;
    int count = 0;
    while (pidFile >> name >> pid) {
        pid_map[name] = pid;
        count++;
    }
    pidFile.close();
    cout << "[killer] Loaded " << count << " process mappings\n";
}

bool kill_pid(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) return false;

    // Try to terminate gracefully
    if (TerminateProcess(hProcess, 1)) {
        // Wait a bit to see if process exits
        for (int i = 0; i < 10; ++i) {
            DWORD exitCode;
            if (!GetExitCodeProcess(hProcess, &exitCode)) {
                CloseHandle(hProcess);
                return true;  // Process already terminated
            }
            if (exitCode != STILL_ACTIVE) {
                CloseHandle(hProcess);
                return true;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        CloseHandle(hProcess);
        return true;
    }

    CloseHandle(hProcess);
    return false;
}

int main(int argc, char** argv) {
    cout << "[killer] Starting with " << argc << " arguments\n";
    load_pid_map();  // Загружаем маппинг имя -> PID
    
    string nameArg;
    DWORD idArg = 0;

    for (int i = 1; i < argc; ++i) {
        string s = argv[i];
        if (s == "--id" && i + 1 < argc) {
            idArg = (DWORD)stoul(argv[++i]);
            cout << "[killer] Got --id " << idArg << "\n";
        } else if (s == "--name" && i + 1 < argc) {
            nameArg = argv[++i];
            cout << "[killer] Got --name " << nameArg << "\n";
        }
    }

    unordered_set<DWORD> seen;

    if (idArg != 0) {
        cout << "[killer] Trying to kill pid=" << idArg << "\n";
        if (kill_pid(idArg)) {
            cout << "killed pid=" << idArg << "\n";
        } else {
            cout << "failed pid=" << idArg << "\n";
        }
        seen.insert(idArg);
    }

    if (!nameArg.empty()) {
        cout << "[killer] Looking for name=" << nameArg << "\n";
        // Используем маппинг для поиска по имени
        if (pid_map.count(nameArg)) {
            DWORD pid = pid_map[nameArg];
            cout << "[killer] Found " << nameArg << " = " << pid << "\n";
            if (!seen.count(pid)) {
                if (kill_pid(pid)) {
                    cout << "killed pid=" << pid << " name=" << nameArg << "\n";
                } else {
                    cout << "failed pid=" << pid << "\n";
                }
                seen.insert(pid);
            }
        } else {
            cout << "none name=" << nameArg << "\n";
        }
    }

    cout << "[killer] Checking PROC_TO_KILL env variable\n";
    char envBuffer[1024] = {0};
    DWORD envSize = GetEnvironmentVariableA("PROC_TO_KILL", envBuffer, sizeof(envBuffer) - 1);
    cout << "[killer] PROC_TO_KILL size=" << envSize << "\n";
    if (envSize > 0 && envSize < sizeof(envBuffer)) {
        cout << "[killer] PROC_TO_KILL content: " << envBuffer << "\n";
        string s(envBuffer);
        stringstream ss(s);
        string token;
        while (getline(ss, token, ',')) {
            auto start = token.find_first_not_of(" \t\n\r\f\v\'");
            auto end = token.find_last_not_of(" \t\n\r\f\v\'");
            if (start == string::npos) continue;
            string pname = token.substr(start, end - start + 1);
            cout << "[killer] Processing env token: " << pname << "\n";
            
            // Используем маппинг для поиска по имени
            if (pid_map.count(pname)) {
                DWORD pid = pid_map[pname];
                cout << "[killer] Found env token " << pname << " = " << pid << "\n";
                if (!seen.count(pid)) {
                    if (kill_pid(pid)) {
                        cout << "killed pid=" << pid << " name=" << pname << "\n";
                    } else {
                        cout << "failed pid=" << pid << "\n";
                    }
                    seen.insert(pid);
                }
            } else {
                cout << "[killer] Not found in map: " << pname << "\n";
                cout << "none name=" << pname << "\n";
            }
        }
    }

    cout << "[killer] Done\n";
    return 0;
}
