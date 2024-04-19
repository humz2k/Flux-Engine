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
} fluxPrefabStruct;

fluxPrefab flux_load_prefab(fluxParsedPrefab parsed) {
    assert(parsed);
    fluxPrefab out;
    assert(out = (fluxPrefab)malloc(sizeof(fluxPrefabStruct)));
    memset(out, 0, sizeof(fluxPrefabStruct));
    out->name = hstr_incref(parser_parsed_prefab_get_name(parsed));
    out->has_model = parser_parsed_prefab_has_model(parsed);
    if (out->has_model) {
        const char* model_path =
            hstr_unpack(parser_parsed_prefab_get_model_path(parsed));
        if (strcmp(model_path, "SPHERE") == 0) {
            out->raw_model = LoadModelFromMesh(GenMeshSphere(1, 10, 10));
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
    return out;
}

void flux_delete_prefab(fluxPrefab prefab) {
    assert(prefab);
    hstr_decref(prefab->name);
    if (prefab->has_model) {
        UnloadModel(prefab->raw_model);
        render_free_model(prefab->model);
    }
    if (prefab->n_scripts > 0)
        free(prefab->scripts);
    free(prefab);
}