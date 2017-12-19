CXX = clang++
LD = clang++ 

CXXFLAGS = -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -pedantic
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi
EXE1 = maze_search
OBJ1 = maze.o

all: $(EXE1)

$(EXE1): $(OBJ1)
	$(LD) $(LDFLAGS) -o $@ $<

%.o : %.c
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -rf *.o $(EXE1) $(EXE2)