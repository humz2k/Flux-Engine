#include "hqtools/hqtools.h"

#include <string.h>

#define FLUX_SCRIPTS_IMPLEMENTATION
#include "GENERATED_SCRIPTS.h"

enum fluxScriptID flux_script_name_to_enum(const char* name) {
    assert(name);
    for (int i = 0; i < (sizeof(SCRIPT_NAME_TO_ENUM) / sizeof(const char*));
         i++) {
        if (strcmp(name, SCRIPT_NAME_TO_ENUM[i]) == 0) {
            return i;
        }
    }
    return fluxEmptyScript;
}