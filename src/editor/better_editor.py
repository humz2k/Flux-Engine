from pyray import *
import pandas as pd
import numpy as np
import json
import os

class Editor:
    def __init__(self, project_path = "/Users/humzaqureshi/GitHub/Flux-Engine/project",
                    width = 1200, height = 800,
                    window_name = "fluxEditor",
                    target_fps = 60,
                    font = "/Users/humzaqureshi/GitHub/Flux-Engine/editor/JetBrainsMono-Regular.ttf",
                    fontsize = 128,
                    background_color = (30,35,43,255),
                    tool_bar_size = 40,
                    top_tool_bar_color = (23,27,33,255),
                    side_tool_bar_color = (50,55,64,255),
                    shadow_off = 0.8):
        set_config_flags(ConfigFlags.FLAG_WINDOW_RESIZABLE + ConfigFlags.FLAG_MSAA_4X_HINT + ConfigFlags.FLAG_WINDOW_HIGHDPI)
        init_window(width,height,window_name)
        set_target_fps(target_fps)

        gui_set_style(0,0,0)
        gui_set_style(0,1,0)
        gui_set_style(0,3,0)
        gui_set_style(0,4,50)
        gui_set_style(0,6,0)
        gui_set_style(0,7,100)

        self.font = load_font_ex(font,fontsize,None,0)
        gui_set_font(self.font)

        self.background_color = background_color

        self.tool_bar_size = tool_bar_size
        self.top_tool_bar_color = top_tool_bar_color
        self.side_tool_bar_color = side_tool_bar_color

        self.shadow_off = shadow_off

        self.update_screen_dims()

    def update_screen_dims(self):
        self.screen_height = get_screen_height()
        self.screen_width = get_screen_width()

    def cleanup(self):
        pass

    def get_top_tool_bar_rect(self):
        return Rectangle(0,0,self.screen_width,self.tool_bar_size)

    def draw_top_tool_bar_background(self):
        rect = self.get_top_tool_bar_rect()
        render_height = rect.height * self.shadow_off
        render_rect = Rectangle(rect.x,rect.y,rect.width,render_height)
        shadow_rect = Rectangle(rect.x,render_height,rect.width,rect.height - render_height)
        draw_rectangle_gradient_v(int(shadow_rect.x),int(shadow_rect.y),int(shadow_rect.width),int(shadow_rect.height),BLACK,(0,0,0,0))
        draw_rectangle_rec(render_rect,self.top_tool_bar_color)

    def get_side_tool_bar_rect(self):
        return Rectangle(0,self.tool_bar_size * self.shadow_off,self.tool_bar_size,self.screen_height)

    def draw_size_tool_bar_background(self):
        rect = self.get_side_tool_bar_rect()
        render_rect = Rectangle(rect.x,rect.y,rect.width * self.shadow_off,rect.height)
        shadow_rect = Rectangle(rect.width * self.shadow_off,rect.y,rect.width - rect.width * self.shadow_off,rect.height)
        draw_rectangle_gradient_h(int(shadow_rect.x),int(shadow_rect.y),int(shadow_rect.width),int(shadow_rect.height),BLACK,(0,0,0,0))
        draw_rectangle_rec(render_rect,self.side_tool_bar_color)

    def run(self):
        while not window_should_close():

            self.update_screen_dims()

            begin_drawing()

            clear_background(self.background_color)

            self.draw_top_tool_bar_background()
            self.draw_size_tool_bar_background()

            end_drawing()

        close_window()

        self.cleanup()

Editor().run()