#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

bool process_exists(DWORD pid) {
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (hProcess == NULL) return false;
    
    DWORD exitCode;
    BOOL success = GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);
    
    return success && (exitCode == STILL_ACTIVE);
}

bool wait_for_exit(DWORD pid, int timeout_ms = 3000) {
    int waited = 0;
    while (waited < timeout_ms) {
        if (!process_exists(pid)) return true;
        this_thread::sleep_for(chrono::milliseconds(100));
        waited += 100;
    }
    return !process_exists(pid);
}

DWORD spawn_victim(const string &name) {
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    string cmdLine = string("victim.exe ") + name;
    
    if (!CreateProcessA(
            NULL,
            (LPSTR)cmdLine.c_str(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
        cerr << "CreateProcess failed for " << name << endl;
        return 0;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return pi.dwProcessId;
}

int main() {
    string procList = "victim1,victim2,victim3";
    SetEnvironmentVariableA("PROC_TO_KILL", procList.c_str());
    cout << "env PROC_TO_KILL=" << procList << "\n";

    const int N = 10;
    vector<DWORD> victims(N);
    vector<string> vnames(N);
    unordered_set<DWORD> killed;

    for (int i = 0; i < N; ++i) {
        string name = string("victim") + to_string(i + 1);
        DWORD pid = spawn_victim(name);
        if (pid > 0) {
            victims[i] = pid;
            vnames[i] = name;
            cout << "started " << name << " pid=" << pid << "\n";
        } else {
            victims[i] = 0;
            vnames[i] = name;
        }
    }

    this_thread::sleep_for(chrono::seconds(1));

    DWORD id4 = victims[3];
    cout << "run: killer --name victim5 --id " << id4 << " (and env list always processed)\n";
    
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    string cmdLine = string("killer.exe --name victim5 --id ") + to_string(id4);
    
    if (CreateProcessA(
            NULL,
            (LPSTR)cmdLine.c_str(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    for (auto p : victims) {
        if (p > 0) {
            wait_for_exit(p, 2000);
            if (!process_exists(p)) {
                killed.insert(p);
            }
        }
    }

    cout << "\nSummary (victim, pid, status):\n";
    for (int i = 0; i < N; ++i) {
        if (victims[i] > 0) {
            cout << vnames[i] << ": pid=" << victims[i] << " -> "
                 << (killed.count(victims[i]) ? "killed" : "alive") << "\n";
        }
    }

    SetEnvironmentVariableA("PROC_TO_KILL", NULL);
    cout << "env cleared\n";
    return 0;
}
