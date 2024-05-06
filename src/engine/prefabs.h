#ifndef _FLUX_PREFABS_H_
#define _FLUX_PREFABS_H_

#include "pipeline.h"
#include "prefab_parser.h"
#include "scripts.h"

struct fluxPrefabStruct;
typedef struct fluxPrefabStruct* fluxPrefab;

fluxPrefab flux_load_prefab(fluxParsedPrefab parsed);

void flux_delete_prefab(fluxPrefab prefab);

hstr flux_prefab_get_name(fluxPrefab prefab);

bool flux_prefab_has_model(fluxPrefab prefab);

Model flux_prefab_get_raw_model(fluxPrefab prefab);

renderModel flux_prefab_get_model(fluxPrefab prefab);

bool flux_prefab_is_camera(fluxPrefab prefab);

int flux_prefab_get_n_scripts(fluxPrefab prefab);

int flux_prefab_get_projection(fluxPrefab prefab);

void flux_prefab_set_projection(fluxPrefab prefab, int projection);

float flux_prefab_get_fov(fluxPrefab prefab);

void flux_prefab_set_fov(fluxPrefab prefab, float fov);

enum fluxScriptID* flux_prefab_get_scripts(fluxPrefab prefab);

#endif