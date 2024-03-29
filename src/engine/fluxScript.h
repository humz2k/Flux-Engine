#include "gameobject.h"
#include <assert.h>

#define fluxConcat_(X, Y) X##_##Y
#define fluxConcat(X, Y) fluxConcat_(X, Y)

extern float deltaTime;

#ifdef SCRIPT

#define fluxCallback static inline void

#define onUpdate fluxConcat(SCRIPT, fluxCallback_onUpdate)
#define afterUpdate fluxConcat(SCRIPT, fluxCallback_afterUpdate)
#define onInit fluxConcat(SCRIPT, fluxCallback_onInit)
#define onDestroy fluxConcat(SCRIPT, fluxCallback_onDestroy)
#define onDraw fluxConcat(SCRIPT, fluxCallback_onDraw)
#define onDraw2D fluxConcat(SCRIPT, fluxCallback_onDraw2D)
#define script_data struct fluxConcat(SCRIPT, fluxData)

#else

#define fluxCallback DID_YOU_FORGET_TO_DEFINE_SCRIPT

#define onUpdate DID_YOU_FORGET_TO_DEFINE_SCRIPT
#define afterUpdate DID_YOU_FORGET_TO_DEFINE_SCRIPT
#define onInit DID_YOU_FORGET_TO_DEFINE_SCRIPT
#define onDestroy DID_YOU_FORGET_TO_DEFINE_SCRIPT
#define onDraw DID_YOU_FORGET_TO_DEFINE_SCRIPT
#define onDraw2D DID_YOU_FORGET_TO_DEFINE_SCRIPT
#define script_data DID_YOU_FORGET_TO_DEFINE_SCRIPT

#endif