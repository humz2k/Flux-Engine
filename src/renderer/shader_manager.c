/**
 * @file shader_manager.c
 * @brief This file contains all the shader management functions including
 * initialization, loading, setting, and cleaning up shaders for the rendering
 * system using Raylib.
 */

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

/** Maximum number of lights supported. */
#define FLUX_MAX_LIGHTS 4

/**
 * @brief Converts a Vector3 to an array format, typically for OpenGL
 * interoperation.
 * @param vec Vector3 structure to be converted.
 */
#define Vec32Array(vec)                                                        \
    { vec.x, vec.y, vec.z }

/**
 * @struct Light
 * @brief Structure representing a light source in the scene.
 *
 * This structure holds all necessary information about light sources including
 * type, position, color, and related shader attributes.
 */
typedef struct Light {
    int enabled;     /**< Flag to indicate if the light is enabled. */
    int type;        /**< Type of the light. */
    float kd;        /**< Diffuse reflectivity. */
    float ks;        /**< Specular reflectivity. */
    float p;         /**< Shininess factor for specular highlights. */
    float intensity; /**< Light intensity. */
    Color cL;        /**< Color of the light. */
    Vector3 pos;     /**< Position of the light in 3D space. */
    Vector3 L;       /**< Direction of the light. */
    RenderTexture2D shadow_map; /**< Texture for shadow mapping. */
    Matrix light_vp;            /**< View-projection matrix for the light. */

    float scale; /**< Scale factor for the light's influence. */
    float fov;   /**< Field of view for the light. */

    renderShaderAttr
        shader_enabled; /**< Shader attribute for light enable state. */
    renderShaderAttr shader_type; /**< Shader attribute for light type. */
    renderShaderAttr
        shader_kd; /**< Shader attribute for diffuse reflectivity. */
    renderShaderAttr
        shader_ks; /**< Shader attribute for specular reflectivity. */
    renderShaderAttr shader_p; /**< Shader attribute for shininess factor. */
    renderShaderAttr
        shader_intensity;        /**< Shader attribute for light intensity. */
    renderShaderAttr shader_cL;  /**< Shader attribute for light color. */
    renderShaderAttr shader_pos; /**< Shader attribute for light position. */
    renderShaderAttr shader_L;   /**< Shader attribute for light direction. */
    renderShaderAttr shader_light_vp; /**< Shader attribute for light
                                         view-projection matrix. */

    int shader_shadow_map_loc; /**< Location index for the shadow map in the
                                  shader. */
} Light;

static Shader flux_default_shader; /**< Default shader used for rendering. */
static Shader flux_empty_shader; /**< Shader used when no lights are active. */
static renderShaderAttr
    shader_ka; /**< Ambient light coefficient shader attribute. */
static renderShaderAttr
    shader_cam_pos; /**< Camera position shader attribute. */
static renderShaderAttr
    shader_shadow_map_res; /**< Shadow map resolution shader attribute. */
static Light lights[FLUX_MAX_LIGHTS]; /**< Array of light structures. */

static int skybox_loaded = 0; /**< Flag to check if the skybox is loaded. */
static Shader skybox_shader;  /**< Shader for rendering the skybox. */
static Model skybox;          /**< 3D model for the skybox. */

static bool skybox_enabled = true;

static int shadowMapRes = 4096; /**< Resolution of the shadow map. */

/**
 * @brief Retrieves a shader attribute location for a specified attribute name.
 * @param shader Shader to query.
 * @param attr Attribute name as a string.
 * @return A renderShaderAttr structure with the location of the shader
 * attribute.
 */
renderShaderAttr render_get_shader_attr(Shader shader, const char* attr) {
    LOG_FUNC_CALL();
    renderShaderAttr out;
    out.shader = shader;
    out.attr = attr;
    out.loc = GetShaderLocation(shader, attr);
    return out;
}

/**
 * @brief Sets a shader attribute's value for a float.
 * @param attr Shader attribute to modify.
 * @param val New float value to set.
 */
void render_set_shader_attr_float(renderShaderAttr attr, float val) {
    LOG_FUNC_CALL();
    SetShaderValue(attr.shader, attr.loc, &val, SHADER_UNIFORM_FLOAT);
}

/**
 * @brief Sets a shader attribute's value for an integer.
 * @param attr Shader attribute to modify.
 * @param val New integer value to set.
 */
void render_set_shader_attr_int(renderShaderAttr attr, int val) {
    LOG_FUNC_CALL();
    SetShaderValue(attr.shader, attr.loc, &val, SHADER_UNIFORM_INT);
}

/**
 * @brief Sets a shader attribute's value for a vector3.
 * @param attr Shader attribute to modify.
 * @param val New Vector3 value to set.
 */
void render_set_shader_attr_vec3(renderShaderAttr attr, Vector3 val) {
    LOG_FUNC_CALL();
    float array[3] = Vec32Array(val);
    SetShaderValue(attr.shader, attr.loc, array, SHADER_UNIFORM_VEC3);
}

/**
 * @brief Initializes all light structures and their shader attributes.
 * Initializes shadow maps for lights and sets initial values for lights
 * properties.
 */
static void init_lights(void) {
    LOG_FUNC_CALL();
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

/**
 * @brief Deletes all light structures and unloads their associated resources.
 * This function is typically called when cleaning up the renderer.
 */
static void delete_lights(void) {
    LOG_FUNC_CALL();
    TraceLog(LOG_INFO, "delete_lights");
    for (int i = 0; i < FLUX_MAX_LIGHTS; i++) {
        UnloadShadowmapRenderTexture(lights[i].shadow_map);
    }
}

/**
 * @brief Loads the default shader and initializes light properties.
 * This function is responsible for setting up the shader used for most of the
 * rendering tasks.
 */
void render_load_default_shader(void) {
    LOG_FUNC_CALL();
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

/**
 * @brief Loads the skybox with a specified texture path.
 * @param path Path to the skybox texture image.
 */
void render_load_skybox(const char* path) {
    LOG_FUNC_CALL();
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

/**
 * @brief Unloads the skybox and frees associated resources.
 */
void render_unload_skybox(void) {
    LOG_FUNC_CALL();
    if (!skybox_loaded)
        return;
    TraceLog(LOG_INFO, "render_unload_skybox");
    UnloadModel(skybox);
}

/**
 * @brief Renders the skybox if it is loaded.
 * Handles enabling and disabling of depth mask and backface culling for correct
 * skybox rendering.
 */
void render_draw_skybox(void) {
    LOG_FUNC_CALL();
    if (!skybox_loaded)
        return;
    if (!skybox_enabled)
        return;
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skybox, render_get_current_cam().position, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
}

void render_enable_skybox(void){
    skybox_enabled = true;
}

void render_disable_skybox(void){
    skybox_enabled = false;
}

/**
 * @brief Unloads the default shader and associated resources.
 * This function also unloads the skybox and all lights.
 */
void render_unload_default_shader(void) {
    LOG_FUNC_CALL();
    TraceLog(LOG_INFO, "render_unload_default_shader");
    delete_lights();
    UnloadShader(flux_default_shader);
    UnloadShader(flux_empty_shader);
    render_unload_skybox();
}

/**
 * @brief Returns the default shader used in the renderer.
 * @return The default shader.
 */
Shader render_get_default_shader(void) { LOG_FUNC_CALL(); return flux_default_shader; }

/**
 * @brief Returns the shader used when no lights are active.
 * @return The empty shader.
 */
Shader render_get_empty_shader(void) { LOG_FUNC_CALL(); return flux_empty_shader; }

/**
 * @brief Calculates and returns a camera configuration for a light based on its
 * index. This camera is used for shadow mapping.
 * @param i Index of the light.
 * @return Configured Camera3D structure.
 */
Camera3D render_get_light_cam(int i) {
    LOG_FUNC_CALL();
    Camera3D lightCam = (Camera3D){0};
    lightCam.position = Vector3Scale(lights[i].L, lights[i].scale);
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    lightCam.target = Vector3Zero();
    lightCam.fovy = lights[i].fov;
    lightCam.up = (Vector3){0.0f, 1.0f, 0.0f};
    return lightCam;
}

/**
 * @brief Calculates shadows for all enabled lights.
 * This function updates the shadow maps for each light if it is enabled.
 */
void render_calculate_shadows(void) {
    LOG_FUNC_CALL();
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

/**
 * @brief Sets the ambient light coefficient in the shader.
 * @param ka Ambient coefficient to set.
 */
void render_set_ka(float ka) { LOG_FUNC_CALL(); render_set_shader_attr_float(shader_ka, ka); }

/**
 * @brief Sets the camera position in the shader.
 * @param pos New camera position.
 */
void render_set_cam_pos(Vector3 pos) {
    LOG_FUNC_CALL();
    render_set_shader_attr_vec3(shader_cam_pos, pos);
}

/**
 * @brief Retrieves a pointer to a light structure based on index.
 * Asserts that the index is within valid range.
 * @param i Index of the light to retrieve.
 * @return Pointer to the Light structure.
 */
static Light* get_light(int i) {
    LOG_FUNC_CALL();
    assert((i < FLUX_MAX_LIGHTS) && (i >= 0));
    return &lights[i];
}

/**
 * @brief Checks if a light at a given index is enabled.
 * @param i Index of the light to check.
 * @return True if the light is enabled, false otherwise.
 */
bool render_light_is_enabled(int i) { LOG_FUNC_CALL(); return get_light(i)->enabled; }

/**
 * @brief Checks if a light at a given index is disabled.
 * @param i Index of the light to check.
 * @return True if the light is disabled, false otherwise.
 */
bool render_light_is_disabled(int i) { LOG_FUNC_CALL(); return !render_light_is_enabled(i); }

/**
 * @brief Enables or disables a light based on the given value.
 * @param i Index of the light to modify.
 * @param val Non-zero to enable the light, zero to disable.
 */
void render_light_set_enabled(int i, int val) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->enabled = val;
    render_set_shader_attr_int(light->shader_enabled, light->enabled);
}

/**
 * @brief Enables a light at the specified index.
 * @param i Index of the light to enable.
 */
void render_light_enable(int i) { LOG_FUNC_CALL(); render_light_set_enabled(i, 1); }

/**
 * @brief Disables a light at the specified index.
 * @param i Index of the light to disable.
 */
void render_light_disable(int i) { LOG_FUNC_CALL(); render_light_set_enabled(i, 0); }

/**
 * @brief Retrieves the type of a light based on its index.
 * @param i Index of the light whose type is to be retrieved.
 * @return The light type as an integer.
 */
int render_light_get_type(int i) { LOG_FUNC_CALL(); return get_light(i)->type; }

/**
 * @brief Sets the type of a light based on the given index and type.
 * @param i Index of the light to modify.
 * @param type The new type of the light to set.
 */
void render_light_set_type(int i, int type) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->type = type;
    render_set_shader_attr_int(light->shader_type, light->type);
}

/**
 * @brief Retrieves the color of a light based on its index.
 * @param i Index of the light whose color is to be retrieved.
 * @return The color of the light.
 */
Color render_light_get_cL(int i) { LOG_FUNC_CALL(); return get_light(i)->cL; }

/**
 * @brief Sets the color of a light based on the given index and color.
 * @param i Index of the light to modify.
 * @param col The new color of the light to set.
 */
void render_light_set_cL(int i, Color col) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->cL = col;
    Vector3 vec_cL = (Vector3){((float)col.r) / 255.0f, ((float)col.g) / 255.0f,
                               ((float)col.b) / 255.0f};
    render_set_shader_attr_vec3(light->shader_cL, vec_cL);
}

/**
 * @brief Retrieves the diffuse reflectivity coefficient of a light based on its
 * index.
 * @param i Index of the light whose diffuse reflectivity is to be retrieved.
 * @return The diffuse reflectivity coefficient as a float.
 */
float render_light_get_kd(int i) { LOG_FUNC_CALL(); return get_light(i)->kd; }

/**
 * @brief Sets the diffuse reflectivity coefficient of a light based on the
 * given index and coefficient.
 * @param i Index of the light to modify.
 * @param kd The new diffuse reflectivity coefficient to set.
 */
void render_light_set_kd(int i, float kd) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->kd = kd;
    render_set_shader_attr_float(light->shader_kd, light->kd);
}

/**
 * @brief Retrieves the specular reflectivity coefficient of a light based on
 * its index.
 * @param i Index of the light whose specular reflectivity is to be retrieved.
 * @return The specular reflectivity coefficient as a float.
 */
float render_light_get_ks(int i) { LOG_FUNC_CALL(); return get_light(i)->ks; }

/**
 * @brief Sets the specular reflectivity coefficient of a light based on the
 * given index and coefficient.
 * @param i Index of the light to modify.
 * @param ks The new specular reflectivity coefficient to set.
 */
void render_light_set_ks(int i, float ks) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->ks = ks;
    render_set_shader_attr_float(light->shader_ks, light->ks);
}

/**
 * @brief Retrieves the position of a light based on its index.
 * @param i Index of the light whose position is to be retrieved.
 * @return The position of the light as a Vector3.
 */
Vector3 render_light_get_pos(int i) { LOG_FUNC_CALL(); return get_light(i)->pos; }

/**
 * @brief Sets the position of a light based on the given index and position.
 * @param i Index of the light to modify.
 * @param pos The new position of the light to set.
 */
void render_light_set_pos(int i, Vector3 pos) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->pos = pos;
    render_set_shader_attr_vec3(light->shader_pos, light->pos);
}

/**
 * @brief Retrieves the direction of a light based on its index.
 * @param i Index of the light whose direction is to be retrieved.
 * @return The direction of the light as a Vector3.
 */
Vector3 render_light_get_L(int i) { LOG_FUNC_CALL(); return get_light(i)->L; }

/**
 * @brief Sets the direction of a light based on the given index and direction.
 * This function also normalizes the direction vector before setting it.
 * @param i Index of the light to modify.
 * @param L The new direction of the light to set.
 */
void render_light_set_L(int i, Vector3 L) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->L = L;
    render_set_shader_attr_vec3(light->shader_L, Vector3Normalize(light->L));
}

/**
 * @brief Retrieves the shininess factor of a light based on its index.
 * @param i Index of the light whose shininess factor is to be retrieved.
 * @return The shininess factor as a float.
 */
float render_light_get_p(int i) { LOG_FUNC_CALL(); return get_light(i)->p; }

/**
 * @brief Sets the shininess factor of a light based on the given index and
 * factor.
 * @param i Index of the light to modify.
 * @param p The new shininess factor to set.
 */
void render_light_set_p(int i, float p) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->p = p;
    render_set_shader_attr_float(light->shader_p, light->p);
}

/**
 * @brief Retrieves the intensity of a light based on its index.
 * @param i Index of the light whose intensity is to be retrieved.
 * @return The intensity of the light as a float.
 */
float render_light_get_intensity(int i) { LOG_FUNC_CALL(); return get_light(i)->intensity; }

/**
 * @brief Sets the intensity of a light based on the given index and intensity.
 * @param i Index of the light to modify.
 * @param intensity The new intensity to set.
 */
void render_light_set_intensity(int i, float intensity) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->intensity = intensity;
    render_set_shader_attr_float(light->shader_intensity, light->intensity);
}

/**
 * @brief Sets the scale factor of a light based on the given index and scale.
 * @param i Index of the light to modify.
 * @param scale The new scale factor to set.
 */
void render_light_set_scale(int i, float scale) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->scale = scale;
}

/**
 * @brief Sets the field of view of a light based on the given index and field
 * of view.
 * @param i Index of the light to modify.
 * @param fov The new field of view to set.
 */
void render_light_set_fov(int i, float fov) {
    LOG_FUNC_CALL();
    Light* light = get_light(i);
    light->fov = fov;
}

/**
 * @brief Retrieves the scale factor of a light based on its index.
 * @param i Index of the light whose scale factor is to be retrieved.
 * @return The scale factor as a float.
 */
float render_light_get_scale(int i) { LOG_FUNC_CALL(); return get_light(i)->scale; }

/**
 * @brief Retrieves the field of view of a light based on its index.
 * @param i Index of the light whose field of view is to be retrieved.
 * @return The field of view as a float.
 */
float render_light_get_fov(int i) { LOG_FUNC_CALL(); return get_light(i)->fov; }