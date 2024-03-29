#ifndef _FLUX_RENDERER_SHADER_MANAGER_H_
#define _FLUX_RENDERER_SHADER_MANAGER_H_

#include "raylib.h"

typedef struct renderShaderAttr {
    Shader shader;
    const char* attr;
    int loc;
    int type;
} renderShaderAttr;

void render_load_default_shader(void);

void render_unload_default_shader(void);

Shader render_get_default_shader(void);
Shader render_get_empty_shader(void);

void render_set_ka(float ka);

void render_set_cam_pos(Vector3 pos);

renderShaderAttr render_get_shader_attr(Shader shader, const char* attr);
void render_set_shader_attr_int(renderShaderAttr attr, int val);
void render_set_shader_attr_vec3(renderShaderAttr attr, Vector3 val);
void render_set_shader_attr_float(renderShaderAttr attr, float val);

void render_load_skybox(const char* path);
void render_draw_skybox(void);

void render_calculate_shadows(void);

bool render_light_is_enabled(int i);

bool render_light_is_disabled(int i);

void render_light_set_enabled(int i, int val);

void render_light_enable(int i);

void render_light_disable(int i);

int render_light_get_type(int i);

void render_light_set_type(int i, int type);

Color render_light_get_cL(int i);

void render_light_set_cL(int i, Color col);

float render_light_get_kd(int i);

void render_light_set_kd(int i, float kd);

float render_light_get_ks(int i);

void render_light_set_ks(int i, float ks);

Vector3 render_light_get_pos(int i);

void render_light_set_pos(int i, Vector3 pos);

Vector3 render_light_get_L(int i);

void render_light_set_L(int i, Vector3 L);

float render_light_get_p(int i);

void render_light_set_p(int i, float p);

float render_light_get_intensity(int i);

void render_light_set_intensity(int i, float intensity);

Camera3D render_get_light_cam(int i);

void render_light_set_scale(int i, float scale);

void render_light_set_fov(int i, float fov);

float render_light_get_scale(int i);

float render_light_get_fov(int i);

#endif