#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "transform.h"
#include "hqtools/hqtools.h"
#include "scene_parser.h"
#include "prefab_parser.h"

/*! \struct fluxParsedSceneStruct
 * A parsed scene
 */
typedef struct fluxParsedSceneStruct{

    // prefabs in scene
    int n_prefabs;
    fluxParsedPrefab* prefabs;

} fluxParsedSceneStruct;