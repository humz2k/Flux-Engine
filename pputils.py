# gets the name of the script enum of `script_name`
def get_script_enum_name(script_name : str) -> str:
    return "fluxScript_" + script_name

def mangle_prefab_name(name : str) -> str:
        return "fluxPrefab_" + name