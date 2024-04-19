#ifndef _PARSER_SCENE_H_
#define _PARSER_SCENE_H_

struct fluxParsedSceneStruct;
typedef struct fluxParsedSceneStruct* fluxParsedScene;

fluxParsedScene parser_read_scene(const char* raw_path);
void parser_delete_parsed_scene(fluxParsedScene scene);

#endif