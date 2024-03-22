from pyray import *
import pandas as pd
import numpy as np
import json
import os
import ctypes
import cffi

class fluxTransform:
    def __init__(self,pos : Vector3, rot : Vector3, scale : Vector3):
        self.pos = pos
        self.rot = rot
        self.scale = scale

    def __str__(self):
        return str(self.pos.x) + "," + str(self.pos.y) + "," + str(self.pos.z) + "," + str(self.rot.x) + "," + str(self.rot.y) + "," + str(self.rot.z) + "," + str(self.scale.x) + "," + str(self.scale.y) + "," + str(self.scale.z)

def defaultTransform():
    return fluxTransform(vector3_zero(),Vector3(0,0,0),vector3_one())

class ModelLoader:
    def __init__(self):
        self.loaded_models = {}

    def load_model(self,model_path):
        if (model_path is None):
            return None
        if (model_path in self.loaded_models):
            return self.loaded_models[model_path]
        model = None
        if (model_path == "SPHERE"):
            model = load_model_from_mesh(gen_mesh_sphere(1,10,10))
        else:
            model = load_model(model_path)
        self.loaded_models[model_path] = model
        return model

    def unload_models(self):
        for i in self.loaded_models.keys():
            unload_model(self.loaded_models[i])

model_loader = ModelLoader()

class fluxGameObject:
    def __init__(self, goid : int, name : str, tag : str, transform : fluxTransform, model : Model = None, is_camera : bool = False, scripts : list[str] = [], children = []):
        self.id = goid
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

    def draw(self):
        if self.model is None:
            return
        axis = Vector3(0,0,0)
        quat = quaternion_from_euler(self.transform.rot.x,self.transform.rot.y,self.transform.rot.z)
        quaternion_to_axis_angle(quat,axis,0.0)
        angle = 2*np.arccos(quaternion_normalize(quat).w)
        draw_model_ex(self.model,self.transform.pos,axis,angle,self.transform.scale,self.tint)

class fluxPrefab:
    def __init__(self, name : str, tag : str, model_path : str, is_camera : bool, scripts : list[str], children : list[str]):
        self.name = name
        self.tag = tag
        self.model_path = model_path
        self.is_camera = is_camera
        self.scripts = scripts
        self.children = children

    def instantiate(self,transform : fluxTransform, goid : int):
        model = None
        if self.model_path is not None:
            model = model_loader.load_model(self.model_path)
        return fluxGameObject(
            goid, self.name, self.tag, transform, model, self.is_camera, self.scripts, self.children
        )

class fluxScene:
    def __init__(self, name : str):
        self.gameobjects : list[fluxGameObject] = []
        self.name = name

    def add_gameobject(self, prefab : fluxPrefab, transform : fluxTransform = None):
        if transform is None:
            transform = defaultTransform()
        self.gameobjects.append(prefab.instantiate(transform,len(self.gameobjects)))

    def draw(self):
        for i in self.gameobjects:
            i.draw()

    def save_scene(self,path):
        out = "prefab_name,pos_x,pos_y,pos_z,rot_x,rot_y,rot_z,scale_x,scale_y,scale_z,tint_r,tint_g,tint_b,tint_a\n"
        for j in self.gameobjects:
            out += j.name + "," + str(j.transform) + "," + ",".join([str(i) for i in j.tint]) + "\n"
        with open(path,"w") as f:
            f.write(out)

class fluxProject:
    def __init__(self,path : str):
        self.path = path
        self.prefab_counter = 0
        self.prefabs = {}
        self.prefabs_path = os.path.join(self.path,"prefabs")
        self.scenes_path = os.path.join(self.path,"scenes")
        self.read_all_prefabs()

        # finds all `.json` files in `prefabs_path`
    def find_prefabs(self) -> list[str]:
        return [os.path.join(self.prefabs_path,i) for i in os.listdir(self.prefabs_path) if i.split(".")[-1].strip() == "json"]

    def read_prefab_from_file(self, path):
        with open(path,"r") as f:
            prefab = json.load(f)
        name = prefab.get("prefabName","unnamed" + str(self.prefab_counter))
        self.prefab_counter += 1
        tag = prefab.get("prefabTag","default")
        model_path = prefab.get("prefabModel",None)
        if model_path is not None:
            if len(model_path.strip()) == 0:
                model_path = None
        is_camera = prefab.get("prefabIsCamera",False)
        scripts = prefab.get("prefabScripts",[])
        children = prefab.get("prefabChildren",[])
        self.prefabs[name] = fluxPrefab(name,tag,model_path,is_camera,scripts,children)

    def read_all_prefabs(self):
        for i in self.find_prefabs():
            self.read_prefab_from_file(i)

    def save_scene(self,scene : fluxScene):
        scene.save_scene(os.path.join(self.scenes_path,scene.name))