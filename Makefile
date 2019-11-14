CXX=clang++-8
CXXFLAGS=-std=c++17 -Ofast -Wall -Werror

all: generic

generic: generic.cpp Makefile
	$(CXX) $(CXXFLAGS) generic.cpp -o $@

clean:
	rm -rf *.o generic
