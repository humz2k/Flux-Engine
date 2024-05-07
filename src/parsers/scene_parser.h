/**
 * @file scene_parser.h
 **/

#ifndef _PARSER_SCENE_H_
#define _PARSER_SCENE_H_

#include "hqtools/hqtools.h"
#include "prefab_parser.h"
#include "transform.h"

struct fluxParsedSceneStruct;
typedef struct fluxParsedSceneStruct* fluxParsedScene;

struct fluxParsedGameObjectStruct;
typedef struct fluxParsedGameObjectStruct* fluxParsedGameObject;

fluxParsedScene parser_read_scene(const char* raw_path);
void parser_delete_parsed_scene(fluxParsedScene scene);

hstr parser_parsed_scene_get_name(fluxParsedScene scene);

int parser_parsed_scene_get_n_prefabs(fluxParsedScene scene);

fluxParsedPrefab parser_parsed_scene_get_prefab(fluxParsedScene scene, int i);

int parser_parsed_scene_get_n_gameobjects(fluxParsedScene scene);

fluxParsedGameObject parser_parsed_scene_get_gameobject(fluxParsedScene scene,
                                                        int i);

hstr parser_parsed_gameobject_get_prefab_name(fluxParsedGameObject gameobject);

fluxTransform
parser_parsed_gameobject_get_transform(fluxParsedGameObject gameobject);

hstrArray parser_parsed_gameobject_get_args(fluxParsedGameObject gameobject);

#endif
