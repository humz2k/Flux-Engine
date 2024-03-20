RAYLIB_DIR ?= ext/raylib/src
RAYLIB_FLAGS ?= -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

INIH_DIR ?= inih

BUILD_DIR ?= build

SOURCE_DIR ?= engine

PROJECT_DIR ?= project

SCRIPT_SOURCES := $(shell find $(PROJECT_DIR) -name '*.c')
SCRIPT_OBJECTS := $(SCRIPT_SOURCES:%.c=%.o)
SCRIPT_OUTPUTS := $(SCRIPT_OBJECTS:%=build/%)

SCENES := $(shell find $(PROJECT_DIR) -name '*.json')

SOURCES := $(shell find $(SOURCE_DIR) -name '*.c') $(shell find $(SOURCE_DIR) -name '*.cpp')
OBJECTS_1 := $(SOURCES:%.c=%.o)
OBJECTS := $(OBJECTS_1:%.cpp=%.o)
OUTPUTS := $(OBJECTS:engine%=build%) $(SCRIPT_OUTPUTS)

main: $(SOURCE_DIR)/GENERATED_SCRIPTS.h driver

$(SOURCE_DIR)/GENERATED_SCRIPTS.h: $(SCRIPT_SOURCES) $(SCENES)
	python3 ./build_scripts.py $(PROJECT_DIR)

.PHONY: $(RAYLIB_DIR)/libraylib.a
$(RAYLIB_DIR)/libraylib.a:
	cd $(RAYLIB_DIR) && $(MAKE) MACOSX_DEPLOYMENT_TARGET=10.9

driver: $(OUTPUTS) $(RAYLIB_DIR)/libraylib.a
	$(CXX) $^ -o $@ $(RAYLIB_FLAGS)

build/%.o: $(SOURCE_DIR)/%.c $(SOURCE_DIR)/GENERATED_SCRIPTS.h | $(BUILD_DIR)
	$(CC) -I$(RAYLIB_DIR) -I$(SOURCE_DIR) -I$(PROJECT_DIR) -c -o $@ $<

build/%.o: %.c $(SOURCE_DIR)/GENERATED_SCRIPTS.h | $(BUILD_DIR)
	$(CC) -I$(RAYLIB_DIR) -I$(SOURCE_DIR) -I$(PROJECT_DIR) -c -o $@ $<

build/%.o: $(SOURCE_DIR)/%.cpp $(SOURCE_DIR)/GENERATED_SCRIPTS.h | $(BUILD_DIR)
	$(CXX) -I$(RAYLIB_DIR) -I$(SOURCE_DIR) -I$(PROJECT_DIR) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(PROJECT_DIR)
	mkdir -p $(BUILD_DIR)/$(PROJECT_DIR)/scripts

clean:
	rm -rf build
	rm -rf $(SOURCE_DIR)/GENERATED*
#	cd $(RAYLIB_DIR) && $(MAKE) clean