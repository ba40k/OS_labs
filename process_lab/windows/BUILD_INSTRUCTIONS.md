# Инструкции по сборке Windows версии

## Требования

Для компиляции нужен один из следующих компиляторов:
- **MinGW-w64** (рекомендуется)
- **MSVC** (Visual Studio)
- **Clang для Windows**

## Способ 1: Используя Batch файл (Command Prompt)

Самый простой способ:

```cmd
cd windows
build.bat
```

## Способ 2: Используя PowerShell

```powershell
cd windows
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
.\build.ps1
```

## Способ 3: Используя Make (MinGW)

Если установлен mingw32-make:

```bash
cd windows
make
```

Очистка:
```bash
make clean
```

## Способ 4: Используя CMake

Если установлен CMake:

```bash
cd windows
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
# или для Visual Studio:
# cmake .. -G "Visual Studio 16 2019"
cmake --build .
```

## Способ 5: Прямая компиляция g++

```bash
cd windows

# Компилируем killer
g++ -std=c++17 -Wall -Wextra -O2 -o killer.exe killer.cpp -lkernel32

# Компилируем user
g++ -std=c++17 -Wall -Wextra -O2 -o user.exe user.cpp -lkernel32

# Компилируем victim
g++ -std=c++17 -Wall -Wextra -O2 -o victim.exe victim.cpp -lkernel32
```

## Способ 6: Используя MSVC (Visual Studio)

```bash
cd windows

# Компилируем killer
cl /std:c++17 /O2 /W4 killer.cpp

# Компилируем user
cl /std:c++17 /O2 /W4 user.cpp

# Компилируем victim
cl /std:c++17 /O2 /W4 victim.cpp
```

## Запуск программ

После сборки в директории `windows` появятся три исполняемых файла:
- `killer.exe`
- `user.exe`
- `victim.exe`

Для запуска основной демонстрации:

```cmd
user.exe
```

Для использования killer напрямую:

```cmd
# Убить процесс по PID
killer.exe --id 1234

# Убить процесс по имени
killer.exe --name victim

# Убить через переменную окружения
set PROC_TO_KILL=victim1,victim2,victim3
killer.exe
```

## Установка компилятора

### MinGW-w64 (рекомендуется)

1. Скачайте с https://www.mingw-w64.org/
2. Распакуйте в удобное место (например `C:\mingw64`)
3. Добавьте `C:\mingw64\bin` в PATH
4. Проверьте: `g++ --version`

### Visual Studio Build Tools

1. Скачайте https://visualstudio.microsoft.com/visual-cpp-build-tools/
2. Установите
3. Откройте "Visual Studio Developer Command Prompt"
4. Используйте `cl` компилятор

### Установка Make (опционально)

Для использования Makefile:
```bash
# Если не установлен:
choco install make  # для Chocolatey
# или скачайте mingw32-make отдельно
```

## Проверка установки

```bash
# Проверка g++
g++ --version

# Проверка наличия Windows заголовков
echo #include ^<windows.h^> | g++ -x c++ -E -

# Проверка make
make --version

# Проверка cmake
cmake --version
```

## Диагностика

### Ошибка: "g++: command not found"
- Установите MinGW-w64 или MSVC
- Добавьте компилятор в PATH

### Ошибка: "windows.h: No such file or directory"
- Используйте MinGW-w64 вместо MinGW
- Проверьте установку MSVC

### Ошибка: "undefined reference to CreateProcess"
- Убедитесь что в компилировании используется `-lkernel32` флаг

### Ошибка: "Permission denied при убийстве процесса"
- Запустите Command Prompt как администратор
- Или запустите программу с правами администратора

## Рекомендации

1. Используйте `build.bat` или `build.ps1` для простоты
2. Если нужна более сложная сборка, используйте CMake
3. Для разработки рекомендуется MinGW-w64 (он бесплатный и простой)
4. MSVC можно использовать, если уже установлена Visual Studio

## Дополнительно

- В папке уже есть готовые скрипты сборки
- Все программы скомпилированы статически (без внешних зависимостей)
- Размер каждого исполняемого файла примерно 100-200 KB
