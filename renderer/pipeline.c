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

static int n_objects = 0;
struct render_object objects[RENDERER_MAX_OBJECTS];
static Shader default_shader;

void render_init(void){
    render_load_default_shader();
    default_shader = render_get_default_shader();
    //cam_pos_loc = GetShaderLocation(default_shader,"camPos");
}

static void draw_object(struct render_object obj){
    obj.model.materials[0].shader = default_shader;
    DrawModelEx(obj.model,obj.pos,obj.rotation_axis,obj.rotation_amount,obj.scale,obj.tint);
}

void render_begin(Camera3D camera){
    current_camera = camera;
    render_set_cam_pos(current_camera.position);
    n_objects = 0;
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
    BeginMode3D(current_camera);
    for (int i = 0; i < n_objects; i++){
        draw_object(objects[i]);
    }
    EndMode3D();
}

void render_close(void){
    render_unload_default_shader();
}

