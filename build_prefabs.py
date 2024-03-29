import sys
import os
import re
import json
from pputils import *

class PrefabProcessor:
    def __init__(self, project_path : str = "project", prefabs_folder : str = "prefabs", engine_path : str = "src/engine", output_file : str = "GENERATED_PREFABS.h"):
        # path of the project (where project_path/scripts is where all the scripts are)
        self.project_path : str = project_path
        # name of the prefabs folder
        self.prefabs_folder : str = prefabs_folder
        # path of the scripts folder
        self.prefabs_path : str = os.path.join(self.project_path,self.prefabs_folder)
        # path of the engine sources
        self.engine_path : str = engine_path
        # name of the output prefabs file
        self.output_file : str = output_file
        # path of the output prefabs file
        self.output_path : str = os.path.join(self.engine_path,output_file)
        # all discovered prefabs
        self.prefabs : list[str] = self.find_prefabs()
        # number of unnamed prefabs
        self.n_prefabs : int = 0
        # list of prefab names
        self.prefab_names : list[str] = []
        # builders output
        self.builders : str = "\n#ifdef FLUX_PRIVATE_PREFABS\nstatic void init_all_prefabs(void){\n    " + 'TraceLog(LOG_INFO,"init_all_prefabs called");\n'

        for i in self.prefabs:
            self.process_prefab(i)

        self.builders += "}\n#endif\n"

        # enum output
        self.enums : str = "\n#ifndef FLUX_PRIVATE_PREFABS\nenum fluxPrefabID{\n    " + ",\n    ".join(["fluxEmptyPrefab"] + [i + " = " + str(idx) for idx,i in enumerate(self.prefab_names)]) + "\n};\n#endif\n"

        self.output = self.enums + self.builders
        with open(self.output_path,"w") as f:
            f.write(self.output)

    # finds all `.prefab` files in `prefabs_path`
    def find_prefabs(self) -> list[str]:
        return [os.path.join(self.prefabs_path,i) for i in os.listdir(self.prefabs_path) if i.split(".")[-1].strip() == "prefab"]

    # reads the prefab name from loaded json
    # or gives it a unique number if no name is specified
    def get_prefab_name(self,prefab) -> str:
        prefab_name = ""
        if "prefabName" in prefab:
            prefab_name = prefab["prefabName"]
        else:
            prefab_name = "unnamed" + str(self.n_unnamed_prefabs)
        self.n_prefabs += 1
        return prefab_name

    # reads the model path from loaded json
    # or sets it to NULL if no path is specified
    def get_prefab_model_path(self,prefab) -> str:
        model_path = prefab.get("prefabModel","").strip()
        if len(model_path) == 0:
            model_path = "NULL"
        else:
            model_path = '"' + model_path + '"'
        return model_path

    # reads the scripts from loaded json
    # or sets it to empty list if not specified
    def get_prefab_scripts(self,prefab) -> str:
        return [get_script_enum_name(i) for i in prefab.get("prefabScripts",[])]

    # processes a single prefab from its path
    def process_prefab(self,path : str):
        # read the prefab file
        with open(path,"r") as f:
            prefab = json.load(f)
        # add this prefab name to prefab_names
        raw_name = self.get_prefab_name(prefab)
        name = mangle_prefab_name(raw_name)
        self.prefab_names.append(name)

        # read relevant stuff from the prefab
        tag = prefab.get("prefabTag","default")
        model_path = self.get_prefab_model_path(prefab)
        is_camera = str(int(prefab.get("prefabIsCamera",False)))
        scripts = self.get_prefab_scripts(prefab)
        n_scripts = str(len(scripts))
        children = [mangle_prefab_name(i) for i in prefab.get("prefabChildren",[])]
        n_children = str(len(children))

        scripts_literal = "enum fluxScriptID " + name + "_scripts[] = {fluxEmptyScript," + ",".join(scripts) + "};"
        children_literal = "enum fluxPrefabID " + name + "_children[] = {fluxEmptyPrefab," + ",".join(children) + "};"
        call = "flux_register_prefab({0},{1},{2},{3},{4},{5},{6},{7});".format(
            '"' + raw_name + '"', '"' + tag + '"', model_path, is_camera, n_scripts, name + "_scripts", n_children, name + "_children"
        )
        self.builders += "\n    " + "\n    ".join([scripts_literal,children_literal,call]) + "\n"

PrefabProcessor()