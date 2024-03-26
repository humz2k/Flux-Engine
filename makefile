PLATFORM_OS ?= UNKNOWN

ifeq ($(OS),Windows_NT)
	PLATFORM_OS = WINDOWS
else
	UNAMEOS = $(shell uname)
	ifeq ($(UNAMEOS), Darwin)
		PLATFORM_OS = OSX
	endif
endif

RAYLIB_DIR ?= ext/raylib/src
RAYLIB_OSX_FLAGS ?= -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
RAYLIB_WINDOWS_FLAGS ?= -lopengl32 -lgdi32 -lwinmmgit

RAYLIB_FLAGS ?= UNSUPPORTED_PLATFORM

ifeq ($(PLATFORM_OS), WINDOWS)
	RAYLIB_FLAGS = $(RAYLIB_WINDOWS_FLAGS)
endif
ifeq ($(PLATFORM_OS), OSX)
	RAYLIB_FLAGS = $(RAYLIB_OSX_FLAGS)
endif

FLUX_CC_FLAGS ?= -Wall -Wpedantic -Wno-newline-eof -O2 -fno-inline -fPIC

INIH_DIR ?= inih

BUILD_DIR ?= build

ENGINE_DIR ?= engine
EDITOR_DIR ?= editor
RENDERER_DIR ?= renderer
DRIVERS_DIR ?= drivers

FLUX_LIB ?= libflux.a

PROJECT_DIR ?= project

FLUX_PRIVATE_INCLUDES := -I$(RAYLIB_DIR) -I$(ENGINE_DIR) -I$(PROJECT_DIR) -I$(EDITOR_DIR) -I$(RENDERER_DIR)

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

ENGINE_SOURCES := $(shell find $(ENGINE_DIR) -name '*.c')
ENGINE_OBJECTS := $(ENGINE_SOURCES:%.c=%.o)
ENGINE_OUTPUTS := $(ENGINE_OBJECTS:%=build/%) $(SCRIPT_OUTPUTS)

main: driver flux_editor test_render

$(SOURCE_DIR)/GENERATED_SCRIPTS.h: $(SCRIPT_SOURCES)
	python3 ./build_scripts.py $(PROJECT_DIR)

$(SOURCE_DIR)/GENERATED_PREFABS.h: $(PREFABS)
	python3 ./build_prefabs.py $(PROJECT_DIR)

.PHONY: $(RAYLIB_DIR)/libraylib.a
$(RAYLIB_DIR)/libraylib.a:
	cd $(RAYLIB_DIR) && $(MAKE) MACOSX_DEPLOYMENT_TARGET=10.9 CUSTOM_CFLAGS=-fno-inline

$(BUILD_DIR)/$(FLUX_LIB): $(ENGINE_OUTPUTS) $(EDITOR_OUTPUTS) $(RENDERER_OUTPUTS) | $(BUILD_DIR)
	$(AR) rcs $@ $^

%: $(DRIVERS_DIR)/%.c $(BUILD_DIR)/$(FLUX_LIB) $(RAYLIB_DIR)/libraylib.a
	$(CC) $(FLUX_PRIVATE_INCLUDES) $^ -o $@ $(RAYLIB_FLAGS) $(FLUX_CC_FLAGS)

build/%.o: %.c $(SOURCE_DIR)/GENERATED_SCRIPTS.h $(SOURCE_DIR)/GENERATED_PREFABS.h | $(BUILD_DIR)
	$(CC) $(FLUX_PRIVATE_INCLUDES) -c -o $@ $< $(FLUX_CC_FLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(ENGINE_DIR)
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
	cd $(RAYLIB_DIR) && $(MAKE) clean