#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "transform.h"
#include "pipeline.h"
#include "shader_manager.h"

#define FLUX_MAX_LIGHTS 4
#define Vec32Array(vec) { vec.x, vec.y, vec.z }

typedef struct Light{
    int enabled;
    int type;
    float kd;
    float ks;
    float p;
    float intensity;
    Color cL;
    Vector3 pos;
    Vector3 L;

    renderShaderAttr shader_enabled;
    renderShaderAttr shader_type;
    renderShaderAttr shader_kd;
    renderShaderAttr shader_ks;
    renderShaderAttr shader_p;
    renderShaderAttr shader_intensity;
    renderShaderAttr shader_cL;
    renderShaderAttr shader_pos;
    renderShaderAttr shader_L;
} Light;

static Shader flux_default_shader;
static renderShaderAttr shader_ka;
static renderShaderAttr shader_cam_pos;
static Light lights[FLUX_MAX_LIGHTS];

renderShaderAttr render_get_shader_attr(Shader shader, const char* attr){
    renderShaderAttr out;
    out.shader = shader;
    out.attr = attr;
    out.loc = GetShaderLocation(shader,attr);
    return out;
}

void render_set_shader_attr_float(renderShaderAttr attr, float val){
    SetShaderValue(attr.shader,attr.loc,&val,SHADER_UNIFORM_FLOAT);
}

void render_set_shader_attr_int(renderShaderAttr attr, int val){
    SetShaderValue(attr.shader,attr.loc,&val,SHADER_UNIFORM_INT);
}

void render_set_shader_attr_vec3(renderShaderAttr attr, Vector3 val){
    float array[3] = Vec32Array(val);
    SetShaderValue(attr.shader,attr.loc,array,SHADER_UNIFORM_VEC3);
}

static void init_lights(void){
    for (int i = 0; i < FLUX_MAX_LIGHTS; i++){
        lights[i].shader_enabled = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].enabled", i));
        lights[i].shader_type = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].type", i));
        lights[i].shader_cL = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].cL", i));
        lights[i].shader_kd = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].kd", i));
        lights[i].shader_ks = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].ks", i));
        lights[i].shader_pos = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].pos", i));
        lights[i].shader_L = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].L", i));
        lights[i].shader_p = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].p", i));
        lights[i].shader_intensity = render_get_shader_attr(flux_default_shader, TextFormat("lights[%i].intensity", i));

        lights[i].enabled = 0;
        render_set_shader_attr_int(lights[i].shader_enabled,lights[i].enabled);
    }
}

void render_load_default_shader(void){
    flux_default_shader = LoadShader("/Users/humzaqureshi/GitHub/Flux-Engine/renderer/shaders/lights.vs","/Users/humzaqureshi/GitHub/Flux-Engine/renderer/shaders/lights.fs");
    shader_ka = render_get_shader_attr(flux_default_shader,"ka");
    shader_cam_pos = render_get_shader_attr(flux_default_shader,"camPos");
    init_lights();
}

void render_unload_default_shader(void){
    UnloadShader(flux_default_shader);
}

Shader render_get_default_shader(void){
    return flux_default_shader;
}

void render_set_ka(float ka){
    render_set_shader_attr_float(shader_ka,ka);
}

void render_set_cam_pos(Vector3 pos){
    render_set_shader_attr_vec3(shader_cam_pos,pos);
}

static Light* get_light(int i){
    assert((i < FLUX_MAX_LIGHTS) && (i >= 0));
    return &lights[i];
}

bool render_light_is_enabled(int i){
    return get_light(i)->enabled;
}

bool render_light_is_disabled(int i){
    return !render_light_is_enabled(i);
}

void render_light_set_enabled(int i, int val){
    Light* light = get_light(i);
    light->enabled = val;
    render_set_shader_attr_int(light->shader_enabled,light->enabled);
}

void render_light_enable(int i){
    render_light_set_enabled(i,1);
}

void render_light_disable(int i){
    render_light_set_enabled(i,0);
}

int render_light_get_type(int i){
    return get_light(i)->type;
}

void render_light_set_type(int i, int type){
    Light* light = get_light(i);
    light->type = type;
    render_set_shader_attr_int(light->shader_type,light->type);
}

Color render_light_get_cL(int i){
    return get_light(i)->cL;
}

void render_light_set_cL(int i, Color col){
    Light* light = get_light(i);
    light->cL = col;
    Vector3 vec_cL = (Vector3){((float)col.r) / 255.0f,((float)col.g) / 255.0f,((float)col.b) / 255.0f};
    render_set_shader_attr_vec3(light->shader_cL,vec_cL);
}

float render_light_get_kd(int i){
    return get_light(i)->kd;
}

void render_light_set_kd(int i, float kd){
    Light* light = get_light(i);
    light->kd = kd;
    render_set_shader_attr_float(light->shader_kd,light->kd);
}

float render_light_get_ks(int i){
    return get_light(i)->ks;
}

void render_light_set_ks(int i, float ks){
    Light* light = get_light(i);
    light->ks = ks;
    render_set_shader_attr_float(light->shader_ks,light->ks);
}

Vector3 render_light_get_pos(int i){
    return get_light(i)->pos;
}

void render_light_set_pos(int i, Vector3 pos){
    Light* light = get_light(i);
    light->pos = pos;
    render_set_shader_attr_vec3(light->shader_pos,light->pos);
}

Vector3 render_light_get_L(int i){
    return get_light(i)->L;
}

void render_light_set_L(int i, Vector3 L){
    Light* light = get_light(i);
    light->L = L;
    render_set_shader_attr_vec3(light->shader_L,Vector3Normalize(light->L));
}

float render_light_get_p(int i){
    return get_light(i)->p;
}

void render_light_set_p(int i, float p){
    Light* light = get_light(i);
    light->p = p;
    render_set_shader_attr_float(light->shader_p,light->p);
}

float render_light_get_intensity(int i){
    return get_light(i)->intensity;
}

void render_light_set_intensity(int i, float intensity){
    Light* light = get_light(i);
    light->intensity = intensity;
    render_set_shader_attr_float(light->shader_intensity,light->intensity);
}