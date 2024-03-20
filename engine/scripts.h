//#ifndef _SCRIPTS_H_
//#define _SCRIPTS_H_

#include "gameobject.h"
#include <assert.h>

#define fluxConcat_(X,Y) X ## _ ## Y
#define fluxConcat(X,Y) fluxConcat_(X,Y)

extern float deltaTime;

#ifdef SCRIPT

#define onUpdate fluxConcat(SCRIPT,fluxCallback_onUpdate)
#define afterUpdate fluxConcat(SCRIPT,fluxCallback_afterUpdate)
#define onInit fluxConcat(SCRIPT,fluxCallback_onInit)
#define onDestroy fluxConcat(SCRIPT,fluxCallback_onDestroy)
#define onDraw fluxConcat(SCRIPT,fluxCallback_onDraw)
#define onDraw2D fluxConcat(SCRIPT,fluxCallback_onDraw2D)
#define script_data struct fluxConcat(SCRIPT,fluxData)

#endif

//struct script* get_script(GameObject* obj,enum script_enum script_name);

//#endif