#ifndef _FLUX_GAMEOBJECT_H_
#define _FLUX_GAMEOBJECT_H_

// gameobject struct
struct fluxGameObjectStruct;
typedef struct fluxGameObjectStruct* fluxGameObject;

#ifndef FLUX_GAMEOBJECT_TYPE_ONLY

#include "raylib.h"

#include "transform.h"

#include "scripts.h"

#include "pipeline.h"

#include "prefabs.h"

fluxGameObject flux_allocate_gameobject(int id, fluxTransform transform, fluxPrefab prefab);

void flux_destroy_gameobject(fluxGameObject obj);

int flux_gameobject_get_id(fluxGameObject obj);

fluxTransform flux_gameobject_get_transform(fluxGameObject obj);

void flux_gameobject_set_transform(fluxGameObject obj, fluxTransform transform);

int flux_gameobject_get_n_scripts(fluxGameObject obj);

bool flux_gameobject_is_camera(fluxGameObject obj);

renderModel flux_gameobject_get_model(fluxGameObject obj);

bool flux_gameobject_has_model(fluxGameObject obj);

Camera3D flux_gameobject_get_raylib_camera(fluxGameObject obj);

#endif
#endif