#include "shader_manager.h"
#include "hqtools/hqtools.h"
#include "pipeline.h"
#include "raylib.h"
#include "raymath.h"
#include "shadow_map_texture.h"
#include "transform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define FLUX_MAX_LIGHTS 4
#define Vec32Array(vec)                                                        \
    { vec.x, vec.y, vec.z }

typedef struct Light {
    int enabled;
    int type;
    float kd;
    float ks;
    float p;
    float intensity;
    Color cL;
    Vector3 pos;
    Vector3 L;
    RenderTexture2D shadow_map;
    Matrix light_vp;

    float scale;
    float fov;

    renderShaderAttr shader_enabled;
    renderShaderAttr shader_type;
    renderShaderAttr shader_kd;
    renderShaderAttr shader_ks;
    renderShaderAttr shader_p;
    renderShaderAttr shader_intensity;
    renderShaderAttr shader_cL;
    renderShaderAttr shader_pos;
    renderShaderAttr shader_L;
    renderShaderAttr shader_light_vp;

    int shader_shadow_map_loc;
} Light;

static Shader flux_default_shader;
static Shader flux_empty_shader;
static renderShaderAttr shader_ka;
static renderShaderAttr shader_cam_pos;
static renderShaderAttr shader_shadow_map_res;
static Light lights[FLUX_MAX_LIGHTS];

static int skybox_loaded = 0;
static Shader skybox_shader;
static Model skybox;

static int shadowMapRes = 4096;

renderShaderAttr render_get_shader_attr(Shader shader, const char* attr) {
    renderShaderAttr out;
    out.shader = shader;
    out.attr = attr;
    out.loc = GetShaderLocation(shader, attr);
    return out;
}

void render_set_shader_attr_float(renderShaderAttr attr, float val) {
    SetShaderValue(attr.shader, attr.loc, &val, SHADER_UNIFORM_FLOAT);
}

void render_set_shader_attr_int(renderShaderAttr attr, int val) {
    SetShaderValue(attr.shader, attr.loc, &val, SHADER_UNIFORM_INT);
}

void render_set_shader_attr_vec3(renderShaderAttr attr, Vector3 val) {
    float array[3] = Vec32Array(val);
    SetShaderValue(attr.shader, attr.loc, array, SHADER_UNIFORM_VEC3);
}

static void init_lights(void) {
    TraceLog(LOG_INFO, "init_lights");
    TraceLog(LOG_INFO, "shadowMapRes %dx%d", shadowMapRes, shadowMapRes);
    for (int i = 0; i < FLUX_MAX_LIGHTS; i++) {
        lights[i].shader_enabled = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].enabled", i));
        lights[i].shader_type = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].type", i));
        lights[i].shader_cL = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].cL", i));
        lights[i].shader_kd = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].kd", i));
        lights[i].shader_ks = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].ks", i));
        lights[i].shader_pos = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].pos", i));
        lights[i].shader_L = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].L", i));
        lights[i].shader_p = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].p", i));
        lights[i].shader_intensity = render_get_shader_attr(
            flux_default_shader, TextFormat("lights[%i].intensity", i));
        lights[i].shader_light_vp = render_get_shader_attr(
            flux_default_shader, TextFormat("light_vp[%i]", i));

        lights[i].shader_shadow_map_loc =
            render_get_shader_attr(flux_default_shader,
                                   TextFormat("lights[%i].shadowMap", i))
                .loc;

        lights[i].shadow_map =
            LoadShadowmapRenderTexture(shadowMapRes, shadowMapRes);

        lights[i].scale = 15.0f;
        lights[i].fov = 20.0f;

        lights[i].enabled = 0;
        render_set_shader_attr_int(lights[i].shader_enabled, lights[i].enabled);
    }
}

static void delete_lights(void) {
    TraceLog(LOG_INFO, "delete_lights");
    for (int i = 0; i < FLUX_MAX_LIGHTS; i++) {
        UnloadShadowmapRenderTexture(lights[i].shadow_map);
    }
}

void render_load_default_shader(void) {
    TraceLog(LOG_INFO, "render_load_default_shader");
    skybox_loaded = 0;
    flux_default_shader = LoadShader("src/renderer/shaders/lights.vs",
                                     "src/renderer/shaders/lights.fs");
    shader_ka = render_get_shader_attr(flux_default_shader, "ka");
    shader_cam_pos = render_get_shader_attr(flux_default_shader, "camPos");
    shader_shadow_map_res =
        render_get_shader_attr(flux_default_shader, "shadowMapRes");
    render_set_shader_attr_int(shader_shadow_map_res, shadowMapRes);

    flux_empty_shader = LoadShader("src/renderer/shaders/lights.vs",
                                   "src/renderer/shaders/empty_lights.fs");
    init_lights();
}

void render_load_skybox(const char* path) {
    TraceLog(LOG_INFO, "render_load_skybox");
    skybox_loaded = 1;
    skybox_shader = LoadShader("src/renderer/shaders/skybox.vs",
                               "src/renderer/shaders/skybox.fs");
    skybox = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    skybox.materials[0].shader = skybox_shader;
    SetShaderValue(
        skybox.materials[0].shader,
        GetShaderLocation(skybox.materials[0].shader, "environmentMap"),
        (int[1]){MATERIAL_MAP_CUBEMAP}, SHADER_UNIFORM_INT);

    Image img = LoadImage(path);

    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(
        img, CUBEMAP_LAYOUT_AUTO_DETECT); // CUBEMAP_LAYOUT_PANORAMA
    UnloadImage(img);
}

void render_unload_skybox(void) {
    if (!skybox_loaded)
        return;
    TraceLog(LOG_INFO, "render_unload_skybox");
    UnloadModel(skybox);
}

void render_draw_skybox(void) {
    if (!skybox_loaded)
        return;
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skybox, render_get_current_cam().position, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
}

void render_unload_default_shader(void) {
    TraceLog(LOG_INFO, "render_unload_default_shader");
    delete_lights();
    UnloadShader(flux_default_shader);
    UnloadShader(flux_empty_shader);
    render_unload_skybox();
}

Shader render_get_default_shader(void) { return flux_default_shader; }

Shader render_get_empty_shader(void) { return flux_empty_shader; }

Camera3D render_get_light_cam(int i) {
    Camera3D lightCam = (Camera3D){0};
    lightCam.position = Vector3Scale(lights[i].L, lights[i].scale);
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    lightCam.target = Vector3Zero();
    lightCam.fovy = lights[i].fov;
    lightCam.up = (Vector3){0.0f, 1.0f, 0.0f};
    return lightCam;
}

void render_calculate_shadows(void) {
    int slot_start = 15 - FLUX_MAX_LIGHTS;
    for (int i = 0; i < FLUX_MAX_LIGHTS; i++) {
        if (!lights[i].enabled)
            continue;
        Camera3D lightCam = render_get_light_cam(i);

        Matrix lightView;
        Matrix lightProj;
        BeginTextureMode(lights[i].shadow_map);
        ClearBackground(WHITE);
        BeginMode3D(lightCam);
        lightView = rlGetMatrixModelview();
        lightProj = rlGetMatrixProjection();
        render_draw_all_no_shader(lightCam);
        EndMode3D();
        EndTextureMode();
        Matrix lightViewProj = MatrixMultiply(lightView, lightProj);
        SetShaderValueMatrix(flux_default_shader, lights[i].shader_light_vp.loc,
                             lightViewProj);

        rlEnableShader(flux_default_shader.id);
        int slot =
            slot_start +
            i; // Can be anything 0 to 15, but 0 will probably be taken up
        rlActiveTextureSlot(slot);
        rlEnableTexture(lights[i].shadow_map.depth.id);
        rlSetUniform(lights[i].shader_shadow_map_loc, &slot, SHADER_UNIFORM_INT,
                     1);
    }
}

void render_set_ka(float ka) { render_set_shader_attr_float(shader_ka, ka); }

void render_set_cam_pos(Vector3 pos) {
    render_set_shader_attr_vec3(shader_cam_pos, pos);
}

static Light* get_light(int i) {
    assert((i < FLUX_MAX_LIGHTS) && (i >= 0));
    return &lights[i];
}

bool render_light_is_enabled(int i) { return get_light(i)->enabled; }

bool render_light_is_disabled(int i) { return !render_light_is_enabled(i); }

void render_light_set_enabled(int i, int val) {
    Light* light = get_light(i);
    light->enabled = val;
    render_set_shader_attr_int(light->shader_enabled, light->enabled);
}

void render_light_enable(int i) { render_light_set_enabled(i, 1); }

void render_light_disable(int i) { render_light_set_enabled(i, 0); }

int render_light_get_type(int i) { return get_light(i)->type; }

void render_light_set_type(int i, int type) {
    Light* light = get_light(i);
    light->type = type;
    render_set_shader_attr_int(light->shader_type, light->type);
}

Color render_light_get_cL(int i) { return get_light(i)->cL; }

void render_light_set_cL(int i, Color col) {
    Light* light = get_light(i);
    light->cL = col;
    Vector3 vec_cL = (Vector3){((float)col.r) / 255.0f, ((float)col.g) / 255.0f,
                               ((float)col.b) / 255.0f};
    render_set_shader_attr_vec3(light->shader_cL, vec_cL);
}

float render_light_get_kd(int i) { return get_light(i)->kd; }

void render_light_set_kd(int i, float kd) {
    Light* light = get_light(i);
    light->kd = kd;
    render_set_shader_attr_float(light->shader_kd, light->kd);
}

float render_light_get_ks(int i) { return get_light(i)->ks; }

void render_light_set_ks(int i, float ks) {
    Light* light = get_light(i);
    light->ks = ks;
    render_set_shader_attr_float(light->shader_ks, light->ks);
}

Vector3 render_light_get_pos(int i) { return get_light(i)->pos; }

void render_light_set_pos(int i, Vector3 pos) {
    Light* light = get_light(i);
    light->pos = pos;
    render_set_shader_attr_vec3(light->shader_pos, light->pos);
}

Vector3 render_light_get_L(int i) { return get_light(i)->L; }

void render_light_set_L(int i, Vector3 L) {
    Light* light = get_light(i);
    light->L = L;
    render_set_shader_attr_vec3(light->shader_L, Vector3Normalize(light->L));
}

float render_light_get_p(int i) { return get_light(i)->p; }

void render_light_set_p(int i, float p) {
    Light* light = get_light(i);
    light->p = p;
    render_set_shader_attr_float(light->shader_p, light->p);
}

float render_light_get_intensity(int i) { return get_light(i)->intensity; }

void render_light_set_intensity(int i, float intensity) {
    Light* light = get_light(i);
    light->intensity = intensity;
    render_set_shader_attr_float(light->shader_intensity, light->intensity);
}

void render_light_set_scale(int i, float scale) {
    Light* light = get_light(i);
    light->scale = scale;
}

void render_light_set_fov(int i, float fov) {
    Light* light = get_light(i);
    light->fov = fov;
}

float render_light_get_scale(int i) { return get_light(i)->scale; }

float render_light_get_fov(int i) { return get_light(i)->fov; }