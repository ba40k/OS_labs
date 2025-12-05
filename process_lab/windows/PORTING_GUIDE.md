# Windows Portирование - Объяснение изменений

## Обзор

Проект был портирован с Linux на Windows с заменой POSIX API на Windows API.

## Основные изменения по файлам

### killer.cpp

**Linux → Windows изменения:**

1. **Заголовки:**
   - Убрано: `<signal.h>`, `<sys/types.h>`, `<dirent.h>`, `<unistd.h>`
   - Добавлено: `<windows.h>`, `<tlhelp32.h>`

2. **Перечисление процессов (`pids_by_name`):**
   - **Linux**: Читает `/proc/<pid>/comm` файлы
   - **Windows**: Использует `CreateToolhelp32Snapshot()` и `Process32First/Next()`
   - Обработка `.exe` расширения в имене процесса

3. **Завершение процесса (`kill_pid`):**
   - **Linux**: Использует `kill(pid, SIGTERM)` и `kill(pid, SIGKILL)`
   - **Windows**: `OpenProcess()` + `TerminateProcess()`
   - Проверка через `GetExitCodeProcess()`

4. **Типы:**
   - `pid_t` → `DWORD` (Windows PID тип)
   - Функции Windows требуют ANSI строки, поэтому используется `SetEnvironmentVariableA()`

### user.cpp

**Linux → Windows изменения:**

1. **Заголовки:**
   - Убрано: `<sys/wait.h>`, `<signal.h>`, `<unistd.h>`, `<sys/stat.h>`
   - Добавлено: `<windows.h>`

2. **Проверка процесса (`process_exists`):**
   - **Linux**: Проверяет `/proc/<pid>` директорию через `stat()`
   - **Windows**: Использует `OpenProcess()` и `GetExitCodeProcess()`

3. **Ожидание завершения (`wait_for_exit`):**
   - **Linux**: Использует `waitpid()` с флагами
   - **Windows**: Проверяет через `GetExitCodeProcess()` в цикле

4. **Запуск процесса (`spawn_victim`):**
   - **Linux**: `fork()` + `execlp()`
   - **Windows**: `CreateProcessA()`
   - Нужно явно передать полное имя команды с аргументами

5. **Ожидание процесса:**
   - **Linux**: `waitpid()`
   - **Windows**: `WaitForSingleObject()`

6. **Переменные окружения:**
   - `setenv()` → `SetEnvironmentVariableA()`
   - `unsetenv()` → `SetEnvironmentVariableA(..., NULL)`

### victim.cpp

**Linux → Windows изменения:**

1. **Заголовки:**
   - Убрано: `<sys/prctl.h>`
   - Добавлено: `<windows.h>`

2. **Получение PID:**
   - `getpid()` → `GetCurrentProcessId()`

3. **Установка имени процесса:**
   - **Linux**: `prctl(PR_SET_NAME, name.c_str(), ...)` - устанавливает отображаемое имя
   - **Windows**: Такой функциональности нет встроенной. Имя процесса берется из исполняемого файла. Можно получить имя из argv[1], но это не повлияет на системное имя процесса.

### Системные различия

| Операция | Linux | Windows |
|----------|-------|---------|
| Создание процесса | fork() + exec* | CreateProcess() |
| Завершение | kill() сигнал | TerminateProcess() |
| Перечисление | /proc FS | CreateToolhelp32Snapshot |
| PID получение | getpid() | GetCurrentProcessId() |
| Синхронизация | wait/waitpid | WaitForSingleObject |
| Сон потока | usleep/sleep | Sleep() или std::this_thread::sleep_for |
| Переменные окружения | setenv/getenv/unsetenv | SetEnvironmentVariable/GetEnvironmentVariable |

## Совместимость

- **MinGW-w64**: ✓ Полная поддержка
- **MSVC**: ✓ Требует только подключение `<windows.h>`
- **Visual Studio**: ✓ Работает как консольное приложение
- **Clang (Windows)**: ✓ С поддержкой Windows API

## Сборка

Был добавлены три способа сборки:
1. `Makefile` - для mingw32-make
2. `build.bat` - для Command Prompt
3. `build.ps1` - для PowerShell

## Функциональность

Все основные функции сохранены:
- ✓ Запуск жертв процессов
- ✓ Убийство по PID
- ✓ Убийство по имени
- ✓ Поддержка `PROC_TO_KILL` переменной окружения
- ✓ Отчет о результатах

## Отличия в поведении

1. **Имена процессов**: На Windows имена берутся из процесс-листа, на Linux из `/proc/<pid>/comm`
2. **Дефолтные расширения**: Windows процессы могут иметь `.exe`, что обрабатывается в коде
3. **Сигналы**: Windows использует `TerminateProcess()` вместо SIGTERM/SIGKILL
4. **Права**: На Windows может потребоваться запуск от администратора для убийства процессов

## Тестирование

Рекомендуется запустить:
```
user.exe
```

Это запустит полный тест:
- Создание 10 процессов-жертв
- Убийство по PID и имени
- Убийство через переменную окружения
- Отчет о статусе каждого процесса
