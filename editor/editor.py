from pyray import *
import pandas as pd
import numpy as np
import json
import os
import ctypes
import cffi
from flux import *

class EditorWindow:
    def __init__(self,editor_width = 1000, editor_height = 1000, super_samp = 2, background_color = (50,59,74,255)):
        self.editor_width = editor_width
        self.editor_height = editor_height
        self.super_samp = super_samp
        self.background_color = background_color
        self.editor_window = load_render_texture(self.editor_width,self.editor_height)

    def unload(self):
        unload_render_texture(self.editor_window)

    def reload_window(self):
        self.unload()
        self.editor_window = load_render_texture(self.editor_width,self.editor_height)

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
        begin_texture_mode(self.editor_window)
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
        draw_texture_pro(self.editor_window.texture,Rectangle(0,0,self.editor_width,-self.editor_height),editor_rectangle,Vector2(0,0),0,WHITE)

class Toolbar:
    def __init__(self):
        pass

    def draw(self,tool_bar_height, tool_bar_render_height):
        tool_bar_shadow = tool_bar_height - tool_bar_render_height
        draw_rectangle_gradient_v(0,tool_bar_render_height,get_screen_width(),tool_bar_shadow,BLACK,(0,0,0,0))
        draw_rectangle(0,0,get_screen_width(),tool_bar_render_height,(23,27,33,255))

set_config_flags(ConfigFlags.FLAG_WINDOW_RESIZABLE + ConfigFlags.FLAG_MSAA_4X_HINT + ConfigFlags.FLAG_WINDOW_HIGHDPI)
init_window(1200, 800, "editor")

project = fluxProject("/Users/humzaqureshi/GitHub/Flux-Engine/project")

sphere_prefab = fluxPrefab("sphere_prefab","default","SPHERE",False,[],[])

scene = fluxScene("my_scene.csv")
scene.add_gameobject(sphere_prefab,defaultTransform())

prev_editor_width = 1000
prev_editor_height = 1000
editor_view = EditorWindow()
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
#font = load_font("/Users/humzaqureshi/GitHub/Flux-Engine/editor/JetBrainsMono-Regular.ttf")
#font.baseSize = 32
#font.glyphCount = 95
#print(font.baseSize)

gui_set_font(font)
#gui_set_style(0,20,100)
while not window_should_close():

    editor_view.update()
    tool_bar_height = int(get_screen_height()/20)
    tool_bar_render_height = int(tool_bar_height*0.8)
    tool_bar_shadow = tool_bar_height - tool_bar_render_height

    # font size
    gui_set_style(0,16,int(tool_bar_height / 3))

    begin_drawing()
    clear_background((30,35,43,255))

    editor_view.draw(scene,camera,tool_bar_height)
    toolbar.draw(tool_bar_height, tool_bar_render_height)

    control_width = int(editor_view.get_panel_width()/5)
    control_shadow = tool_bar_shadow
    draw_rectangle_gradient_h(control_width,tool_bar_render_height,control_shadow,get_screen_height(),BLACK,(0,0,0,0))
    draw_rectangle(0,tool_bar_render_height,control_width,get_screen_height(),(50,55,64,255))

    icon_scale = int(tool_bar_height * 0.045)
    if (icon_scale < 1):
        icon_scale = 1

    n_buttons = 10

    buttons = ["new","save","undo","redo"]
    button_width = get_screen_width()/n_buttons

    for idx,i in enumerate(buttons):
        gui_button(Rectangle(button_width * idx,0,button_width,tool_bar_render_height),i)

    #gui_draw_icon(1,5,0,icon_scale,WHITE)
    fps_str = "fps: " + str(get_fps())
    fps_sz = measure_text_ex(font,fps_str,15,1)
    draw_text_ex(font,fps_str,Vector2(0.2,get_screen_height() - fps_sz.y * 1.2),15,1,WHITE)

    end_drawing()

model_loader.unload_models()

#unload_render_texture(editor_view)
editor_view.unload()
unload_font(font)

close_window()
