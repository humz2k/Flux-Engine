#ifndef _FLUX_PREFAB_H_
#define _FLUX_PREFAB_H_

#include "gameobject.h"
#include "transform.h"

#include "GENERATED_PREFABS.h"

void flux_init_prefabs(void);

fluxGameObject flux_instantiate_prefab(enum fluxPrefabID prefabid,
                                       fluxTransform transform);

#endif