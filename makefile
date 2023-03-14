MAKEFLAGS += -j
CXX = g++
CXXFLAGS = -MD -MP -fopenmp

LDFLAGS = -lepoxy -lglut -lfreeimage
BUILD = build
OUT = $(BUILD)/nicemesh
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(BUILD)/%.o)
DEPS = $(SRC:%.cpp=$(BUILD)/%.d)

TRIANGLE = triangle/triangle.o

.PHONY: dir clean debug

all: dir main

debug: CXXFLAGS += -g
debug: all

dir:
	@mkdir -p $(BUILD)

main: $(OBJ) $(TRIANGLE)
	$(CXX) $(CXXFLAGS) -o $(OUT) $^ $(LDFLAGS)

$(OBJ): $(BUILD)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TRIANGLE):
	@$(MAKE) -C ./triangle trilibrary

clean:
	@rm -f -r $(BUILD)
	@rm -f triangle/triangle.o