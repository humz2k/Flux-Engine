from pyray import *
import pandas as pd
import numpy as np
import json
import os

class fluxTransform:
    def __init__(self,pos : Vector3, rot : Vector3, scale : Vector3):
        self.pos = pos
        self.rot = rot
        self.scale = scale

    def __str__(self):
        return str(self.pos.x) + "," + str(self.pos.y) + "," + str(self.pos.z) + "," + str(self.rot.x) + "," + str(self.rot.y) + "," + str(self.rot.z) + "," + str(self.scale.x) + "," + str(self.scale.y) + "," + str(self.scale.z)

def defaultTransform():
    return fluxTransform(vector3_zero(),vector3_zero(),vector3_one())

class fluxGameObject:
    def __init__(self, name : str, tag : str, transform : fluxTransform, model : Model = None, is_camera : bool = False, scripts : list[str] = [], children = []):
        global GAMEOBJECT_COUNTER
        self.id = GAMEOBJECT_COUNTER
        GAMEOBJECT_COUNTER += 1
        self.name = name
        self.tag = tag
        self.transform = transform
        self.model = model
        self.is_camera = is_camera
        self.scripts = scripts
        self.tint = WHITE
        self.children : list[fluxGameObject] = children

    def __eq__(self,other):
        return self.id == other.id

class fluxPrefab:
    def __init__(self, name : str, tag : str, model_path : str, is_camera : bool, scripts : list[str], children : list[str]):
        self.name = name
        self.tag = tag
        self.model_path = model_path
        self.is_camera = is_camera
        self.scripts = scripts
        self.children = children

    def instantiate(self,transform : fluxTransform):
        model = None
        return fluxGameObject(
            self.name, self.tag, transform, model, self.is_camera, self.scripts, self.children
        )

class fluxProject:
    def __init__(self,path : str):
        self.path = path
        self.prefab_counter = 0
        self.prefabs = {}

    def read_prefab_from_file(self, path):
        with open(path,"r") as f:
            prefab = json.load(f)
        name = prefab.get("prefabName","unnamed" + str(PREFAB_COUNTER))
        PREFAB_COUNTER += 1
        tag = prefab.get("prefabTag","default")
        model_path = prefab.get("prefabModel",None)
        if model_path is not None:
            if len(model_path.strip()) == 0:
                model_path = None
        is_camera = prefab.get("prefabIsCamera",False)
        scripts = prefab.get("prefabScripts",[])
        children = prefab.get("prefabChildren",[])
        return fluxPrefab(name,tag,model_path,is_camera,scripts,children)

def read_prefabs():
    pass

#read_prefab_from_file("/Users/humzaqureshi/GitHub/Flux-Engine/project/prefabs/defaultCamera.json")

exit()

class fluxScene:
    def __init__(self):
        self.gameobjects = []

    def add_gameobject(self, prefab : fluxPrefab, transform : fluxTransform = None):
        if transform is None:
            transform = defaultTransform()
        self.gameobjects.append(prefab.instantiate(transform))

    def save_scene(self,scene_name):
        out = "prefab_name,pos_x,pos_y,pos_z,rot_x,rot_y,rot_z,scale_x,scale_y,scale_z,tint_r,tint_g,tint_b,tint_a\n"
        for j in self.gameobjects:
            out += j.name + "," + str(j.transform) + "," + ",".join([str(i) for i in j.tint]) + "\n"
        with open(os.path.join(PROJECT_PATH,"scenes",scene_name),"w") as f:
            f.write(out)

sphere_prefab = fluxPrefab("sphere_prefab","default","SPHERE",False,[],[])

scene = fluxScene()
scene.add_gameobject(sphere_prefab,defaultTransform())
scene.save_scene("my_scene.csv")


'''camera = Camera3D(vector3_one(),vector3_zero(),Vector3(0,1,0),45,CameraProjection.CAMERA_PERSPECTIVE)

set_config_flags(ConfigFlags.FLAG_WINDOW_RESIZABLE)
init_window(400, 400, "editor")
while not window_should_close():
    begin_drawing()
    clear_background(BLACK)

    begin_mode_3d(camera)

    draw_grid(100,0.1)

    end_mode_3d()

    end_drawing()
close_window()'''
