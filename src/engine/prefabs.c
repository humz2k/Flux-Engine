/*
                  __      _
                 / _|    | |
  _ __  _ __ ___| |_ __ _| |__  ___
 | '_ \| '__/ _ \  _/ _` | '_ \/ __|
 | |_) | | |  __/ || (_| | |_) \__ \
 | .__/|_|  \___|_| \__,_|_.__/|___/
 | |
 |_|
*/

/**
 * @file prefabs.c
 * @brief Handles the creation, manipulation, and destruction of prefabs, which are templates for constructing game objects.
 *
 * This file defines the operations for loading prefabs from parsed data, accessing their properties,
 * and cleaning them up. Prefabs encapsulate data necessary to instantiate game objects with predefined attributes like models,
 * scripts, and camera settings.
 */

#include "prefabs.h"
#include "hqtools/hqtools.h"
#include "pipeline.h"
#include "prefab_parser.h"
#include "scripts.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct fluxPrefabStruct
 * @brief Represents a template from which game objects can be created.
 *
 * Contains properties that can be applied to new game objects, such as models, camera settings, and scripts.
 */
typedef struct fluxPrefabStruct {
    hstr name;                 ///< Name of the prefab.
    bool has_model;            ///< Flag indicating whether the prefab includes a model.
    Model raw_model;           ///< Raw model data loaded from resources.
    renderModel model;         ///< Processed model ready for rendering.
    bool is_camera;            ///< Indicates if the prefab represents a camera.
    int n_scripts;             ///< Number of scripts attached to the prefab.
    enum fluxScriptID* scripts; ///< Array of script identifiers.
    int projection;            ///< Camera projection type (orthographic, perspective).
    float fov;                 ///< Field of view, relevant if the prefab is a camera.
    Color tint;
} fluxPrefabStruct;

/**
 * @brief Retrieves the name of a prefab.
 * @param prefab Pointer to the prefab.
 * @return Name of the prefab.
 */
hstr flux_prefab_get_name(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->name;
}

/**
 * @brief Retrieves the projection type of the prefab.
 * @param prefab Pointer to the prefab.
 * @return Projection type as an integer.
 */
int flux_prefab_get_projection(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->projection;
}

/**
 * @brief Sets the projection type for the prefab.
 * @param prefab Pointer to the prefab.
 * @param projection The projection type to set.
 */
void flux_prefab_set_projection(fluxPrefab prefab, int projection) {
    LOG_FUNC_CALL();
    assert(prefab);
    prefab->projection = projection;
}

/**
 * @brief Retrieves the field of view of the prefab.
 * @param prefab Pointer to the prefab.
 * @return Field of view as a float.
 */
float flux_prefab_get_fov(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->fov;
}

/**
 * @brief Sets the field of view for the prefab.
 * @param prefab Pointer to the prefab.
 * @param fov The field of view value to set.
 */
void flux_prefab_set_fov(fluxPrefab prefab, float fov) {
    LOG_FUNC_CALL();
    assert(prefab);
    prefab->fov = fov;
}

/**
 * @brief Checks if the prefab has an associated model.
 * @param prefab Pointer to the prefab.
 * @return True if a model is associated, otherwise false.
 */
bool flux_prefab_has_model(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->has_model;
}

/**
 * @brief Retrieves the raw model associated with the prefab.
 * @param prefab Pointer to the prefab.
 * @return Model data.
 */
Model flux_prefab_get_raw_model(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->raw_model;
}

/**
 * @brief Retrieves the renderModel associated with the prefab.
 * @param prefab Pointer to the prefab.
 * @return renderModel
 */
renderModel flux_prefab_get_model(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->model;
}

/**
 * @brief Checks if the prefab represents a camera.
 * @param prefab Pointer to the prefab.
 * @return True if the prefab is a camera, otherwise false.
 */
bool flux_prefab_is_camera(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->is_camera;
}

/**
 * @brief Retrieves the number of scripts attached to the prefab.
 * @param prefab Pointer to the prefab.
 * @return Number of scripts.
 */
int flux_prefab_get_n_scripts(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->n_scripts;
}

/**
 * @brief Retrieves the script identifiers attached to the prefab.
 * @param prefab Pointer to the prefab.
 * @return Array of script identifiers.
 */
enum fluxScriptID* flux_prefab_get_scripts(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    return prefab->scripts;
}

/**
 * @brief Loads a prefab from parsed data.
 *
 * This function constructs a prefab from parsed data, initializing its properties and preparing it for use in the game.
 * @param parsed Parsed prefab data.
 * @return Pointer to the newly created prefab.
 */
fluxPrefab flux_load_prefab(fluxParsedPrefab parsed) {
    LOG_FUNC_CALL();
    assert(parsed);
    fluxPrefab out;
    assert(out = (fluxPrefab)malloc(sizeof(fluxPrefabStruct)));
    memset(out, 0, sizeof(fluxPrefabStruct));
    out->name = hstr_incref(parser_parsed_prefab_get_name(parsed));
    out->has_model = parser_parsed_prefab_has_model(parsed);
    out->model = NULL;
    if (out->has_model) {
        const char* model_path =
            hstr_unpack(parser_parsed_prefab_get_model_path(parsed));
        TraceLog(LOG_INFO, "loading model %s", model_path);
        if (strcmp(model_path, "SPHERE") == 0) {
            out->raw_model = LoadModelFromMesh(GenMeshSphere(1, 50, 50));
        }
        else if (strcmp(model_path, "CUBE") == 0) {
            out->raw_model = LoadModelFromMesh(GenMeshCube(1, 1, 1));
        }
        else if (strcmp(model_path, "PLANE") == 0) {
            out->raw_model = LoadModelFromMesh(GenMeshPlane(1, 1, 10, 10));
        } else {
            out->raw_model = LoadModel(model_path);
        }
        out->model = render_make_model(out->raw_model);
    }
    out->is_camera = parser_parsed_prefab_is_camera(parsed);
    hstrArray scripts = parser_parsed_prefab_get_scripts(parsed);
    out->n_scripts = hstr_array_len(scripts);
    assert(out->scripts = malloc(sizeof(enum fluxScriptID) * out->n_scripts));
    for (int i = 0; i < out->n_scripts; i++) {
        out->scripts[i] =
            flux_script_name_to_enum(hstr_unpack(hstr_array_get(scripts, i)));
    }
    out->projection = parser_parsed_prefab_get_projection(parsed);
    out->fov = parser_parsed_prefab_get_fov(parsed);
    out->tint = parser_parsed_prefab_get_tint(parsed);
    return out;
}

/**
 * @brief Frees resources associated with a prefab and deletes it.
 * @param prefab Pointer to the prefab to delete.
 */
void flux_delete_prefab(fluxPrefab prefab) {
    LOG_FUNC_CALL();
    assert(prefab);
    hstr_decref(prefab->name);
    if (prefab->has_model) {
        UnloadModel(prefab->raw_model);
        render_free_model(prefab->model);
    }
    if (prefab->scripts)
        free(prefab->scripts);
    free(prefab);
}

Color flux_prefab_get_tint(fluxPrefab prefab){
    return prefab->tint;
}