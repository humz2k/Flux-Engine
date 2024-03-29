from pyray import *
import pandas as pd
import numpy as np
import json
import os
import ctypes
import cffi

class fluxTransform:
    def __init__(self, pos: Vector3, rot: Vector3, scale: Vector3):
        self.pos = pos
        self.rot = rot
        self.scale = scale

    def __str__(self):
        return ",".join(str(v) for v in (self.pos.x, self.pos.y, self.pos.z, self.rot.x, self.rot.y, self.rot.z, self.scale.x, self.scale.y, self.scale.z))

    def __repr__(self):
        return f"FluxTransform(pos={self.pos}, rot={self.rot}, scale={self.scale})"

def defaultTransform():
    return fluxTransform(vector3_zero(),Vector3(0,0,0),vector3_one())

class ModelLoader:
    def __init__(self):
        self.loaded_models = {}

    def load_model(self, model_path):
        if not model_path:
            return None
        if model_path in self.loaded_models:
            return self.loaded_models[model_path]

        if model_path == "SPHERE":
            model = load_model_from_mesh(gen_mesh_sphere(1, 10, 10))
        else:
            model = load_model(model_path)

        self.loaded_models[model_path] = model
        return model

    def unload_models(self):
        for model in self.loaded_models.values():
            unload_model(model)

model_loader = ModelLoader()

LAST_FOCUSED = None

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
        self.children : list[str] = children
        self.focused = False
        self.alive = True

    def set_focused(self):
        global LAST_FOCUSED
        if (LAST_FOCUSED is not None):
            LAST_FOCUSED.focused = False
        self.focused = True
        LAST_FOCUSED = self

    def delete(self):
        self.alive = False

    def __eq__(self,other):
        return self.id == other.id

    def draw(self):
        if not self.alive:
            return
        if self.model is None:
            return
        axis = Vector3(0,0,0)
        quat = quaternion_from_euler(self.transform.rot.x,self.transform.rot.y,self.transform.rot.z)
        quaternion_to_axis_angle(quat,axis,0.0)
        angle = 2*np.arccos(quaternion_normalize(quat).w)
        draw_model_ex(self.model,self.transform.pos,axis,angle,self.transform.scale,self.tint)
        if (self.focused):
            draw_bounding_box(get_model_bounding_box(self.model),GREEN)

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
        self.gameobjects[-1].set_focused()

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

    def refresh_prefabs(self):
        self.prefabs = {}
        self.read_all_prefabs()

        # finds all `.prefab` files in `prefabs_path`
    def find_prefabs(self) -> list[str]:
        return [os.path.join(self.prefabs_path,i) for i in os.listdir(self.prefabs_path) if i.split(".")[-1].strip() == "prefab"]

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

class EditorView:
    def __init__(self,editor_width = 1000, editor_height = 1000, super_samp = 2, background_color = (50,59,74,255)):
        self.editor_width = editor_width
        self.editor_height = editor_height
        self.super_samp = super_samp
        self.background_color = background_color
        self.window = load_render_texture(self.editor_width,self.editor_height)

    def unload(self):
        unload_render_texture(self.window)

    def reload_window(self):
        self.unload()
        self.window = load_render_texture(self.editor_width,self.editor_height)

    def update(self):
        editor_width = int(get_screen_width()/2) * self.super_samp
        editor_height = int(get_screen_height()/1.5) * self.super_samp
        if (editor_width != self.editor_width) or (editor_height != self.editor_height):
            self.editor_height = editor_height
            self.editor_width = editor_width
            self.reload_window()

    def get_panel_width(self):
        return self.editor_width//(2*self.super_samp)

    def draw(self,scene,camera, tool_bar_height):
        #tool_bar_height = int(get_screen_height()/20)
        begin_texture_mode(self.window)
        clear_background(BLACK)
        begin_mode_3d(camera)
        draw_grid(100,1)
        scene.draw()
        end_mode_3d()
        end_texture_mode()
        editor_rectangle = Rectangle(self.editor_width//(2*self.super_samp),tool_bar_height,self.editor_width//self.super_samp,self.editor_height//self.super_samp)
        border_size = get_screen_width()/100
        border_rectangle = Rectangle(editor_rectangle.x - border_size, editor_rectangle.y - border_size, editor_rectangle.width + 2*border_size, editor_rectangle.height + 2 * border_size)
        draw_rectangle_rec(border_rectangle,self.background_color)
        draw_texture_pro(self.window.texture,Rectangle(0,0,self.editor_width,-self.editor_height),editor_rectangle,Vector2(0,0),0,WHITE)
        return border_rectangle

class Toolbar:
    def __init__(self):
        pass

    def draw(self,tool_bar_height, tool_bar_render_height):
        tool_bar_shadow = tool_bar_height - tool_bar_render_height
        draw_rectangle_gradient_v(0,tool_bar_render_height,get_screen_width(),tool_bar_shadow,BLACK,(0,0,0,0))
        draw_rectangle(0,0,get_screen_width(),tool_bar_render_height,(23,27,33,255))

        n_buttons = 10

        buttons = ["new","save","undo","redo"]
        button_width = get_screen_width()/n_buttons

        for idx,i in enumerate(buttons):
            if gui_button(Rectangle(button_width * idx,0,button_width,tool_bar_render_height),i):
                if (i == "save"):
                    project.save_scene(scene)

class FileViewer:
    def __init__(self, init_path : str):
        self.cd = init_path
        self.refresh()
        self.last_was_cd = False

    def refresh(self):
        self.file_names = ["",".."] + os.listdir(self.cd)
        self.file_types = []
        for i in range(len(self.file_names)):
            if self.file_names[i] == "":
                self.file_types.append("refresh")
            elif os.path.isdir(os.path.join(self.cd,self.file_names[i])):
                self.file_types.append("folder")
            elif self.file_names[i].split(".")[-1].strip() == "prefab":
                self.file_types.append("prefab")
                self.file_names[i] = ".".join(self.file_names[i].split(".")[:-1])
            else:
                self.file_types.append("file")

    def draw(self, editor_rect : Rectangle):
        start_x = int((editor_rect.x) * 1.05)
        end_x = int((editor_rect.x + editor_rect.width) * 0.95)

        line_height = measure_text_ex(font,"abcdefghijklmnopqrstuvwxyz/_-",10,1).y
        y = int((editor_rect.y + editor_rect.height) * 1.02)
        x = int(start_x)

        cd_to = None

        count = 0
        while count < len(self.file_names):
            #for i in range(icons_per_line):
                #x = int(start_x + diff_x * i)
            text_width = int(measure_text_ex(font,self.file_names[count],10,1).x)
            if (text_width < 32):
                text_width = 32

            if (x + text_width > end_x):
                x = int(start_x)
                y += line_height + 32
                y = int(y)

            icon = GuiIconName.ICON_FOLDER
            if (self.file_types[count] == "file"):
                icon = GuiIconName.ICON_FILE
            elif (self.file_types[count] == "prefab"):
                icon = GuiIconName.ICON_EMPTYBOX
            elif (self.file_types[count] == "refresh"):
                icon = GuiIconName.ICON_ROTATE

            if gui_button(Rectangle(x,y,text_width,int(32 + line_height)),""):
                if self.file_types[count] in ["folder","refresh"]:
                    cd_to = os.path.join(os.path.join(self.cd,self.file_names[count]))
                elif self.file_types[count] == "prefab":
                    scene.add_gameobject(project.prefabs[self.file_names[count]],defaultTransform())

            gui_draw_icon(icon, x, y, 2, WHITE)
            draw_text_ex(font,self.file_names[count],Vector2(x,y+32),10,1,WHITE)
            count += 1
            x += int(text_width * 1.2)

        if (cd_to is not None):
            self.cd = cd_to
            self.refresh()
            project.refresh_prefabs()

def draw_hierarchy(gameobjects : list[fluxGameObject],x,y):
    for i in gameobjects:
        if (not i.alive):
            continue
        text_size = measure_text_ex(font,i.name,15,1)
        next_y = y + (text_size.y) * 1.05
        if gui_button(Rectangle(x,y,text_size.x,text_size.y),""):
            i.set_focused()
        draw_text_ex(font,i.name,Vector2(x,y),15,1,WHITE)
        y = next_y
        y = draw_hierarchy(i.children,x * 1.2,y)
    return y

def draw_line(text,x,y,size=15,color=WHITE):
    delta_y = (measure_text_ex(font,text,size,1).y) * 1.2
    draw_text_ex(font,text,Vector2(x,y),size,1,color)
    return y + delta_y

def draw_stats_last_focused(x,y):
    global LAST_FOCUSED
    if (LAST_FOCUSED is None):
        return

    y = draw_line("name: " + LAST_FOCUSED.name,x,y)
    y = draw_line("tag: " + LAST_FOCUSED.tag,x,y)
    y = draw_line("transform: ",x,y)
    t = LAST_FOCUSED.transform
    y = draw_line(f" - pos = [{t.pos.x},{t.pos.y},{t.pos.z}]",x,y)
    y = draw_line(f" - rot = [{t.rot.x},{t.rot.y},{t.rot.z}]",x,y)
    y = draw_line(f" - scale = [{t.scale.x},{t.scale.y},{t.scale.z}]",x,y)

set_config_flags(ConfigFlags.FLAG_WINDOW_RESIZABLE + ConfigFlags.FLAG_MSAA_4X_HINT + ConfigFlags.FLAG_WINDOW_HIGHDPI)
init_window(1200, 800, "editor")

project = fluxProject("/Users/humzaqureshi/GitHub/Flux-Engine/project")
fileviewer = FileViewer(project.path)

#sphere_prefab = fluxPrefab("sphere_prefab","default","SPHERE",False,[],[])

scene = fluxScene("my_scene.csv")
#scene.add_gameobject(sphere_prefab,defaultTransform())

prev_editor_width = 1000
prev_editor_height = 1000
editor_view = EditorView()
toolbar = Toolbar()

camera = Camera3D(Vector3(3,3,3),vector3_zero(),Vector3(0,1,0),45,CameraProjection.CAMERA_PERSPECTIVE)
set_target_fps(60)

checked = True

gui_set_style(0,0,0)
gui_set_style(0,1,0)
gui_set_style(0,3,0)
gui_set_style(0,4,50)
gui_set_style(0,6,0)
gui_set_style(0,7,100)
font = load_font_ex("/Users/humzaqureshi/GitHub/Flux-Engine/editor/JetBrainsMono-Regular.ttf",128,None,0)

gui_set_font(font)
cursor_disabled = False

while not window_should_close():

    editor_view.update()
    tool_bar_height = int(get_screen_height()/20)
    tool_bar_render_height = int(tool_bar_height*0.8)
    tool_bar_shadow = tool_bar_height - tool_bar_render_height

    # font size
    gui_set_style(0,16,int(tool_bar_height / 3))

    begin_drawing()
    clear_background((30,35,43,255))

    editor_rect = editor_view.draw(scene,camera,tool_bar_height)

    if (is_key_down(KeyboardKey.KEY_C)):

        if not cursor_disabled:
            mouse_pos = get_mouse_position()
            if (mouse_pos.x > editor_rect.x) and (mouse_pos.x - editor_rect.x < editor_rect.width):
                if (mouse_pos.y > editor_rect.y) and (mouse_pos.y - editor_rect.y < editor_rect.height):
                    cursor_disabled = True
                    disable_cursor()
        if (cursor_disabled):
            update_camera(camera,CameraMode.CAMERA_THIRD_PERSON)
    elif cursor_disabled:
        cursor_disabled = False
        show_cursor()

    if (is_key_pressed(KeyboardKey.KEY_R)):
        camera = Camera3D(Vector3(3,3,3),vector3_zero(),Vector3(0,1,0),45,CameraProjection.CAMERA_PERSPECTIVE)

    toolbar.draw(tool_bar_height, tool_bar_render_height)

    control_width = int(editor_view.get_panel_width()/5)

    draw_hierarchy(scene.gameobjects,control_width * 1.1,tool_bar_height)

    control_shadow = tool_bar_shadow
    draw_rectangle_gradient_h(control_width,tool_bar_render_height,control_shadow,get_screen_height(),BLACK,(0,0,0,0))
    draw_rectangle(0,tool_bar_render_height,control_width,get_screen_height(),(50,55,64,255))

    #gui_draw_icon(1,5,0,icon_scale,WHITE)
    fps_str = "fps: " + str(get_fps())
    fps_sz = measure_text_ex(font,fps_str,15,1)
    draw_text_ex(font,fps_str,Vector2(0.2,get_screen_height() - fps_sz.y * 1.2),15,1,WHITE)

    fileviewer.draw(editor_rect)
    draw_stats_last_focused((editor_rect.x + editor_rect.width) * 1.01, (editor_rect.y) * 1.5)

    #print(editor_rect)

    end_drawing()

model_loader.unload_models()

#unload_render_texture(editor_view)
editor_view.unload()
unload_font(font)

close_window()
