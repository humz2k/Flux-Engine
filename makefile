RAYLIB_DIR ?= ext/raylib/src
RAYLIB_FLAGS ?= -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

FLUX_CC_FLAGS ?= -Wall -Wpedantic -Wno-newline-eof -fsanitize=address -fno-omit-frame-pointer -fPIC

INIH_DIR ?= inih

BUILD_DIR ?= build

SOURCE_DIR ?= engine
EDITOR_DIR ?= editor
RENDERER_DIR ?= renderer
DRIVERS_DIR ?= drivers

FLUX_LIB ?= libflux.a

PROJECT_DIR ?= project

FLUX_PRIVATE_INCLUDES := -I$(RAYLIB_DIR) -I$(SOURCE_DIR) -I$(PROJECT_DIR) -I$(EDITOR_DIR)
FLUX_LIBRARIES := $(BUILD_DIR)/$(FLUX_LIB) $(RAYLIB_DIR)/libraylib.a

SCRIPT_SOURCES := $(shell find $(PROJECT_DIR)/scripts -name '*.c')
SCRIPT_OBJECTS := $(SCRIPT_SOURCES:%.c=%.o)
SCRIPT_OUTPUTS := $(SCRIPT_OBJECTS:%=build/%)

EDITOR_SOURCES := $(shell find $(EDITOR_DIR) -name '*.c')
EDITOR_OBJECTS := $(EDITOR_SOURCES:%.c=%.o)
EDITOR_OUTPUTS := $(EDITOR_OBJECTS:%=build/%)

RENDERER_SOURCES := $(shell find $(RENDERER_DIR) -name '*.c')
RENDERER_OBJECTS := $(RENDERER_SOURCES:%.c=%.o)
RENDERER_OUTPUTS := $(RENDERER_OBJECTS:%=build/%)

PREFABS := $(shell find $(PROJECT_DIR)/prefabs -name '*.prefab')

SOURCES := $(shell find $(SOURCE_DIR) -name '*.c') $(shell find $(SOURCE_DIR) -name '*.cpp')
OBJECTS_1 := $(SOURCES:%.c=%.o)
OBJECTS := $(OBJECTS_1:%.cpp=%.o)
OUTPUTS := $(OBJECTS:%=build/%) $(SCRIPT_OUTPUTS)

main: driver flux_editor test_render

$(SOURCE_DIR)/GENERATED_SCRIPTS.h: $(SCRIPT_SOURCES)
	python3 ./build_scripts.py $(PROJECT_DIR)

$(SOURCE_DIR)/GENERATED_PREFABS.h: $(PREFABS)
	python3 ./build_prefabs.py $(PROJECT_DIR)

.PHONY: $(RAYLIB_DIR)/libraylib.a
$(RAYLIB_DIR)/libraylib.a:
	cd $(RAYLIB_DIR) && $(MAKE) MACOSX_DEPLOYMENT_TARGET=10.9

$(BUILD_DIR)/$(FLUX_LIB): $(OUTPUTS) $(EDITOR_OUTPUTS) $(RENDERER_OUTPUTS) | $(BUILD_DIR)
	$(AR) rcs $@ $^

%: $(DRIVERS_DIR)/%.c $(FLUX_LIBRARIES)
	$(CC) $(FLUX_PRIVATE_INCLUDES) $^ -o $@ $(RAYLIB_FLAGS) $(FLUX_CC_FLAGS)

build/%.o: %.c $(SOURCE_DIR)/GENERATED_SCRIPTS.h $(SOURCE_DIR)/GENERATED_PREFABS.h | $(BUILD_DIR)
	$(CC) $(FLUX_PRIVATE_INCLUDES) -c -o $@ $< $(FLUX_CC_FLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(SOURCE_DIR)
	mkdir -p $(BUILD_DIR)/$(EDITOR_DIR)
	mkdir -p $(BUILD_DIR)/$(PROJECT_DIR)
	mkdir -p $(BUILD_DIR)/$(RENDERER_DIR)
	mkdir -p $(BUILD_DIR)/$(PROJECT_DIR)/scripts

clean:
	rm -rf build
	rm -rf $(SOURCE_DIR)/GENERATED*
	rm -rf driver
	rm -rf flux_editor
	rm -rf test_render
#	cd $(RAYLIB_DIR) && $(MAKE) clean