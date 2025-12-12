CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGETS = Main M A P S

all: $(TARGETS)

Main: main.cpp
	$(CXX) $(CXXFLAGS) -o Main main.cpp

M: M.cpp
	$(CXX) $(CXXFLAGS) -o M M.cpp

A: A.cpp
	$(CXX) $(CXXFLAGS) -o A A.cpp

P: P.cpp
	$(CXX) $(CXXFLAGS) -o P P.cpp

S: S.cpp
	$(CXX) $(CXXFLAGS) -o S S.cpp

run: all
	./Main

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
