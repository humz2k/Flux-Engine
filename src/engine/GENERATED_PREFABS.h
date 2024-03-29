
#ifndef FLUX_PRIVATE_PREFABS
enum fluxPrefabID{
    fluxEmptyPrefab,
    fluxPrefab_testPrefab = 0,
    fluxPrefab_defaultCamera = 1
};
#endif

#ifdef FLUX_PRIVATE_PREFABS
static void init_all_prefabs(void){
    TraceLog(LOG_INFO,"init_all_prefabs called");

    enum fluxScriptID fluxPrefab_testPrefab_scripts[] = {fluxEmptyScript,fluxScript_test};
    enum fluxPrefabID fluxPrefab_testPrefab_children[] = {fluxEmptyPrefab,};
    flux_register_prefab("testPrefab","default","SPHERE",0,1,fluxPrefab_testPrefab_scripts,0,fluxPrefab_testPrefab_children);

    enum fluxScriptID fluxPrefab_defaultCamera_scripts[] = {fluxEmptyScript,};
    enum fluxPrefabID fluxPrefab_defaultCamera_children[] = {fluxEmptyPrefab,};
    flux_register_prefab("defaultCamera","default",NULL,1,0,fluxPrefab_defaultCamera_scripts,0,fluxPrefab_defaultCamera_children);
}
#endif
