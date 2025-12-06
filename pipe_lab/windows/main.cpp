#include <windows.h>
#include <iostream>
#include <vector>
#include <string>


void setNonInheritable(HANDLE h) {
    if (h && h != INVALID_HANDLE_VALUE) {
        SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0);
    }
}


void setInheritable(HANDLE h) {
    if (h && h != INVALID_HANDLE_VALUE) {
        SetHandleInformation(h, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    }
}

bool createChild(const std::string &cmd, HANDLE hIn, HANDLE hOut, PROCESS_INFORMATION &pi) {
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    
    std::vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back('\0');

    ZeroMemory(&pi, sizeof(pi));
    BOOL ok = CreateProcessA(NULL, cmdBuf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (!ok) {
        std::cerr << "CreateProcess failed for " << cmd << " (err=" << GetLastError() << ")\n";
        return false;
    }
    return true;
}


std::string runProcess(const std::string &cmd, const std::string &input) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    HANDLE childStdinRd = NULL, childStdinWr = NULL;
    HANDLE childStdoutRd = NULL, childStdoutWr = NULL;

    if (!CreatePipe(&childStdinRd, &childStdinWr, &sa, 0)) {
        throw std::runtime_error("CreatePipe stdin failed");
    }
    if (!CreatePipe(&childStdoutRd, &childStdoutWr, &sa, 0)) {
        CloseHandle(childStdinRd); CloseHandle(childStdinWr);
        throw std::runtime_error("CreatePipe stdout failed");
    }

    
    setNonInheritable(childStdinWr);
    setNonInheritable(childStdoutRd);

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = childStdinRd;
    si.hStdOutput = childStdoutWr;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back('\0');

    BOOL ok = CreateProcessA(NULL, cmdBuf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (!ok) {
        CloseHandle(childStdinRd); CloseHandle(childStdinWr);
        CloseHandle(childStdoutRd); CloseHandle(childStdoutWr);
        throw std::runtime_error("CreateProcess failed for " + cmd);
    }

    
    CloseHandle(childStdinRd);
    CloseHandle(childStdoutWr);

    
    DWORD written = 0;
    if (!input.empty()) {
        BOOL wok = WriteFile(childStdinWr, input.c_str(), (DWORD)input.size(), &written, NULL);
        if (!wok) {
            std::cerr << "WriteFile to child failed (err=" << GetLastError() << ")\n";
        } else if (written != input.size()) {
            std::cerr << "Warning: wrote " << written << " of " << input.size() << " bytes to child\n";
        }
    }
    CloseHandle(childStdinWr); // 


    std::string output;
    const DWORD bufSize = 4096;
    char buffer[bufSize];
    DWORD read = 0;
    while (true) {
        BOOL rok = ReadFile(childStdoutRd, buffer, bufSize, &read, NULL);
        if (!rok || read == 0) break;
        output.append(buffer, buffer + read);
    }
    CloseHandle(childStdoutRd);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess); CloseHandle(pi.hThread);

    return output;
}

static std::string trimNewline(const std::string &s) {
    if (s.empty()) return s;
    std::string r = s;
    while (!r.empty() && (r.back() == '\n' || r.back() == '\r')) r.pop_back();
    return r;
}

int main() {
    try {
        std::string line;
        std::cout << "Enter numbers separated by spaces and press Enter: ";
        if (!std::getline(std::cin, line)) return 0;
        std::string input = line;
        if (input.empty()) {
            std::cout << "No input provided.\n";
            return 0;
        }
        if (input.back() != '\n') input.push_back('\n');
        std::cout << "Input: " << trimNewline(input) << "\n";

        std::string outM = runProcess("M.exe", input);
        std::cout << "After M: " << trimNewline(outM) << "\n";

        std::string outA = runProcess("A.exe", outM);
        std::cout << "After A: " << trimNewline(outA) << "\n";

        std::string outP = runProcess("P.exe", outA);
        std::cout << "After P: " << trimNewline(outP) << "\n";

        std::string outS = runProcess("S.exe", outP);
        std::cout << "After S (sum): " << trimNewline(outS) << "\n";
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
