EXE = trifocal
LDFLAGS = -lSDL 
CFLAGS=-Wall -g -I include
GCC = g++

SRC_PATH = src
BIN_PATH = bin

SRC_FILES = $(shell find $(SRC_PATH) -type f -name '*.cpp')
OBJ_FILES = $(patsubst $(SRC_PATH)/%.cpp, $(SRC_PATH)/%.o, $(SRC_FILES))

all: $(BIN_PATH)/$(EXE)

$(BIN_PATH)/$(EXE): $(OBJ_FILES)
	$(GCC) -o $@ $^ $(LDFLAGS)

$(SRC_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(GCC) -c -o $@ $(CFLAGS) $^ 

clean:
	rm $(OBJ_FILES)

cleanall:
	rm $(BIN_PATH)/$(EXE) $(OBJ_FILES)

