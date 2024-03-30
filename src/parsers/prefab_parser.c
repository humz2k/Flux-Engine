#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "transform.h"
#include "hqtools/hqtools.h"
#include "prefab.h"

/*! \struct fluxParsedPrefabStruct
 * A parsed prefab
 */
typedef struct fluxParsedPrefabStruct{
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

    /*! \brief the names of any scripts attached to this prefab */
    hstrArray scripts;

    /*! \brief the names of the children of this prefab */
    hstrArray children;

    /*! \brief the initial transform of this prefab (do I need this???) */
    fluxTransform transform;
} fluxParsedPrefabStruct;

typedef fluxParsedPrefabStruct* fluxParsedPrefab;

static fluxParsedPrefab alloc_parsed_prefab_internal(void){
    fluxParsedPrefab out = (fluxParsedPrefab)malloc(sizeof(fluxParsedPrefabStruct));
    memset(out,0,sizeof(fluxParsedPrefabStruct));
    out->scripts = hstr_array_make();
    out->children = hstr_array_make();
    return out;
}

static void parsed_prefab_set_path(fluxParsedPrefab prefab, hstr path){
    assert(prefab);
    assert(path);
    if (prefab->path){
        hstr_decref(prefab->path);
    }
    prefab->path = hstr_incref(path);
}

static void parsed_prefab_set_name(fluxParsedPrefab prefab, hstr name){
    assert(prefab);
    assert(name);
    if (prefab->name){
        hstr_decref(prefab->name);
    }
    prefab->name = hstr_incref(name);
}

static void parsed_prefab_set_model_path(fluxParsedPrefab prefab, hstr model_path){
    assert(prefab);
    assert(model_path);
    if (prefab->has_model){
        assert(prefab->model_path);
        hstr_decref(prefab->model_path);
    }
    assert(!prefab->is_camera);
    prefab->has_model = true;
    prefab->model_path = hstr_incref(model_path);
}

static void parsed_prefab_set_is_camera(fluxParsedPrefab prefab){
    assert(prefab);
    assert(!prefab->has_model);
    prefab->is_camera = true;
}

static void parsed_prefab_add_script(fluxParsedPrefab prefab, hstr script){
    assert(prefab);
    assert(script);
    assert(prefab->scripts);
    hstr_array_append(prefab->scripts,script);
}

static void parsed_prefab_add_child(fluxParsedPrefab prefab, hstr child){
    assert(prefab);
    assert(child);
    assert(prefab->children);
    hstr_array_append(prefab->children,child);
}

void parser_delete_parsed_prefab(fluxParsedPrefab prefab){
    TraceLog(LOG_INFO,"deleting parsed prefab");
    assert(prefab);
    if (prefab->path)hstr_decref(prefab->path);
    if (prefab->name)hstr_decref(prefab->name);
    if (prefab->model_path)hstr_decref(prefab->model_path);

    assert(prefab->scripts);
    hstr_array_delete(prefab->scripts);

    assert(prefab->children);
    hstr_array_delete(prefab->children);
    free(prefab);
}

static size_t get_file_length(FILE* fptr){
    assert(fptr);
    fseek(fptr, 0L, SEEK_END);
    size_t sz = ftell(fptr);
    rewind(fptr);
    return sz;
}

static hstr read_whole_file(FILE* fptr){
    hstr file_str = hstr_new("");

    char buffer[100];
    while (fgets(buffer,100,fptr)){
        file_str = hstr_concat(file_str,hstr_new(buffer));
    }
    return file_str;
}

fluxParsedPrefab parser_read_prefab(const char* raw_path){
    hstr path = hstr_incref(hstr_new(raw_path));
    fluxParsedPrefab out = alloc_parsed_prefab_internal();

    FILE *fptr;
    TraceLog(LOG_INFO,"opening %s",hstr_unpack(path));
    fptr = fopen(hstr_unpack(path), "r");
    if (!fptr){
        TraceLog(LOG_ERROR,"could not open %s",hstr_unpack(path));
        goto cleanup;
    }
    TraceLog(LOG_INFO,"opened %s",hstr_unpack(path));

    size_t sz = get_file_length(fptr);
    TraceLog(LOG_INFO,"file is %lu bytes",sz);

    hstr file_str = hstr_incref(read_whole_file(fptr));

    hstrArray lines = hstr_split(file_str,"\n");

    for (int i = 0; i < hstr_array_len(lines); i++){
        hstr line = hstr_incref(hstr_array_get(lines,i));

        hstrArray arguments = hstr_split(line,":");

        if (hstr_array_len(arguments) >= 2){

            TraceLog(LOG_INFO,"ARGUMENT: %s : %s",hstr_unpack(hstr_array_get(arguments,0)),hstr_unpack(hstr_array_get(arguments,1)));

        }
        //TraceLog(LOG_INFO,"line %d: %s",i,hstr_unpack(line));

        hstr_array_delete(arguments);
        hstr_decref(line);
    }

    hstr_array_delete(lines);
    hstr_decref(file_str);

    TraceLog(LOG_INFO,"closing %s",hstr_unpack(path));
    if (fclose(fptr)){
        TraceLog(LOG_ERROR,"close %s failed...",hstr_unpack(path));
    } else{
        TraceLog(LOG_INFO,"closed %s",hstr_unpack(path));
    }
    cleanup:
    hstr_decref(path);
    return out;
}
