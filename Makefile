CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

TARGET = sagitta
SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.hpp)

.PHONY: all repl example clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# starts the interactive REPL
repl: $(TARGET)
	./$(TARGET)

# runs the example program
example: $(TARGET)
	./$(TARGET) examples/test.sgt

clean:
	rm -f $(TARGET)
