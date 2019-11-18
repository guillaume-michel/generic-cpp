CXX=clang++-9
CXXFLAGS=-std=c++17 -Ofast -Wall -Werror

all: generic virtual

generic: generic.cpp Makefile
	$(CXX) $(CXXFLAGS) generic.cpp -o $@

virtual: virtual.cpp Makefile
	$(CXX) $(CXXFLAGS) virtual.cpp -o $@

clean:
	rm -rf *.o generic virtual
