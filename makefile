MAKEFLAGS += -j
CXX = g++
CXXFLAGS = -MD -MP -fopenmp

LDFLAGS = -lepoxy -lglut -lfreeimage
BUILD = build
OUT = $(BUILD)/nicemesh
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(BUILD)/%.o)
DEPS = $(SRC:%.cpp=$(BUILD)/%.d)

all: dir main

debug: CXXFLAGS += -g
debug: all

dir:
	@mkdir -p $(BUILD)

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(OUT) $^ $(LDFLAGS)

$(OBJ): $(BUILD)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(SILENCE)

-include $(DEPS)

clean:
	@rm -f -r $(BUILD)