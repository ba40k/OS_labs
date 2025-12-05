#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <cctype>
#include <unordered_set>
#include <thread>
#include <chrono>

using namespace std;

vector<DWORD> pids_by_name(const string &name) {
    vector<DWORD> result;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return result;

    if (Process32First(snapshot, &entry)) {
        do {
            // Convert wide string to regular string
            string procName;
            for (int i = 0; entry.szExeFile[i] != '\0'; ++i) {
                procName += static_cast<char>(entry.szExeFile[i]);
            }
            
            // Remove .exe extension if present
            if (procName.length() >= 4) {
                string ext = procName.substr(procName.length() - 4);
                // Convert to lowercase for comparison
                for (auto &c : ext) c = tolower(c);
                if (ext == ".exe") {
                    procName = procName.substr(0, procName.length() - 4);
                }
            }

            if (procName == name) {
                result.push_back(entry.th32ProcessID);
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return result;
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
    string nameArg;
    DWORD idArg = 0;

    for (int i = 1; i < argc; ++i) {
        string s = argv[i];
        if (s == "--id" && i + 1 < argc) {
            idArg = (DWORD)stoul(argv[++i]);
        } else if (s == "--name" && i + 1 < argc) {
            nameArg = argv[++i];
        }
    }

    unordered_set<DWORD> seen;

    if (idArg != 0) {
        if (kill_pid(idArg)) {
            cout << "killed pid=" << idArg << "\n";
        } else {
            cout << "failed pid=" << idArg << "\n";
        }
        seen.insert(idArg);
    }

    if (!nameArg.empty()) {
        auto pids = pids_by_name(nameArg);
        if (pids.empty()) {
            cout << "none name=" << nameArg << "\n";
        } else {
            for (auto p : pids) {
                if (seen.count(p)) continue;
                if (kill_pid(p)) cout << "killed pid=" << p << " name=" << nameArg << "\n";
                else cout << "failed pid=" << p << "\n";
                seen.insert(p);
            }
        }
    }

    const char* env = getenv("PROC_TO_KILL");
    if (env) {
        string s(env);
        stringstream ss(s);
        string token;
        while (getline(ss, token, ',')) {
            auto start = token.find_first_not_of(" \t\n\r\f\v\'");
            auto end = token.find_last_not_of(" \t\n\r\f\v\'");
            if (start == string::npos) continue;
            string pname = token.substr(start, end - start + 1);
            auto pids = pids_by_name(pname);
            if (pids.empty()) {
                cout << "none name=" << pname << "\n";
            } else {
                for (auto p : pids) {
                    if (seen.count(p)) continue;
                    if (kill_pid(p)) cout << "killed pid=" << p << " name=" << pname << "\n";
                    else cout << "failed pid=" << p << "\n";
                    seen.insert(p);
                }
            }
        }
    }

    return 0;
}
