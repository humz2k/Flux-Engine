/**
 * @file scene_parser.c
 * @brief This file provides functionality for parsing scene and game object
 * data from files.
 */

#include "scene_parser.h"
#include "file_tools.h"
#include "hqtools/hqtools.h"
#include "prefab_parser.h"
#include "raylib.h"
#include "transform.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct fluxParsedGameObjectStruct
 * @brief Represents a parsed game object which can be instantiated in a scene.
 *
 * Contains a reference to a prefab, transformation data, and additional
 * arguments for instantiation.
 */
typedef struct fluxParsedGameObjectStruct {
    hstr prefab_name;        /**< Name of the prefab used for creating this game
                                object. */
    fluxTransform transform; /**< Transform data including position, rotation,
                                and scale. */
    hstrArray args; /**< Additional arguments specific to this game object
                       instance. */
} fluxParsedGameObjectStruct;

/**
 * @brief Creates a parsed game object.
 *
 * Allocates and initializes a game object structure with the specified prefab
 * name, transformation, and arguments.
 * @param prefab_name Name of the prefab to associate with the game object.
 * @param transform Transformation data for the game object.
 * @param args Additional instantiation arguments.
 * @return Pointer to the newly created parsed game object structure.
 */
static fluxParsedGameObject make_parsed_gameobject(hstr prefab_name,
                                                   fluxTransform transform,
                                                   hstrArray args) {
    LOG_FUNC_CALL();
    assert(prefab_name);
    TraceLog(LOG_INFO, "making gameobject from %s", hstr_unpack(prefab_name));
    fluxParsedGameObject out;
    assert(
        out = (fluxParsedGameObject)malloc(sizeof(fluxParsedGameObjectStruct)));
    memset(out, 0, sizeof(fluxParsedGameObjectStruct));
    out->prefab_name = hstr_incref(prefab_name);
    out->transform = transform;
    out->args = args;
    return out;
}

/**
 * @brief Retrieves the additional arguments of a parsed game object.
 * @param gameobject Pointer to the parsed game object.
 * @return Array of additional arguments.
 */
hstrArray parser_parsed_gameobject_get_args(fluxParsedGameObject gameobject) {
    LOG_FUNC_CALL();
    assert(gameobject);
    return gameobject->args;
}

/**
 * @brief Deletes a parsed game object.
 *
 * Frees the memory allocated for the parsed game object and decrements
 * reference counts for any held strings.
 * @param gameobject Pointer to the parsed game object to delete.
 */
static void delete_parsed_gameobject(fluxParsedGameObject gameobject) {
    LOG_FUNC_CALL();
    assert(gameobject);
    hstr_decref(gameobject->prefab_name);
    hstr_array_delete(gameobject->args);
    free(gameobject);
}

/**
 * @brief Retrieves the prefab name of a parsed game object.
 * @param gameobject Pointer to the parsed game object.
 * @return Name of the associated prefab.
 */
hstr parser_parsed_gameobject_get_prefab_name(fluxParsedGameObject gameobject) {
    LOG_FUNC_CALL();
    assert(gameobject);
    return gameobject->prefab_name;
}

/**
 * @brief Retrieves the transform data of a parsed game object.
 * @param gameobject Pointer to the parsed game object.
 * @return Transformation data.
 */
fluxTransform
parser_parsed_gameobject_get_transform(fluxParsedGameObject gameobject) {
    LOG_FUNC_CALL();
    assert(gameobject);
    return gameobject->transform;
}

/**
 * @struct fluxParsedSceneStruct
 * @brief Represents a parsed scene containing multiple game objects and
 * prefabs.
 *
 * Stores scene metadata including its name and path, and lists of prefabs and
 * game objects that exist within the scene.
 */
typedef struct fluxParsedSceneStruct {
    hstr path;                 /**< Path to the scene file. */
    hstr name;                 /**< Name of the scene. */
    int n_prefabs;             /**< Number of prefabs included in the scene. */
    fluxParsedPrefab* prefabs; /**< Array of pointers to parsed prefabs. */
    int n_gameobjects; /**< Number of game objects included in the scene. */
    fluxParsedGameObject*
        gameobjects; /**< Array of pointers to parsed game objects. */
} fluxParsedSceneStruct;

/**
 * @brief Allocates and initializes a parsed scene structure.
 * @return Pointer to the newly allocated parsed scene structure.
 */
static fluxParsedScene alloc_parsed_scene_internal(void) {
    LOG_FUNC_CALL();
    fluxParsedScene out =
        (fluxParsedScene)malloc(sizeof(fluxParsedSceneStruct));
    memset(out, 0, sizeof(fluxParsedSceneStruct));
    out->n_prefabs = 0;
    out->prefabs = NULL;
    return out;
}

/**
 * @brief Sets the path of a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @param path Path to set for the scene.
 */
static void parsed_scene_set_path(fluxParsedScene scene, hstr path) {
    LOG_FUNC_CALL();
    assert(scene);
    assert(path);
    if (scene->path)
        hstr_decref(scene->path);
    scene->path = hstr_incref(path);
}

/**
 * @brief Sets the name of a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @param name Name to set for the scene.
 */
static void parsed_scene_set_name(fluxParsedScene scene, hstr name) {
    LOG_FUNC_CALL();
    assert(scene);
    assert(name);
    if (scene->name)
        hstr_decref(scene->name);
    scene->name = hstr_incref(name);
}

/**
 * @brief Adds a prefab to a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @param prefab Pointer to the prefab to add.
 */
static void parsed_scene_add_prefab(fluxParsedScene scene,
                                    fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(scene);
    assert(prefab);
    assert(scene->n_prefabs >= 0);
    scene->n_prefabs++;
    scene->prefabs =
        realloc(scene->prefabs, sizeof(fluxParsedPrefab) * scene->n_prefabs);
    scene->prefabs[scene->n_prefabs - 1] = prefab;
}

/**
 * @brief Adds a game object to a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @param gameobject Pointer to the game object to add.
 */
static void parsed_scene_add_gameobject(fluxParsedScene scene,
                                        fluxParsedGameObject gameobject) {
    LOG_FUNC_CALL();
    assert(scene);
    assert(gameobject);
    assert(scene->n_gameobjects >= 0);
    scene->n_gameobjects++;
    scene->gameobjects =
        realloc(scene->gameobjects,
                sizeof(fluxParsedGameObject) * scene->n_gameobjects);
    scene->gameobjects[scene->n_gameobjects - 1] = gameobject;
}

/**
 * @brief Deletes a parsed scene and all its content.
 *
 * Frees memory allocated for the scene, its prefabs, and its game objects, and
 * decrements reference counts for any held strings.
 * @param scene Pointer to the parsed scene to delete.
 */
void parser_delete_parsed_scene(fluxParsedScene scene) {
    LOG_FUNC_CALL();
    assert(scene);
    if (scene->path)
        hstr_decref(scene->path);
    if (scene->name)
        hstr_decref(scene->name);
    for (int i = 0; i < scene->n_prefabs; i++) {
        parser_delete_parsed_prefab(scene->prefabs[i]);
    }
    for (int i = 0; i < scene->n_gameobjects; i++) {
        delete_parsed_gameobject(scene->gameobjects[i]);
    }
    if (scene->gameobjects)
        free(scene->gameobjects);
    if (scene->prefabs)
        free(scene->prefabs);
    free(scene);
}

/**
 * @brief Retrieves the name of a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @return Name of the scene.
 */
hstr parser_parsed_scene_get_name(fluxParsedScene scene) {
    LOG_FUNC_CALL();
    assert(scene);
    return scene->name;
}

/**
 * @brief Retrieves the number of prefabs in a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @return Number of prefabs.
 */
int parser_parsed_scene_get_n_prefabs(fluxParsedScene scene) {
    LOG_FUNC_CALL();
    assert(scene);
    return scene->n_prefabs;
}

/**
 * @brief Retrieves a specific prefab from a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @param i Index of the prefab to retrieve.
 * @return Pointer to the requested prefab.
 */
fluxParsedPrefab parser_parsed_scene_get_prefab(fluxParsedScene scene, int i) {
    LOG_FUNC_CALL();
    assert(scene);
    assert(i >= 0);
    assert(i < scene->n_prefabs);
    return scene->prefabs[i];
}

/**
 * @brief Retrieves the number of game objects in a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @return Number of game objects.
 */
int parser_parsed_scene_get_n_gameobjects(fluxParsedScene scene) {
    LOG_FUNC_CALL();
    assert(scene);
    return scene->n_gameobjects;
}

/**
 * @brief Retrieves a specific game object from a parsed scene.
 * @param scene Pointer to the parsed scene.
 * @param i Index of the game object to retrieve.
 * @return Pointer to the requested game object.
 */
fluxParsedGameObject parser_parsed_scene_get_gameobject(fluxParsedScene scene,
                                                        int i) {
    LOG_FUNC_CALL();
    assert(scene);
    assert(i >= 0);
    assert(i < scene->n_gameobjects);
    return scene->gameobjects[i];
}

/**
 * @brief Parses a scene from a specified file path.
 *
 * Reads a scene file and creates a parsed scene structure populated with data
 * extracted from the file.
 * @param raw_path Path to the scene file to parse.
 * @return Pointer to the newly parsed scene structure.
 */
fluxParsedScene parser_read_scene(const char* raw_path) {
    LOG_FUNC_CALL();
    hstr path = hstr_incref(hstr_new(raw_path));
    fluxParsedScene out = alloc_parsed_scene_internal();
    parsed_scene_set_path(out, path);

    FILE* fptr;
    TraceLog(LOG_INFO, "opening %s", hstr_unpack(path));
    fptr = fopen(hstr_unpack(path), "r");
    if (!fptr) {
        TraceLog(LOG_ERROR, "could not open %s", hstr_unpack(path));
        goto cleanup;
    }
    TraceLog(LOG_INFO, "opened %s", hstr_unpack(path));

    size_t sz = get_file_length(fptr);
    TraceLog(LOG_INFO, "file is %lu bytes", sz);

    hstr file_str = hstr_incref(read_whole_file(fptr));
    hstrArray lines = hstr_split(file_str, "\n");

    for (int i = 0; i < hstr_array_len(lines); i++) {
        hstr line = hstr_incref(hstr_array_get(lines, i));

        hstrArray arguments = hstr_split(line, "=");

        if (hstr_array_len(arguments) == 2) {

            hstr command =
                hstr_incref(hstr_strip(hstr_array_get(arguments, 0)));
            hstr argument =
                hstr_incref(hstr_strip(hstr_array_get(arguments, 1)));

            hstrArray argument_list = hstr_split(argument, ",");

            if (strcmp(hstr_unpack(command), "sceneName") == 0) {
                parsed_scene_set_name(out, argument);
            } else if (strcmp(hstr_unpack(command), "scenePrefabs") == 0) {
                for (int k = 0; k < hstr_array_len(argument_list); k++) {
                    hstr prefab_path = hstr_incref(
                        hstr_strip(hstr_array_get(argument_list, k)));
                    fluxParsedPrefab prefab =
                        parser_read_prefab(hstr_unpack(prefab_path));
                    parsed_scene_add_prefab(out, prefab);
                    hstr_decref(prefab_path);
                }
            } else if (strcmp(hstr_unpack(command), "sceneGameObject") == 0) {
                assert(hstr_array_len(argument_list) >= 10);
                hstr prefab_name =
                    hstr_incref(hstr_array_get(argument_list, 0));
                fluxTransform transform;
                transform.pos.x =
                    atof(hstr_unpack(hstr_array_get(argument_list, 1)));
                transform.pos.y =
                    atof(hstr_unpack(hstr_array_get(argument_list, 2)));
                transform.pos.z =
                    atof(hstr_unpack(hstr_array_get(argument_list, 3)));
                transform.rot.x =
                    atof(hstr_unpack(hstr_array_get(argument_list, 4)));
                transform.rot.y =
                    atof(hstr_unpack(hstr_array_get(argument_list, 5)));
                transform.rot.z =
                    atof(hstr_unpack(hstr_array_get(argument_list, 6)));
                transform.scale.x =
                    atof(hstr_unpack(hstr_array_get(argument_list, 7)));
                transform.scale.y =
                    atof(hstr_unpack(hstr_array_get(argument_list, 8)));
                transform.scale.z =
                    atof(hstr_unpack(hstr_array_get(argument_list, 9)));
                //TraceLog(INFO,"transform: %g %g %g, %g %g %g, %g %g %g",transform.pos.x,transform.pos.y,transform.pos.z,transform.rot.x,transform.rot.y,transform.rot.z,transform.scale.x,transform.scale.y,transform.scale.z);
                hstrArray args = hstr_array_make();
                for (int m = 10; m < hstr_array_len(argument_list); m++) {
                    hstr_array_append(args, hstr_array_get(argument_list, m));
                }
                parsed_scene_add_gameobject(
                    out, make_parsed_gameobject(prefab_name, transform, args));
                hstr_decref(prefab_name);
            }

            hstr_array_delete(argument_list);
            hstr_decref(command);
            hstr_decref(argument);

        } else {
            TraceLog(LOG_ERROR, "malformed line %s", hstr_unpack(line));
        }

        hstr_array_delete(arguments);
        hstr_decref(line);
    }

    hstr_array_delete(lines);
    hstr_decref(file_str);

    TraceLog(LOG_INFO, "closing %s", hstr_unpack(path));
    if (fclose(fptr)) {
        TraceLog(LOG_ERROR, "close %s failed...", hstr_unpack(path));
    } else {
        TraceLog(LOG_INFO, "closed %s", hstr_unpack(path));
    }
cleanup:
    hstr_decref(path);
    return out;
}