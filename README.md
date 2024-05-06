# VERY WIP!!!

# Flux-Engine

## Goals
* Super simple DIY game engine:
  * Very simple renderer.
  * Very simple entity-component system (kind of).
  * Very simple scene manager.
* Bare minimum external dependencies:
  * Raylib
  * ODE (physics)
  * ENET (networking)
* Pure C (including scripting).

## Building
First run `python3 configure.py`, then `make`.

If building in debug mode, `make DEBUG=true`.

## Architecture
Four modules:

### `src/editor`
* Deals with drawing UI elements to be used for a very simple scene editor/developer console.

### `src/engine`
* Deals with all entity/scene things (loading scenes, calling scripts etc.).

### `src/parsers`
* Parses config files, like prefabs and scenes.
* All `src/parsers` functions are (I'm pretty sure) prefixed by `parser`.
* Generally, these functions act/return on `fluxParsed[Object]`, e.g. `fluxParsedScene`/`fluxParsedGameObject`/`fluxParsedPrefab`, that are then handled and converted into `fluxGameObject`s and `fluxPrefab`s in `src/engine`.

#### Notes
Scenes are parsed by calling `fluxParsedScene parser_read_scene(const char* raw_path)`. `fluxParsedScene` is a heap allocated opaque pointer. Use getters/setters in `scene_parser.h` to interact.

#### Config file syntax:
NOTE: whitespace is unfortunately significant - pay attention to this.
##### `.prefab`
```
attributename = value
attributename = value,value,value
```
For example:
```
prefabName = defaultCamera
prefabModel = /path/to/model
prefabScripts = script1,script2
```
All attributes:
```
prefabName - name of the prefab (used to reference it in code)
prefabModel - path to prefab model, or PRIMITIVE (e.g., SPHERE)
prefabScripts - list of scripts for this prefab
prefabChildren - NOT IMPLEMENTED, but list of names of prefabs that will be children
prefabIsCamera - is this prefab a camera?
prefabFOV - NOT IMPLEMENTED, but the fov of the camera if prefabIsCamera
prefabProjection - NOT IMPLEMENTED, but the projection of the camera if prefabIsCamera
```
##### `.scene`
```
sceneName = name_of_scene
scenePrefabs = /paths/to/prefabs.prefab,/paths/to/other/prefabs.prefab
sceneGameObject = prefab_name,posx,posy,posz,rotx,roty,rotz,scalex,scaley,scalez,extra,arguments,here
sceneGameObject = ...
...
```
For example:
```
sceneName = testScene
scenePrefabs = /Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/defaultCamera.prefab,/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/testPrefab.prefab,/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/lightmanager.prefab
sceneGameObject = defaultCamera,0,0,-10,0,0,0,1,1,1
sceneGameObject = testPrefab,0,0,0,0,0,0,1,1,1
sceneGameObject = testPrefab,2,0,0,0,0,0,1,1,1
sceneGameObject = testPrefab,-2,0,0,0,0,0,1,1,1
sceneGameObject = lightmanager,0,0,0,0,0,0,1,1,1,ka:0.2,skybox:drivers/assets/Daylight Box UV.png
```

### `src/renderer`
* Super simple renderer.

#### Notes:
* Include `pipeline.h`.
* Make sure you call `render_init()` at the start of `main` and `render_close()` at the end of `main` ONCE.
* Set scene lights using `render_light` functions. E.g., to enable light `0`, call `render_light_enable(0)`. Then to set values for light `0`, call `render_light_set_ATTR(0,VAL)`.