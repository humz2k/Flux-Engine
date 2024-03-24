#ifndef _FLUX_EDITOR_DISPLAY_SIZE_H_
#define _FLUX_EDITOR_DISPLAY_SIZE_H_

#include "raylib.h"

static int GetDisplayWidth(){
    if (IsWindowFullscreen()){
        return GetRenderWidth();
    }
    return GetScreenWidth();
}

static int GetDisplayHeight(){
    if (IsWindowFullscreen()){
        return GetRenderHeight();
    }
    return GetScreenHeight();
}

#endif