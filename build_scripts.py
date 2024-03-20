import sys
import os
import re
from pputils import *

SCRIPT_CALLBACKS = [
    "onUpdate", "afterUpdate", "onInit", "onDestroy", "onDraw", "onDraw2D"
]

# processes all Flux scripts and creates `struct script`
class ScriptProcessor:
    def __init__(self, project_path : str = "project", scripts_folder : str = "scripts", engine_path : str = "engine", output_file : str = "GENERATED_SCRIPTS.h"):
        # path of the project (where project_path/scripts is where all the scripts are)
        self.project_path : str = project_path
        # name of the scripts folder
        self.scripts_folder : str = scripts_folder
        # path of the scripts folder
        self.scripts_path : str = os.path.join(self.project_path,self.scripts_folder)
        # path of the engine sources
        self.engine_path : str = engine_path
        # name of the output scripts file
        self.output_file : str = output_file
        # path of the output scripts file
        self.output_path : str = os.path.join(self.engine_path,output_file)
        # list of all script files
        self.scripts : list[str] = self.find_scripts()
        # empty list of script names
        # when we process a script, we add the name of it to this list
        self.script_names : list[str] = []
        # the output generated file (initially empty)
        self.output : str = '#include "gameobject.h"\n#include "sceneallocator.h"\n' + "#ifdef FLUX_SCRIPTS_IMPLEMENTATION\n"
        # now we can process all the scripts
        self.process_scripts()
        # we can then add the script id enum to the start of output
        self.output += "\n#endif\n" + self.generate_enum_script_id() + self.generate_struct_script() + self.generate_all_callbacks() + self.generate_script_allocator()
        # we also want to forward declare all data scripts
        self.output = self.generate_forward_declarations() + self.output
        # now write to the output file
        with open(self.output_path,"w") as f:
            f.write(self.output)

    # finds all `.c` files in `scripts_path`
    def find_scripts(self) -> list[str]:
        return [os.path.join(self.scripts_path,i) for i in os.listdir(self.scripts_path) if i.split(".")[-1].strip() == "c"]

    # finds not implemented callbacks in a loaded script
    # this just looks for instances of the callback name,
    # SO, might get things wrong - be careful!
    # should probably change this in the future, so
    # TODO: fix me...
    def find_not_implemented(self, raw : str) -> list[str]:
        return [i for i in SCRIPT_CALLBACKS if not i in raw]

    # given a callback name, return an empty `implementation`
    # i.e., a function that doesn't do anything
    def get_empty_implementation(self,callback : str) -> str:
        return "fluxCallback {0}(fluxGameObject obj, script_data* data){{}}".format(callback)

    # gets implementations for all not implemented callbacks
    def get_extra_implementations(self, raw : str) -> str:
        return "\n".join([self.get_empty_implementation(i) for i in self.find_not_implemented(raw)])

    # parses the script name from the raw text
    # this is what SCRIPT is defined to at the start of the script,
    # so we can do two simple `splits` and a strip to get it
    def parse_script_name(self, raw : str) -> str:
        return raw.split("#define SCRIPT")[1].split("\n")[0].strip()

    # processes a single script file
    def process_file(self, path : str) -> None:
        with open(path,"r") as f:
            raw = f.read()
        raw += "\n\n" + self.get_extra_implementations(raw) + "\n\n"
        self.output += raw
        self.script_names.append(self.parse_script_name(raw))

    # process all scripts
    def process_scripts(self) -> None:
        for i in self.scripts:
            self.process_file(i)

    # generates `enum fluxScriptID`
    def generate_enum_script_id(self):
        return "\nenum fluxScriptID{" + ",".join([get_script_enum_name(i) for i in self.script_names]) + "};\n"

    def get_script_data_name(self,script_name : str) -> str:
        return script_name + "_fluxData"

    # generates `struct fluxScriptStruct`
    def generate_struct_script(self) -> str:
        return """

struct fluxScriptStruct;
typedef struct fluxScriptStruct* fluxScript;
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
struct fluxScriptStruct{
    enum fluxScriptID id;
    union {
        void* raw;
        """ + ";\n        ".join(["struct " + self.get_script_data_name(i) + "* " + self.get_script_data_name(i) for i in self.script_names]) + """;
    };
};
#endif

"""

    # gets the mangled callback function for a script_name
    def get_mangled_callback(self, callback : str, script_name : str):
        return script_name + "_fluxCallback_" + callback

    # generates the switch statement callback `callback` for the script `script_name`
    def generate_switch_script_callback(self,callback : str, script_name : str) -> str:
        return """
        case {0}:
            {1}(obj,script->{2});
            break;
""".format(get_script_enum_name(script_name),self.get_mangled_callback(callback,script_name),self.get_script_data_name(script_name))

    # generates the callback `callback` for `struct fluxScriptStruct`
    def generate_callback(self,callback : str) -> str:
        return """

void fluxCallback_{0}(fluxGameObject obj, fluxScript script)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{{
    switch(script->id){{
        {1}
        default:
            assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }}
}}
#else
;
#endif

""".format(callback,"\n".join([self.generate_switch_script_callback(callback,i) for i in self.script_names]))

    # generates all callbacks
    def generate_all_callbacks(self) -> str:
        return "\n".join([self.generate_callback(i) for i in SCRIPT_CALLBACKS])

    # forward declares the script data structs
    def generate_forward_declarations(self) -> str:
        return "\n" + "\n".join(["struct " + self.get_script_data_name(i) + ";" for i in self.script_names]) + "\n"

    # generates the script allocator
    def generate_script_allocator(self) -> str:
        return """

fluxScript fluxAllocateScript(enum fluxScriptID id)
#ifdef FLUX_SCRIPTS_IMPLEMENTATION
{
    fluxScript out = (fluxScript)flux_scene_alloc(sizeof(struct fluxScriptStruct));
    out->id = id;
    size_t sz = 0;
    switch(id){
        """ + "\n        ".join(["case " + get_script_enum_name(i) + ":\n            sz = sizeof(struct " + self.get_script_data_name(i) + ");\n            break;" for i in self.script_names]) + """
        default:
            assert((1 == 0) && "something terrible happened at build time!");
            break;
    }
    out->raw = flux_scene_alloc(sz);
    return out;
}
#else
;
#endif

"""

processor = ScriptProcessor()
#print(processor.generate_callback(SCRIPT_CALLBACKS[0]))
#print(processor.script_names)