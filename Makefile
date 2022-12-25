SRC = src
EXE = a.out

SOURCEFILES = $(shell find $(SRC) -type f -name *.cpp)

all:
	g++ $(SOURCEFILES) -o $(EXE)