#include <bits/stdc++.h>
#include <sys/prctl.h>
using namespace std;
int main(int argc, char** argv) {
    string name = (argc>1 ? string(argv[1]) : string("victim"));
    // set the process name (affects /proc/<pid>/comm)
    prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
    cout << "victim " << name << " pid=" << getpid() << "\n";
    // run until killed
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}
