/**
 * @file prefab_parser.c
 * @brief Functions for parsing prefab data structures from file inputs,
 * managing and querying prefab properties.
 */

#include "prefab_parser.h"
#include "file_tools.h"
#include "hqtools/hqtools.h"
#include "raylib.h"
#include "transform.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct fluxParsedPrefabStruct
 * @brief Represents a parsed prefab data structure.
 *
 * This structure holds data related to a prefab including paths, names, model
 * info, camera settings, scripts, and child prefab names.
 */
typedef struct fluxParsedPrefabStruct {
    hstr path;      /**< Path from which this prefab was parsed. */
    hstr name;      /**< Name of the prefab used for instantiation. */
    bool has_model; /**< Flag indicating whether the prefab includes a model. */
    hstr model_path; /**< Path to the prefab's model file. */
    bool is_camera; /**< Flag indicating whether the prefab represents a camera.
                     */
    float fov;      /**< Field of view if the prefab is a camera. */
    int projection; /**< Projection type if the prefab is a camera. */
    hstrArray scripts;  /**< Array of script names attached to the prefab. */
    hstrArray children; /**< Array of child prefab names. */
    Color tint;
} fluxParsedPrefabStruct;

/**
 * @brief Retrieves the field of view setting for a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return The field of view as a float.
 */
float parser_parsed_prefab_get_fov(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->fov;
}

/**
 * @brief Retrieves the projection type of a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return The projection type as an integer.
 */
int parser_parsed_prefab_get_projection(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->projection;
}

/**
 * @brief Sets the field of view for a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @param fov The field of view value to set.
 */
void parser_parsed_prefab_set_fov(fluxParsedPrefab prefab, float fov) {
    LOG_FUNC_CALL();
    assert(prefab);
    prefab->fov = fov;
}

/**
 * @brief Sets the projection type for a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @param projection The projection type value to set.
 */
void parser_parsed_prefab_set_projection(fluxParsedPrefab prefab,
                                         int projection) {
    LOG_FUNC_CALL();
    assert(prefab);
    prefab->projection = projection;
}

/**
 * @brief Retrieves the path from which a prefab was parsed.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return The path as an hstr.
 */
hstr parser_parsed_prefab_get_path(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->path;
}

/**
 * @brief Retrieves the name of a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return The name as an hstr.
 */
hstr parser_parsed_prefab_get_name(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->name;
}

/**
 * @brief Checks if a prefab has a model.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return True if the prefab has a model, otherwise false.
 */
bool parser_parsed_prefab_has_model(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->has_model;
}

/**
 * @brief Retrieves the model path for a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return The model path as an hstr.
 */
hstr parser_parsed_prefab_get_model_path(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->model_path;
}

/**
 * @brief Checks if a prefab is configured as a camera.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return True if the prefab is a camera, otherwise false.
 */
bool parser_parsed_prefab_is_camera(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->is_camera;
}

/**
 * @brief Retrieves the scripts attached to a prefab.
 * @param prefab A pointer to the fluxParsedPrefabStruct.
 * @return An array of script names as hstrArray.
 */
hstrArray parser_parsed_prefab_get_scripts(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->scripts;
}

/**
 * @brief Allocates and initializes a new parsed prefab structure.
 * This function sets default values for a new prefab, including default camera
 * settings and empty script and child arrays.
 * @return A pointer to the newly allocated fluxParsedPrefabStruct.
 */
static fluxParsedPrefab alloc_parsed_prefab_internal(void) {
    LOG_FUNC_CALL();
    fluxParsedPrefab out =
        (fluxParsedPrefab)malloc(sizeof(fluxParsedPrefabStruct));
    memset(out, 0, sizeof(fluxParsedPrefabStruct));
    out->scripts = hstr_array_make();
    out->children = hstr_array_make();
    out->fov = 45;
    out->projection = CAMERA_PERSPECTIVE;
    out->tint = WHITE;
    return out;
}

/**
 * @brief Sets the path from which a prefab was parsed.
 * This function updates the path property of a prefab and manages reference
 * counting for the hstr type.
 * @param prefab A pointer to the fluxParsedPrefabStruct whose path is being
 * set.
 * @param path The new path as an hstr.
 */
static void parsed_prefab_set_path(fluxParsedPrefab prefab, hstr path) {
    LOG_FUNC_CALL();
    assert(prefab);
    assert(path);
    if (prefab->path) {
        hstr_decref(prefab->path);
    }
    prefab->path = hstr_incref(path);
    TraceLog(LOG_INFO, "setting prefab->path = %s", hstr_unpack(prefab->path));
}

/**
 * @brief Sets the name of a prefab.
 * This function updates the name property of a prefab and manages reference
 * counting for the hstr type.
 * @param prefab A pointer to the fluxParsedPrefabStruct whose name is being
 * set.
 * @param name The new name as an hstr.
 */
static void parsed_prefab_set_name(fluxParsedPrefab prefab, hstr name) {
    LOG_FUNC_CALL();
    assert(prefab);
    assert(name);
    if (prefab->name) {
        hstr_decref(prefab->name);
    }
    prefab->name = hstr_incref(name);
    TraceLog(LOG_INFO, "setting prefab->name = %s", hstr_unpack(prefab->name));
}

/**
 * @brief Sets the model path for a prefab.
 * This function updates the model path property of a prefab, sets the has_model
 * flag to true, and manages reference counting for the hstr type.
 * @param prefab A pointer to the fluxParsedPrefabStruct whose model path is
 * being set.
 * @param model_path The new model path as an hstr.
 */
static void parsed_prefab_set_model_path(fluxParsedPrefab prefab,
                                         hstr model_path) {
    LOG_FUNC_CALL();
    assert(prefab);
    assert(model_path);
    if (prefab->has_model) {
        assert(prefab->model_path);
        hstr_decref(prefab->model_path);
    }
    assert(!prefab->is_camera);
    prefab->has_model = true;
    prefab->model_path = hstr_incref(model_path);
    TraceLog(LOG_INFO, "setting prefab->model_path = %s",
             hstr_unpack(prefab->model_path));
}

/**
 * @brief Configures a prefab to represent a camera.
 * This function sets the is_camera flag to true for a prefab, ensuring it does
 * not also have a model.
 * @param prefab A pointer to the fluxParsedPrefabStruct to be configured as a
 * camera.
 */
static void parsed_prefab_set_is_camera(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    assert(!prefab->has_model);
    prefab->is_camera = true;
    TraceLog(LOG_INFO, "setting prefab->is_camera = true");
}

/**
 * @brief Adds a script to a prefab.
 * This function appends a new script name to the scripts array of a prefab and
 * manages reference counting for the hstr type.
 * @param prefab A pointer to the fluxParsedPrefabStruct to which the script is
 * being added.
 * @param script The script name as an hstr to add.
 */
static void parsed_prefab_add_script(fluxParsedPrefab prefab, hstr script) {
    LOG_FUNC_CALL();
    assert(prefab);
    assert(script);
    assert(prefab->scripts);
    hstr_array_append(prefab->scripts, script);
    TraceLog(LOG_INFO, "adding prefab script = %s", hstr_unpack(script));
}

/**
 * @brief Adds a child to a prefab.
 * This function appends a new child name to the children array of a prefab and
 * manages reference counting for the hstr type.
 * @param prefab A pointer to the fluxParsedPrefabStruct to which the child is
 * being added.
 * @param child The child name as an hstr to add.
 */
static void parsed_prefab_add_child(fluxParsedPrefab prefab, hstr child) {
    LOG_FUNC_CALL();
    assert(prefab);
    assert(child);
    assert(prefab->children);
    hstr_array_append(prefab->children, child);
    TraceLog(LOG_INFO, "adding prefab child = %s", hstr_unpack(child));
}

static void parsed_prefab_set_tint(fluxParsedPrefab prefab, Color tint) {
    LOG_FUNC_CALL();
    assert(prefab);
    prefab->tint = tint;
}

/**
 * @brief Frees all resources associated with a parsed prefab and deletes it.
 * This function cleans up all properties of a prefab, including paths, scripts,
 * and children, managing reference counting and freeing all associated memory.
 * @param prefab A pointer to the fluxParsedPrefabStruct to delete.
 */
void parser_delete_parsed_prefab(fluxParsedPrefab prefab) {
    LOG_FUNC_CALL();
    TraceLog(LOG_INFO, "deleting parsed prefab");
    assert(prefab);
    if (prefab->path)
        hstr_decref(prefab->path);
    if (prefab->name)
        hstr_decref(prefab->name);
    if (prefab->model_path)
        hstr_decref(prefab->model_path);

    assert(prefab->scripts);
    hstr_array_delete(prefab->scripts);

    assert(prefab->children);
    hstr_array_delete(prefab->children);
    free(prefab);
}

/**
 * @brief Parses a prefab from a specified file path.
 * This function reads and parses prefab data from a file, creating a new parsed
 * prefab structure populated with the data extracted from the file.
 * @param raw_path The file path from which to parse the prefab.
 * @return A pointer to the newly parsed fluxParsedPrefabStruct.
 */
fluxParsedPrefab parser_read_prefab(const char* raw_path) {
    LOG_FUNC_CALL();
    hstr path = hstr_incref(hstr_new(raw_path));
    fluxParsedPrefab out = alloc_parsed_prefab_internal();

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

            if (strcmp(hstr_unpack(command), "prefabName") == 0) {
                parsed_prefab_set_name(out, argument);
            } else if (strcmp(hstr_unpack(command), "prefabModel") == 0) {
                parsed_prefab_set_model_path(out, argument);
            } else if (strcmp(hstr_unpack(command), "prefabScripts") == 0) {
                for (int k = 0; k < hstr_array_len(argument_list); k++) {
                    hstr script_name = hstr_incref(
                        hstr_strip(hstr_array_get(argument_list, k)));
                    parsed_prefab_add_script(out, script_name);
                    hstr_decref(script_name);
                }
            } else if (strcmp(hstr_unpack(command), "prefabChildren") == 0) {
                for (int k = 0; k < hstr_array_len(argument_list); k++) {
                    hstr child_name = hstr_incref(
                        hstr_strip(hstr_array_get(argument_list, k)));
                    parsed_prefab_add_child(out, child_name);
                    hstr_decref(child_name);
                }
            } else if (strcmp(hstr_unpack(command), "prefabIsCamera") == 0) {
                if (strcmp(hstr_unpack(argument), "true") == 0) {
                    parsed_prefab_set_is_camera(out);
                }
            } else if (strcmp(hstr_unpack(command), "prefabTint") == 0) {
                assert(hstr_array_len(argument_list) == 4);
                Color tint;
                tint.r = atoi(hstr_unpack(hstr_array_get(argument_list, 0)));
                tint.g = atoi(hstr_unpack(hstr_array_get(argument_list, 1)));
                tint.b = atoi(hstr_unpack(hstr_array_get(argument_list, 2)));
                tint.a = atoi(hstr_unpack(hstr_array_get(argument_list, 3)));
                parsed_prefab_set_tint(out, tint);
            }

            hstr_array_delete(argument_list);
            hstr_decref(command);
            hstr_decref(argument);

        } else {
            TraceLog(LOG_ERROR, "malformed line %s", hstr_unpack(line));
        }
        // TraceLog(LOG_INFO,"line %d: %s",i,hstr_unpack(line));

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

    parsed_prefab_set_path(out, path);

cleanup:
    hstr_decref(path);
    return out;
}

Color parser_parsed_prefab_get_tint(fluxParsedPrefab parsed) {
    assert(parsed);
    return parsed->tint;
}