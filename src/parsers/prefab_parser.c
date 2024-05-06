#include "prefab_parser.h"
#include "file_tools.h"
#include "hqtools/hqtools.h"
#include "raylib.h"
#include "transform.h"
#include <stdio.h>
#include <stdlib.h>

/*! \struct fluxParsedPrefabStruct
 * A parsed prefab
 */
typedef struct fluxParsedPrefabStruct {
    /*! \brief the path that this prefab was parsed from */
    hstr path;

    /*! \brief the name of this prefab (used to refer to it to instantiate) */
    hstr name;

    /*! \brief whether this prefab has a model */
    bool has_model;

    /*! \brief the path to the model of this prefab */
    hstr model_path;

    /*! \brief whether this prefab is a camera */
    bool is_camera;

    float fov;
    int projection;

    /*! \brief the names of any scripts attached to this prefab */
    hstrArray scripts;

    /*! \brief the names of the children of this prefab */
    hstrArray children;

} fluxParsedPrefabStruct;

float parser_parsed_prefab_get_fov(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->fov;
}

int parser_parsed_prefab_get_projection(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->projection;
}

void parser_parsed_prefab_set_fov(fluxParsedPrefab prefab, float fov) {
    assert(prefab);
    prefab->fov = fov;
}

void parser_parsed_prefab_set_projection(fluxParsedPrefab prefab,
                                         int projection) {
    assert(prefab);
    prefab->projection = projection;
}

hstr parser_parsed_prefab_get_path(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->path;
}

hstr parser_parsed_prefab_get_name(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->name;
}

bool parser_parsed_prefab_has_model(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->has_model;
}

hstr parser_parsed_prefab_get_model_path(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->model_path;
}

bool parser_parsed_prefab_is_camera(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->is_camera;
}

hstrArray parser_parsed_prefab_get_scripts(fluxParsedPrefab prefab) {
    assert(prefab);
    return prefab->scripts;
}

static fluxParsedPrefab alloc_parsed_prefab_internal(void) {
    fluxParsedPrefab out =
        (fluxParsedPrefab)malloc(sizeof(fluxParsedPrefabStruct));
    memset(out, 0, sizeof(fluxParsedPrefabStruct));
    out->scripts = hstr_array_make();
    out->children = hstr_array_make();
    out->fov = 45;
    out->projection = CAMERA_PERSPECTIVE;
    return out;
}

static void parsed_prefab_set_path(fluxParsedPrefab prefab, hstr path) {
    assert(prefab);
    assert(path);
    if (prefab->path) {
        hstr_decref(prefab->path);
    }
    prefab->path = hstr_incref(path);
    TraceLog(LOG_INFO, "setting prefab->path = %s", hstr_unpack(prefab->path));
}

static void parsed_prefab_set_name(fluxParsedPrefab prefab, hstr name) {
    assert(prefab);
    assert(name);
    if (prefab->name) {
        hstr_decref(prefab->name);
    }
    prefab->name = hstr_incref(name);
    TraceLog(LOG_INFO, "setting prefab->name = %s", hstr_unpack(prefab->name));
}

static void parsed_prefab_set_model_path(fluxParsedPrefab prefab,
                                         hstr model_path) {
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

static void parsed_prefab_set_is_camera(fluxParsedPrefab prefab) {
    assert(prefab);
    assert(!prefab->has_model);
    prefab->is_camera = true;
    TraceLog(LOG_INFO, "setting prefab->is_camera = true");
}

static void parsed_prefab_add_script(fluxParsedPrefab prefab, hstr script) {
    assert(prefab);
    assert(script);
    assert(prefab->scripts);
    hstr_array_append(prefab->scripts, script);
    TraceLog(LOG_INFO, "adding prefab script = %s", hstr_unpack(script));
}

static void parsed_prefab_add_child(fluxParsedPrefab prefab, hstr child) {
    assert(prefab);
    assert(child);
    assert(prefab->children);
    hstr_array_append(prefab->children, child);
    TraceLog(LOG_INFO, "adding prefab child = %s", hstr_unpack(child));
}

void parser_delete_parsed_prefab(fluxParsedPrefab prefab) {
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

fluxParsedPrefab parser_read_prefab(const char* raw_path) {
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

            hstr argument_list = hstr_split(argument, ",");

            // TraceLog(LOG_INFO,"ARGUMENT: %s :
            // %s",hstr_unpack(command),hstr_unpack(argument));

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
