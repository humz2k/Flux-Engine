#ifndef _PARSER_PREFAB_H_
#define _PARSER_PREFAB_H_

struct fluxPrefabStruct;
typedef struct fluxPrefabStruct* fluxPrefab;

struct fluxParsedPrefabStruct;
typedef struct fluxParsedPrefabStruct* fluxParsedPrefab;

void parser_delete_parsed_prefab(fluxParsedPrefab prefab);
fluxParsedPrefab parser_read_prefab(const char* raw_path);

#endif