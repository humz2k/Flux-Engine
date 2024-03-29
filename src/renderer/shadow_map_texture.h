#ifndef _SHADOW_MAP_TEXTURE_
#define _SHADOW_MAP_TEXTURE_

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

RenderTexture2D LoadShadowmapRenderTexture(int width, int height);

// Unload shadowmap render texture from GPU memory (VRAM)
void UnloadShadowmapRenderTexture(RenderTexture2D target);

#endif