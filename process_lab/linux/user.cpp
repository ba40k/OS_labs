#include <bits/stdc++.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

bool proc_dir_exists(pid_t pid) {
    string path = "/proc/" + to_string(pid);
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}

bool wait_for_exit(pid_t pid, int timeout_ms=3000) {
    int waited = 0;
    while (waited < timeout_ms) {
        if (!proc_dir_exists(pid)) return true;
        this_thread::sleep_for(chrono::milliseconds(100));
        waited += 100;
    }
    return !proc_dir_exists(pid);
}

void reap_child(pid_t pid) {
    int status = 0;
    pid_t r = waitpid(pid, &status, WNOHANG);
    if (r == 0) {
        for (int i = 0; i < 30; ++i) {
            r = waitpid(pid, &status, WNOHANG);
            if (r == pid || r == -1) break;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

int main() {
    string procList = "victim1,victim2,victim3";
    setenv("PROC_TO_KILL", procList.c_str(), 1);
    cout << "env PROC_TO_KILL=" << procList << "\n";

    const int N = 10;
    vector<pid_t> victims(N);
    vector<string> vnames(N);
    unordered_set<pid_t> killed;

    for (int i = 0; i < N; ++i) {
        string name = string("victim") + to_string(i+1);
        pid_t pid = fork();
        if (pid == 0) {
            execlp("./victim", name.c_str(), name.c_str(), (char*)NULL);
            _exit(1);
        } else if (pid > 0) {
            victims[i] = pid;
            vnames[i] = name;
            cout << "started " << name << " pid=" << pid << "\n";
        } else {
            perror("fork");
        }
    }

    this_thread::sleep_for(chrono::seconds(1));

    pid_t id4 = victims[3];
    cout << "run: killer --name victim5 --id " << id4 << " (and env list always processed)\n";
    if (fork() == 0) {
        string sid = to_string(id4);
        execlp("./killer", "killer", "--name", "victim5", "--id", sid.c_str(), (char*)NULL);
        _exit(1);
    } else waitpid(-1, NULL, 0);

    for (auto p: victims) { wait_for_exit(p, 2000); reap_child(p); if (!proc_dir_exists(p)) killed.insert(p); }

    cout << "\nSummary (victim, pid, status):\n";
    for (int i = 0; i < N; ++i) {
        cout << vnames[i] << ": pid=" << victims[i] << " -> " << (killed.count(victims[i]) ? "killed" : "alive") << "\n";
    }

    unsetenv("PROC_TO_KILL");
    cout << "env cleared\n";
    return 0;
}
