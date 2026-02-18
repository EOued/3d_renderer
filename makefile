CFILES = \
	texturer/texturer.cpp\
	arcball/arcball.cpp\
	camera/camera.cpp\
	main.cpp\

SRCDIR = src
SOURCES = $(foreach file, $(CFILES), $(SRCDIR)/$(file))
OBJDIR = build/obj
INCLUDE = -I$(SRCDIR)/headers
LIBRARIES = -lGLEW -lGL -lglut -lSDL3 -lyaml-cpp
OFILES = $(CFILES:.cpp=.o)
SUBDIRS = arcball texturer camera
TARGETS = $(foreach file, $(OFILES), $(OBJDIR)/$(file))
EXECDIR = build/target
EXECNAME = main
EXEC = $(EXECDIR)/$(EXECNAME)
VERSION = c++23
GXX = g++

prefixe=renderer

FLAGS=-g -Wall -Wextra -Wvla -fsanitize=address,undefined
POSTFLAGS=-lm

$(shell mkdir -p $(OBJDIR))
$(shell mkdir -p $(EXECDIR))
$(foreach dir, $(SUBDIRS), $(shell mkdir -p $(OBJDIR)/$(dir)))

$(EXEC): $(TARGETS)
	$(GXX)  $(FLAGS) -std=$(VERSION) $(INCLUDE) $(LIBRARIES) -o $(EXEC) $(TARGETS) $(POSTFLAGS)
	@echo ""
	@echo -e "\033[38;5;10m\033[1mFile available in $(EXEC)\033[0m"

# Rule to generate object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(GXX) $(FLAGS) -std=$(VERSION) $(INCLUDE) -c $< $(LIBRARIES) -o $@ $(POSTFLAGS)

# Clean rule
clean:
	rm -rf $(EXEC) $(OBJDIR);



