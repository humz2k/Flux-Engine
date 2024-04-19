#ifndef _FLUX_GAMEOBJECT_H_
#define _FLUX_GAMEOBJECT_H_

// gameobject struct
struct fluxGameObjectStruct;
typedef struct fluxGameObjectStruct* fluxGameObject;

#ifndef FLUX_GAMEOBJECT_TYPE_ONLY

#include "raylib.h"

#include "transform.h"

#include "scripts.h"

fluxGameObject flux_allocate_gameobject(const char* name, const char* tag,
                                        fluxTransform transform);

void flux_gameobject_set_model(fluxGameObject obj, Model model, Color tint);

void flux_gameobject_set_camera(fluxGameObject obj, float fovy, int projection);

void flux_gameobject_add_script(fluxGameObject obj, enum fluxScriptID script);

void flux_gameobject_add_child(fluxGameObject obj, fluxGameObject child);

bool flux_gameobject_is_camera(fluxGameObject obj);

bool flux_gameobject_has_model(fluxGameObject obj);

int flux_gameobject_get_id(fluxGameObject obj);

Model flux_gameobject_get_model(fluxGameObject obj);

fluxTransform flux_gameobject_get_transform(fluxGameObject obj);

Color flux_gameobject_get_tint(fluxGameObject obj);

Camera3D flux_gameobject_get_raylib_camera(fluxGameObject obj);

void flux_gameobject_draw(fluxGameObject obj);

#endif
#endif