# Compiler and compilation flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g # Use this doe debug
# CXXFLAGS = -std=c++20 -Wall -Wextra


# Identifies all source files in the current directory
SRC = $(wildcard *.cpp)

# Maps source files to corresponding binary targets in the bin/ directory
TARGETS = $(SRC:%.cpp=bin/%)

# Default build target
all: $(TARGETS)

# Pattern rule: compiles each .cpp into a standalone executable in bin/
bin/%: %.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $< -o $@ -lcurl

# Removes all compiled binaries
clean:
	rm -rf bin/*