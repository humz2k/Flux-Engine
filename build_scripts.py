import sys
import os
import re

SCRIPT_CALLBACKS = [
    "onUpdate", "afterUpdate", "onInit", "onDestroy", "onDraw", "onDraw2D"
]

# processes all Flux scripts and creates `struct script`
class ScriptProcessor:
    def __init__(self, project_path : str = "project", scripts_folder : str = "scripts", engine_path : str = "engine", output_file : str = "GENERATED_SCRIPTS.c"):
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
        self.output : str = ""
        # now we can process all the scripts
        self.process_scripts()
        # we can then add the script id enum to the start of output
        self.output += self.generate_enum_script_id() + self.generate_struct_script() + self.generate_all_callbacks()
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
        return "void {0}(fluxGameObject obj, script_data* data){{}}".format(callback)

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

    # gets the name of the script enum of `script_name`
    def get_script_enum_name(self,script_name : str) -> str:
        return "fluxScript_" + script_name

    # generates `enum fluxScriptID`
    def generate_enum_script_id(self):
        return "\nenum fluxScriptID{" + ",".join([self.get_script_enum_name(i) for i in self.script_names]) + "};\n"

    def get_script_data_name(self,script_name : str) -> str:
        return script_name + "_fluxData"

    # generates `struct fluxScriptStruct`
    def generate_struct_script(self) -> str:
        return """

struct fluxScriptStruct{
    enum fluxScriptID id;
    union {
        """ + ";\n        ".join(["struct " + self.get_script_data_name(i) + " " + self.get_script_data_name(i) for i in self.script_names]) + """;
    };
};

"""

    # gets the mangled callback function for a script_name
    def get_mangled_callback(self, callback : str, script_name : str):
        return callback + "_fluxCallback_" + script_name

    # generates the switch statement callback `callback` for the script `script_name`
    def generate_switch_script_callback(self,callback : str, script_name : str) -> str:
        return """
        case {0}:
            {1}(obj,&(script->{2}));
            break;
""".format(self.get_script_enum_name(script_name),self.get_mangled_callback(callback,script_name),self.get_script_data_name(script_name))

    # generates the callback `callback` for `struct fluxScriptStruct`
    def generate_callback(self,callback : str) -> str:
        return """

void fluxCallback_{0}(fluxGameObject obj, struct fluxScriptStruct* script){{
    switch(script->id){{
        {1}
        default:
            assert((1 == 0) && "something terrible happened at compile time!");
            break;
    }}
}}

""".format(callback,"\n".join([self.generate_switch_script_callback(callback,i) for i in self.script_names]))

    def generate_all_callbacks(self) -> str:
        return "\n".join([self.generate_callback(i) for i in SCRIPT_CALLBACKS])


processor = ScriptProcessor()
print(processor.output)
#print(processor.generate_callback(SCRIPT_CALLBACKS[0]))
#print(processor.script_names)