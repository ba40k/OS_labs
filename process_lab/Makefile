CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -O2

all: killer user victim

killer: killer.cpp
	$(CC) $(CFLAGS) -o $@ killer.cpp

user: user.cpp
	$(CC) $(CFLAGS) -o $@ user.cpp

victim: victim.cpp
	$(CC) $(CFLAGS) -o $@ victim.cpp

clean:
	rm -f killer user victim
