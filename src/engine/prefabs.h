#ifndef _FLUX_PREFABS_H_
#define _FLUX_PREFABS_H_

#include "prefab_parser.h"

struct fluxPrefabStruct;
typedef struct fluxPrefabStruct* fluxPrefab;

fluxPrefab flux_load_prefab(fluxParsedPrefab parsed);

void flux_delete_prefab(fluxPrefab prefab);

#endif