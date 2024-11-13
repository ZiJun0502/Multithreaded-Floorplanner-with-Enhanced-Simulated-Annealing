CXX = g++
CXXFLAGS = -std=c++11 -lpthread -Ofast

# Folders
SRCDIR = src
BUILDDIR = build

# Find all .cpp files in src/ directory and their corresponding .o files in build/
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Target binary in the root directory
TARGET = floorplanner

all: $(TARGET)

# Link all object files to create the final binary
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile each .cpp file to the corresponding .o file in the build/ directory
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)
