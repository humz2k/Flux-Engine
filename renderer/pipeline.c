#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "transform.h"
#include "pipeline.h"
#include "shader_manager.h"

#define Vec32Array(vec) { vec.x, vec.y, vec.z }

static Camera3D current_camera;

struct render_object{
    Model model;
    Vector3 pos;
    Vector3 scale;
    Color tint;
    Vector3 rotation_axis;
    float rotation_amount;
};

typedef struct renderModelInternal{
    Model model;
    int n_instances;
    Color tints[RENDER_MAX_INSTANCES];
    Matrix instances[RENDER_MAX_INSTANCES];
} renderModelInternal;

static int n_objects = 0;
struct render_object objects[RENDERER_MAX_OBJECTS];
static Shader default_shader;
static int draw_grid = 0;
static int n_grid;
static float spacing_grid;

renderModel render_make_model(Model model){
    renderModel out = (renderModel)malloc(sizeof(renderModelInternal));
    out->n_instances = 0;
    out->model = model;
    return out;
}

void render_add_model_instance(renderModel model, fluxTransform transform, Color tint){
    assert(model);
    assert(model->n_instances < RENDER_MAX_INSTANCES);
    model->tints[model->n_instances] = tint;
    // finish me...
}

void render_free_model(renderModel model){
    assert(model);
    free(model);
}

void render_init(void){
    render_load_default_shader();
    default_shader = render_get_default_shader();
}

static void draw_object(struct render_object obj){
    Shader old_shader = obj.model.materials[0].shader;
    obj.model.materials[0].shader = default_shader;
    DrawModelEx(obj.model,obj.pos,obj.rotation_axis,obj.rotation_amount,obj.scale,obj.tint);
    obj.model.materials[0].shader = old_shader;
}

static void draw_object_no_shader(struct render_object obj){
    if(obj.model.materials[0].shader.id == default_shader.id){
        //TraceLog(LOG_ERROR,"wrong shader!!!");
    }
    DrawModelEx(obj.model,obj.pos,obj.rotation_axis,obj.rotation_amount,obj.scale,obj.tint);
}

void render_draw_all_no_shader(void){
    for (int i = 0; i < n_objects; i++){
        draw_object_no_shader(objects[i]);
    }
}

void render_begin(Camera3D camera){
    current_camera = camera;
    render_set_cam_pos(current_camera.position);
    n_objects = 0;
    draw_grid = 0;
}

void render_model(Model model, fluxTransform transform, Color tint){
    assert(n_objects < RENDERER_MAX_OBJECTS);
    struct render_object* obj = &objects[n_objects];
    n_objects++;

    obj->model = model;
    obj->pos = transform.pos;
    obj->scale = transform.scale;
    Quaternion qrot = QuaternionFromEuler(Wrap(transform.rot.x,0,2*M_PI),Wrap(transform.rot.y,0,2*M_PI),Wrap(transform.rot.z,0,2*M_PI));
    QuaternionToAxisAngle(qrot,&obj->rotation_axis,&obj->rotation_amount);
    obj->tint = tint;
}

void render_end(void){
    render_calculate_shadows();
    BeginMode3D(current_camera);

    render_draw_skybox();

    for (int i = 0; i < n_objects; i++){
        draw_object(objects[i]);
    }

    if (draw_grid)
        DrawGrid(n_grid,spacing_grid);

    EndMode3D();
}

void render_draw_grid(int n, float s){
    draw_grid = 1;
    n_grid = n;
    spacing_grid = s;
}

void render_close(void){
    render_unload_default_shader();
}

Camera3D render_get_current_cam(void){
    return current_camera;
}

