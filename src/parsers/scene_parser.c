#include "scene_parser.h"
#include "file_tools.h"
#include "hqtools/hqtools.h"
#include "prefab_parser.h"
#include "raylib.h"
#include "transform.h"
#include <stdio.h>
#include <stdlib.h>

/*! \struct fluxParsedSceneStruct
 * A parsed scene
 */
typedef struct fluxParsedSceneStruct {
    /*! \brief path to the scene */
    hstr path;

    /*! \brief name of the scene */
    hstr name;

    // prefabs in scene
    int n_prefabs;
    fluxParsedPrefab* prefabs;

} fluxParsedSceneStruct;

static fluxParsedScene alloc_parsed_scene_internal(void) {
    fluxParsedScene out =
        (fluxParsedScene)malloc(sizeof(fluxParsedSceneStruct));
    memset(out, 0, sizeof(fluxParsedSceneStruct));
    out->n_prefabs = 0;
    out->prefabs = NULL;
    return out;
}

static void parsed_scene_set_path(fluxParsedScene scene, hstr path) {
    assert(scene);
    assert(path);
    if (scene->path)
        hstr_decref(scene->path);
    scene->path = hstr_incref(path);
}

static void parsed_scene_set_name(fluxParsedScene scene, hstr name) {
    assert(scene);
    assert(name);
    if (scene->name)
        hstr_decref(scene->name);
    scene->name = hstr_incref(name);
}

static void parsed_scene_add_prefab(fluxParsedScene scene,
                                    fluxParsedPrefab prefab) {
    assert(scene);
    assert(prefab);
    assert(scene->n_prefabs >= 0);
    scene->n_prefabs++;
    scene->prefabs =
        realloc(scene->prefabs, sizeof(fluxParsedPrefab) * scene->n_prefabs);
    scene->prefabs[scene->n_prefabs - 1] = prefab;
}

void parser_delete_parsed_scene(fluxParsedScene scene) {
    assert(scene);
    if (scene->path)
        hstr_decref(scene->path);
    if (scene->name)
        hstr_decref(scene->name);
    for (int i = 0; i < scene->n_prefabs; i++) {
        parser_delete_parsed_prefab(scene->prefabs[i]);
    }
    free(scene->prefabs);
    free(scene);
}

fluxParsedScene parser_read_scene(const char* raw_path) {
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

            hstr argument_list = hstr_split(argument, ",");

            // TraceLog(LOG_INFO,"ARGUMENT: %s :
            // %s",hstr_unpack(command),hstr_unpack(argument));

            if (strcmp(hstr_unpack(command), "sceneName") == 0) {
                parsed_scene_set_name(out, argument);
            } else if (strcmp(hstr_unpack(command), "scenePrefabs") == 0) {
                for (int k = 0; k < hstr_array_len(argument_list); k++) {
                    hstr prefab_path = hstr_incref(
                        hstr_strip(hstr_array_get(argument_list, k)));
                    // parsed_prefab_add_script(out, script_name);
                    fluxParsedPrefab prefab =
                        parser_read_prefab(hstr_unpack(prefab_path));
                    parsed_scene_add_prefab(out, prefab);
                    hstr_decref(prefab_path);
                }
            } /* else if (strcmp(hstr_unpack(command), "prefabChildren") == 0) {
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
             }*/

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
cleanup:
    hstr_decref(path);
    return out;
}