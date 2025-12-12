#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

std::string runProcess(const std::string &cmd, const std::string &input) {
    // Создаем две трубы: одну для ввода, одну для вывода
    int stdinPipe[2];
    int stdoutPipe[2];
    
    if (pipe(stdinPipe) == -1) {
        throw std::runtime_error("pipe stdin failed");
    }
    if (pipe(stdoutPipe) == -1) {
        close(stdinPipe[0]);
        close(stdinPipe[1]);
        throw std::runtime_error("pipe stdout failed");
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(stdinPipe[0]);
        close(stdinPipe[1]);
        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        throw std::runtime_error("fork failed");
    }

    if (pid == 0) {
        // Дочерний процесс
        // Закрываем ненужные концы труб
        close(stdinPipe[1]);   // закрываем запись в stdin трубу
        close(stdoutPipe[0]);  // закрываем чтение из stdout трубы

        // Перенаправляем stdin и stdout
        dup2(stdinPipe[0], STDIN_FILENO);
        dup2(stdoutPipe[1], STDOUT_FILENO);

        // Закрываем оригинальные файловые дескрипторы труб
        close(stdinPipe[0]);
        close(stdoutPipe[1]);

        // Выполняем команду
        execlp(cmd.c_str(), cmd.c_str(), (char *)NULL);
        // Если execlp вернулся, произошла ошибка
        std::cerr << "exec failed for " << cmd << std::endl;
        exit(1);
    }

    // Родительский процесс
    // Закрываем ненужные концы труб
    close(stdinPipe[0]);   // закрываем чтение из stdin трубы
    close(stdoutPipe[1]);  // закрываем запись в stdout трубу

    // Пишем входные данные в процесс
    if (!input.empty()) {
        ssize_t written = write(stdinPipe[1], input.c_str(), input.size());
        if (written != (ssize_t)input.size()) {
            std::cerr << "Warning: wrote " << written << " of " << input.size() << " bytes to child\n";
        }
    }
    close(stdinPipe[1]); // закрываем трубу для записи, чтобы дочерний процесс знал об EOF

    // Читаем вывод из процесса
    std::string output;
    const size_t bufSize = 4096;
    char buffer[bufSize];
    ssize_t bytesRead;
    while ((bytesRead = read(stdoutPipe[0], buffer, bufSize)) > 0) {
        output.append(buffer, buffer + bytesRead);
    }
    close(stdoutPipe[0]);

    // Ждем завершения дочернего процесса
    int status;
    waitpid(pid, &status, 0);

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

        std::string outM = runProcess("./M", input);
        std::cout << "After M: " << trimNewline(outM) << "\n";

        std::string outA = runProcess("./A", outM);
        std::cout << "After A: " << trimNewline(outA) << "\n";

        std::string outP = runProcess("./P", outA);
        std::cout << "After P: " << trimNewline(outP) << "\n";

        std::string outS = runProcess("./S", outP);
        std::cout << "After S (sum): " << trimNewline(outS) << "\n";
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
