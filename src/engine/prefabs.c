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
 **/

#include "prefabs.h"
#include "hqtools/hqtools.h"
#include "pipeline.h"
#include "prefab_parser.h"
#include "scripts.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct fluxPrefabStruct {
    hstr name;
    bool has_model;
    Model raw_model;
    renderModel model;
    bool is_camera;
    int n_scripts;
    enum fluxScriptID* scripts;
    int projection;
    float fov;
} fluxPrefabStruct;

hstr flux_prefab_get_name(fluxPrefab prefab) {
    assert(prefab);
    return prefab->name;
}

int flux_prefab_get_projection(fluxPrefab prefab) {
    assert(prefab);
    return prefab->projection;
}

void flux_prefab_set_projection(fluxPrefab prefab, int projection) {
    assert(prefab);
    prefab->projection = projection;
}

float flux_prefab_get_fov(fluxPrefab prefab) {
    assert(prefab);
    return prefab->fov;
}

void flux_prefab_set_fov(fluxPrefab prefab, float fov) {
    assert(prefab);
    prefab->fov = fov;
}

bool flux_prefab_has_model(fluxPrefab prefab) {
    assert(prefab);
    return prefab->has_model;
}

Model flux_prefab_get_raw_model(fluxPrefab prefab) {
    assert(prefab);
    return prefab->raw_model;
}

renderModel flux_prefab_get_model(fluxPrefab prefab) {
    assert(prefab);
    return prefab->model;
}

bool flux_prefab_is_camera(fluxPrefab prefab) {
    assert(prefab);
    return prefab->is_camera;
}

int flux_prefab_get_n_scripts(fluxPrefab prefab) {
    assert(prefab);
    return prefab->n_scripts;
}

enum fluxScriptID* flux_prefab_get_scripts(fluxPrefab prefab) {
    assert(prefab);
    return prefab->scripts;
}

fluxPrefab flux_load_prefab(fluxParsedPrefab parsed) {
    assert(parsed);
    fluxPrefab out;
    assert(out = (fluxPrefab)malloc(sizeof(fluxPrefabStruct)));
    memset(out, 0, sizeof(fluxPrefabStruct));
    out->name = hstr_incref(parser_parsed_prefab_get_name(parsed));
    out->has_model = parser_parsed_prefab_has_model(parsed);
    // out->raw_model = NULL;
    out->model = NULL;
    if (out->has_model) {
        const char* model_path =
            hstr_unpack(parser_parsed_prefab_get_model_path(parsed));
        TraceLog(LOG_INFO, "loading model %s", model_path);
        if (strcmp(model_path, "SPHERE") == 0) {
            out->raw_model = LoadModelFromMesh(GenMeshSphere(1, 50, 50));
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
    return out;
}

void flux_delete_prefab(fluxPrefab prefab) {
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