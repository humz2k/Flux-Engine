#ifndef _FLUX_SCRIPTS_H_
#define _FLUX_SCRIPTS_H_

#include "hqtools/hqtools.h"

#undef FLUX_SCRIPTS_IMPLEMENTATION
#include "GENERATED_SCRIPTS.h"

enum fluxScriptID flux_script_name_to_enum(const char* name);

#endif