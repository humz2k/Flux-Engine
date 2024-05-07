/**
 * @file prefab_parser.h
 **/

#ifndef _PARSER_PREFAB_H_
#define _PARSER_PREFAB_H_

#include "hqtools/hqtools.h"

struct fluxParsedPrefabStruct;
typedef struct fluxParsedPrefabStruct* fluxParsedPrefab;

void parser_delete_parsed_prefab(fluxParsedPrefab prefab);
fluxParsedPrefab parser_read_prefab(const char* raw_path);

hstr parser_parsed_prefab_get_path(fluxParsedPrefab prefab);

hstr parser_parsed_prefab_get_name(fluxParsedPrefab prefab);

bool parser_parsed_prefab_has_model(fluxParsedPrefab prefab);

hstr parser_parsed_prefab_get_model_path(fluxParsedPrefab prefab);

bool parser_parsed_prefab_is_camera(fluxParsedPrefab prefab);

hstrArray parser_parsed_prefab_get_scripts(fluxParsedPrefab prefab);

float parser_parsed_prefab_get_fov(fluxParsedPrefab prefab);

int parser_parsed_prefab_get_projection(fluxParsedPrefab prefab);

void parser_parsed_prefab_set_fov(fluxParsedPrefab prefab, float fov);

void parser_parsed_prefab_set_projection(fluxParsedPrefab prefab,
                                         int projection);

#endif