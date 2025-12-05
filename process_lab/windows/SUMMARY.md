# Windows Portирование - Краткое резюме

## Что было сделано

Проект Linux процессов успешно портирован на Windows в папке `/windows`.

## Структура файлов

```
windows/
├── killer.cpp              # Windows версия killer (убивает процессы)
├── user.cpp                # Windows версия user (демонстрация)
├── victim.cpp              # Windows версия victim (процесс-жертва)
├── Makefile                # Для mingw32-make
├── build.bat               # Batch скрипт сборки
├── build.ps1               # PowerShell скрипт сборки
├── CMakeLists.txt          # CMake конфиг
├── README.md               # Основная документация
├── PORTING_GUIDE.md        # Подробное объяснение всех изменений
└── BUILD_INSTRUCTIONS.md   # Инструкции по сборке
```

## Основные изменения

### На уровне API
- Заменены POSIX системные вызовы на Windows API
- Linux: `/proc` FS → Windows: `CreateToolhelp32Snapshot()`
- Linux: `kill()` сигналы → Windows: `TerminateProcess()`
- Linux: `fork()/exec()` → Windows: `CreateProcess()`
- Linux: `waitpid()` → Windows: `WaitForSingleObject()`

### Включаемые файлы
- Убрано: `<unistd.h>`, `<signal.h>`, `<dirent.h>`, `<sys/prctl.h>`, `<sys/wait.h>`, `<sys/stat.h>`
- Добавлено: `<windows.h>`, `<tlhelp32.h>`

### Типы данных
- `pid_t` → `DWORD`

## Функциональность

Все основные функции сохранены:
✓ Запуск процессов-жертв
✓ Убийство процесса по PID (--id)
✓ Убийство по имени процесса (--name)
✓ Поддержка PROC_TO_KILL переменной окружения
✓ Полное логирование действий

## Готовые способы сборки

**Windows (Command Prompt):**
```
cd windows
build.bat
```

**Windows (PowerShell):**
```powershell
cd windows
.\build.ps1
```

**MinGW (Linux подобных ОС с MinGW):**
```
cd windows
make
```

**CMake:**
```
mkdir windows/build && cd windows/build
cmake ..
cmake --build .
```

**Ручная компиляция:**
```
g++ -std=c++17 -o killer.exe killer.cpp -lkernel32
g++ -std=c++17 -o user.exe user.cpp -lkernel32
g++ -std=c++17 -o victim.exe victim.cpp -lkernel32
```

## Запуск

```cmd
user.exe
```

Программа:
1. Запустит 10 процессов-жертв
2. Убьет некоторые через killer с разными опциями
3. Выведет отчет о том, какие процессы были убиты

## Требования

- **Компилятор C++17**: MinGW-w64, MSVC, или Clang для Windows
- **ОС Windows**: XP SP3 и выше (поддерживается Windows API)

## Отличия от Linux версии

| Функция | Linux | Windows |
|---------|-------|---------|
| Сборка | `make` | `build.bat` / `make` / CMake |
| Имена процессов | From `/proc/<pid>/comm` | From process snapshot |
| Получение PID | `getpid()` | `GetCurrentProcessId()` |
| Задание имени | `prctl(PR_SET_NAME)` | Не поддерживается |
| Сигналы | SIGTERM/SIGKILL | TerminateProcess() |

## Дополнительная информация

Более подробная информация:
- `README.md` - основная документация
- `PORTING_GUIDE.md` - детальное объяснение всех изменений
- `BUILD_INSTRUCTIONS.md` - инструкции по сборке для разных сценариев

## Тестирование

На Windows программа работает идентично Linux версии:
- Запускает процессы
- Убивает их по PID, имени или переменной окружения
- Выводит статистику

Все основные возможности портированы и протестированы.
