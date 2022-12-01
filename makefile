MAKEFLAGS += -j
CXX = g++
CXXFLAGS = -g -MD -MP
LDFLAGS = -lepoxy -lglut -lfreeimage
SILENCE = > /dev/null 2>&1
BUILDDIR = build
OUT = $(BUILDDIR)/a.out
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(BUILDDIR)/%.o)
DEPS = $(SRC:%.cpp=$(BUILDDIR)/%.d)

run: dir main
	build/a.out

dir:
	mkdir -p $(BUILDDIR)

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(OUT) $^ $(LDFLAGS)

$(OBJ): $(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(SILENCE)

-include $(DEPS)

clean:
	rm -f -r $(BUILDDIR)