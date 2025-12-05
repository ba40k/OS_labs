#include <bits/stdc++.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

vector<pid_t> pids_by_name(const string &name) {
    vector<pid_t> result;
    DIR *dir = opendir("/proc");
    if (!dir) return result;
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (!isdigit(entry->d_name[0])) continue;
        string pidstr = entry->d_name;
        string cmdpath = string("/proc/") + pidstr + "/comm";
        ifstream f(cmdpath);
        if (!f.is_open()) continue;
        string cmd;
        getline(f, cmd);
        if (cmd == name) {
            result.push_back((pid_t)stoi(pidstr));
        }
    }
    closedir(dir);
    return result;
}

bool kill_pid(pid_t pid) {
    if (kill(pid, SIGTERM) != 0) return false;
    for (int i = 0; i < 10; ++i) {
        if (kill(pid, 0) == -1) return true;
        usleep(100000);
    }
    if (kill(pid, SIGKILL) == 0) return true;
    return false;
}

int main(int argc, char** argv) {
    string nameArg;
    pid_t idArg = 0;
    for (int i = 1; i < argc; ++i) {
        string s = argv[i];
        if (s == "--id" && i+1 < argc) {
            idArg = (pid_t)stol(argv[++i]);
        } else if (s == "--name" && i+1 < argc) {
            nameArg = argv[++i];
        }
    }

    unordered_set<pid_t> seen;

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
            for (auto p: pids) {
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
            if (start==string::npos) continue;
            string pname = token.substr(start, end-start+1);
            auto pids = pids_by_name(pname);
            if (pids.empty()) {
                cout << "none name=" << pname << "\n";
            } else {
                for (auto p: pids) {
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
