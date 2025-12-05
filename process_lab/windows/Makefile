CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -O2

# For Windows, we need to link against Windows libraries
LDFLAGS=-lkernel32

all: killer.exe user.exe victim.exe

killer.exe: killer.cpp
	$(CC) $(CFLAGS) -o $@ killer.cpp $(LDFLAGS)

user.exe: user.cpp
	$(CC) $(CFLAGS) -o $@ user.cpp $(LDFLAGS)

victim.exe: victim.cpp
	$(CC) $(CFLAGS) -o $@ victim.cpp $(LDFLAGS)

clean:
	del killer.exe user.exe victim.exe 2>nul || true

.PHONY: all clean
