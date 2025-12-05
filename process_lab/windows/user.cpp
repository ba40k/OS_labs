#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <map>
#include <fstream>

using namespace std;

map<DWORD, HANDLE> process_handles;
map<string, DWORD> name_to_pid;  // Маппинг имя -> PID

bool process_exists(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess == NULL) return false;
    
    DWORD exitCode;
    BOOL success = GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);
    
    // Если процесс не активен или мы не можем получить его статус - он мертв
    if (!success) return false;
    return (exitCode == STILL_ACTIVE);
}

bool wait_for_exit(DWORD pid, int timeout_ms = 3000) {
    int waited = 0;
    while (waited < timeout_ms) {
        if (!process_exists(pid)) {
            // Процесс умер
            return true;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        waited += 100;
    }
    // По истечении таймаута вернуть текущее состояние
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
    // Сохраняем дескриптор процесса для отслеживания
    process_handles[pi.dwProcessId] = pi.hProcess;
    return pi.dwProcessId;
}

int main() {
    string procList = "victim1,victim2,victim3";
    SetEnvironmentVariableA("PROC_TO_KILL", procList.c_str());
    // Сохраняем маппинг в файл для killer'а
    ofstream pidFile("pid_map.txt");
    
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
            name_to_pid[name] = pid;
            pidFile << name << " " << pid << "\n";
            cout << "started " << name << " pid=" << pid << "\n";
        } else {
            victims[i] = 0;
            vnames[i] = name;
        }
    }
    pidFile.close();

    this_thread::sleep_for(chrono::seconds(1));

    DWORD id4 = victims[3];
    cout << "run: killer --name victim5 --id " << id4 << " (and env list always processed)\n";
    
    STARTUPINFOA si2 = {0};
    PROCESS_INFORMATION pi2 = {0};
    si2.cb = sizeof(si2);

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
            &si2,
            &pi2)) {
        WaitForSingleObject(pi2.hProcess, INFINITE);
        CloseHandle(pi2.hThread);
        CloseHandle(pi2.hProcess);
    }

    // Даем время killer'у завершиться
    this_thread::sleep_for(chrono::milliseconds(500));
    
    // Проверяем которые процессы еще живы
    for (auto p : victims) {
        if (p > 0 && process_exists(p)) {
            // Процесс еще жив - это значит он НЕ был убит
            // (не добавляем в killed set)
        } else if (p > 0 && !process_exists(p)) {
            // Процесс мертв - killer его убил
            killed.insert(p);
        }
    }
    
    // Закрываем дескрипторы
    for (auto &item : process_handles) {
        CloseHandle(item.second);
    }
    process_handles.clear();

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
