CXX		  := g++
CXX_FLAGS := -std=c++17 -Ofast #-Wall -Wextra -ggdb 

BIN		:= bin
SRC		:= src

LIBRARIES	:= -lopengl32 -lglew32 -lfreeglut -lglu32 -I C:\\mingw64\\x86_64-w64-mingw32\\include -L C:\\mingw64\\x86_64-w64-mingw32\\lib
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LIBRARIES)

clean:
	mkdir -p $(BIN)
	-rm $(BIN)/* || true
