#include "scene_parser.h"
#include "hqtools/hqtools.h"
#include "prefab_parser.h"
#include "raylib.h"
#include "transform.h"
#include <stdio.h>
#include <stdlib.h>

/*! \struct fluxParsedSceneStruct
 * A parsed scene
 */
typedef struct fluxParsedSceneStruct {

    // prefabs in scene
    int n_prefabs;
    fluxParsedPrefab* prefabs;

} fluxParsedSceneStruct;