#include "pipeline.h"
#include "hqtools/hqtools.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "shader_manager.h"
#include "transform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define Vec32Array(vec)                                                        \
    { vec.x, vec.y, vec.z }

static Camera3D current_camera;

struct render_object {
    Model model;
    Vector3 pos;
    Vector3 scale;
    Color tint;
    Vector3 rotation_axis;
    float rotation_amount;
    Matrix transform;
};

typedef struct betterBBox {
    Vector4 c1, c2, c3, c4, c5, c6, c7, c8;
} betterBBox;

typedef struct renderModelInternal {
    Model model;
    int n_instances;
    Color tint;
    Matrix transforms[RENDER_MAX_INSTANCES];
    betterBBox* mesh_bounding_boxes;
} renderModelInternal;

static Vector4 Vector32Vector4(Vector3 vec) {
    Vector4 out;
    out.x = vec.x;
    out.y = vec.y;
    out.z = vec.z;
    out.w = 1;
    return out;
}

static Vector3 Vector4toVector3(Vector4 vec) {
    Vector3 out;
    out.x = vec.x;
    out.y = vec.y;
    out.z = vec.z;
    return out;
}

static betterBBox bbox2better(BoundingBox bbox) {
    betterBBox out;
    out.c1 = Vector32Vector4(bbox.min);
    out.c2 = Vector32Vector4(bbox.min);
    out.c2.z = bbox.max.z;
    out.c3 = Vector32Vector4(bbox.min);
    out.c3.x = bbox.max.x;
    out.c4 = Vector32Vector4(bbox.min);
    out.c4.y = bbox.max.y;
    out.c5 = Vector32Vector4(bbox.min);
    out.c5.x = bbox.max.x;
    out.c5.z = bbox.max.z;
    out.c6 = Vector32Vector4(bbox.min);
    out.c6.y = bbox.max.y;
    out.c6.z = bbox.max.z;
    out.c7 = Vector32Vector4(bbox.min);
    out.c7.y = bbox.max.y;
    out.c7.x = bbox.max.x;
    out.c8 = Vector32Vector4(bbox.max);
    return out;
}

static Vector4 Vector4Transform(Vector4 v, Matrix mat) {
    Vector4 result = {0};

    float x = v.x;
    float y = v.y;
    float z = v.z;
    float w = v.w;

    result.x = mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12;
    result.y = mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13;
    result.z = mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14;
    result.w = mat.m3 * x + mat.m7 * y + mat.m11 * z + mat.m15 * w;

    return result;
}

static betterBBox bboxTransform(betterBBox bbox, Matrix transform) {
    betterBBox out;
    out.c1 = Vector4Transform(bbox.c1, transform);
    out.c2 = Vector4Transform(bbox.c2, transform);
    out.c3 = Vector4Transform(bbox.c3, transform);
    out.c4 = Vector4Transform(bbox.c4, transform);
    out.c5 = Vector4Transform(bbox.c5, transform);
    out.c6 = Vector4Transform(bbox.c6, transform);
    out.c7 = Vector4Transform(bbox.c7, transform);
    out.c8 = Vector4Transform(bbox.c8, transform);
    return out;
}

static void drawBBox(betterBBox bbox) {
    DrawLine3D(Vector4toVector3(bbox.c1), Vector4toVector3(bbox.c2), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c1), Vector4toVector3(bbox.c3), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c1), Vector4toVector3(bbox.c4), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c8), Vector4toVector3(bbox.c5), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c8), Vector4toVector3(bbox.c6), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c8), Vector4toVector3(bbox.c7), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c5), Vector4toVector3(bbox.c3), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c5), Vector4toVector3(bbox.c2), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c6), Vector4toVector3(bbox.c4), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c6), Vector4toVector3(bbox.c2), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c7), Vector4toVector3(bbox.c4), GREEN);
    DrawLine3D(Vector4toVector3(bbox.c7), Vector4toVector3(bbox.c3), GREEN);
}

static bool bboxVisible(betterBBox bbox) {
    Vector4* points = (Vector4*)&bbox;
    bool out = false;
    for (int i = 0; i < 8; i++) {
        // TraceLog(LOG_INFO,"point %g %g %g
        // %g",points[i].x,points[i].y,points[i].z,points[i].w);
        if (points[i].z <= 0)
            return true;
        // if (points[i].w * points[i].z > 0)
        //     return false;
    }
    return out;
}

static int n_objects = 0;
static int n_rmodels = 0;
static struct render_object objects[RENDERER_MAX_OBJECTS];
static renderModel rmodels[RENDERER_MAX_OBJECTS];
static Shader default_shader;
static int draw_grid = 0;
static int n_grid;
static float spacing_grid;
static int visible_meshes = 0;

renderModel render_make_model(Model model) {
    renderModel out;
    assert(out = (renderModel)malloc(sizeof(renderModelInternal)));
    out->n_instances = 0;
    out->model = model;
    assert(out->mesh_bounding_boxes =
               (betterBBox*)malloc(sizeof(betterBBox) * model.meshCount));
    for (int i = 0; i < model.meshCount; i++) {
        out->mesh_bounding_boxes[i] =
            bbox2better(GetMeshBoundingBox(model.meshes[i]));
    }
    return out;
}

static Matrix get_mesh_transform(Model model, fluxTransform transform) {
    // Calculate transformation matrix from function parameters
    // Get transform matrix (rotation -> scale -> translation)
    Vector3 rotationAxis;
    float rotationAngle;
    Quaternion qrot = QuaternionFromEuler(Wrap(transform.rot.x, 0, 2 * M_PI),
                                          Wrap(transform.rot.y, 0, 2 * M_PI),
                                          Wrap(transform.rot.z, 0, 2 * M_PI));
    QuaternionToAxisAngle(qrot, &rotationAxis, &rotationAngle);
    Matrix matScale =
        MatrixScale(transform.scale.x, transform.scale.y, transform.scale.z);
    Matrix matRotation = MatrixRotate(rotationAxis, rotationAngle * DEG2RAD);
    Matrix matTranslation =
        MatrixTranslate(transform.pos.x, transform.pos.y, transform.pos.z);

    Matrix matTransform =
        MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

    // Combine model transformation matrix (model.transform) with matrix
    // generated by function parameters (matTransform)
    return MatrixMultiply(model.transform, matTransform);
}

void render_reset_instances(renderModel model) {
    assert(model);
    model->n_instances = 0;
}

void render_add_model_instance(renderModel model, fluxTransform transform) {
    assert(model);
    assert(model->n_instances < RENDER_MAX_INSTANCES);
    model->transforms[model->n_instances] =
        get_mesh_transform(model->model, transform);
    model->n_instances++;
}

void render_rmodel(renderModel rmodel, Color tint) {
    assert(rmodel);
    assert(n_rmodels < RENDERER_MAX_OBJECTS);
    rmodel->tint = tint;
    rmodels[n_rmodels] = rmodel;
    n_rmodels++;
}

void render_free_model(renderModel model) {
    assert(model);
    assert(model->mesh_bounding_boxes);
    free(model->mesh_bounding_boxes);
    free(model);
}

void render_init(void) {
    render_load_default_shader();
    default_shader = render_get_default_shader();
}

static void draw_rmodel(renderModel rmodel, Shader shader, Camera3D camera,
                        Matrix vp) {
    Model model = rmodel->model;

    Shader old_shader = model.materials[0].shader;
    model.materials[0].shader = shader;

    betterBBox* bboxes = rmodel->mesh_bounding_boxes;

    Color tint = rmodel->tint;

    for (int i = 0; i < model.meshCount; i++) {
        Color color = model.materials[model.meshMaterial[i]]
                          .maps[MATERIAL_MAP_DIFFUSE]
                          .color;
        Color colorTint = WHITE;
        colorTint.r = (unsigned char)((((float)color.r / 255.0f) *
                                       ((float)tint.r / 255.0f)) *
                                      255.0f);
        colorTint.g = (unsigned char)((((float)color.g / 255.0f) *
                                       ((float)tint.g / 255.0f)) *
                                      255.0f);
        colorTint.b = (unsigned char)((((float)color.b / 255.0f) *
                                       ((float)tint.b / 255.0f)) *
                                      255.0f);
        colorTint.a = (unsigned char)((((float)color.a / 255.0f) *
                                       ((float)tint.a / 255.0f)) *
                                      255.0f);

        model.materials[model.meshMaterial[i]]
            .maps[MATERIAL_MAP_DIFFUSE]
            .color = colorTint;

        // TODO: only call this on model registration!!!
        betterBBox bbox =
            bboxes[i]; // bbox2better(GetMeshBoundingBox(model.meshes[i]));

        // DrawMeshInstanced(model.meshes[i],model.materials[model.meshMaterial[i]],rmodel->transforms,rmodel->n_instances);
        for (int j = 0; j < rmodel->n_instances; j++) {

            Matrix transform = rmodel->transforms[j];
            betterBBox transformed = bboxTransform(bbox, transform);

            // Matrix mvp = MatrixMultiply(vp,transform);
            betterBBox projected = bboxTransform(transformed, vp);
            // if (i == 0)
            //     TraceLog(LOG_INFO,"proj %g %g %g
            //     %g",projected.c1.x,projected.c1.y,projected.c1.z,
            //     projected.c1.w);
            // TraceLog(LOG_INFO,"break");
            if (bboxVisible(projected)) {
                // TraceLog(LOG_INFO,"drawing\n");
                visible_meshes++;
                DrawMesh(model.meshes[i],
                         model.materials[model.meshMaterial[i]], transform);
                // drawBBox(transformed);
            }
        }

        model.materials[model.meshMaterial[i]]
            .maps[MATERIAL_MAP_DIFFUSE]
            .color = color;
    }

    model.materials[0].shader = old_shader;
}

void render_draw_all_no_shader(Camera3D camera) {
    Matrix view = GetCameraMatrix(camera);
    for (int i = 0; i < n_rmodels; i++) {
        draw_rmodel(rmodels[i], render_get_empty_shader(), camera, view);
    }
}

void render_begin(Camera3D camera) {
    current_camera = camera;
    render_set_cam_pos(current_camera.position);
    n_objects = 0;
    draw_grid = 0;
    n_rmodels = 0;
}

static void draw_all(Camera3D camera) {
    Matrix view = GetCameraMatrix(camera);
    visible_meshes = 0;
    for (int i = 0; i < n_rmodels; i++) {
        draw_rmodel(rmodels[i], default_shader, camera, view);
    }
}

int render_get_visible_meshes(void) { return visible_meshes; }

void render_end(void) {
    // render_calculate_shadows();
    ClearBackground(BLACK);
    BeginMode3D(current_camera);

    render_draw_skybox();

    draw_all(current_camera);

    if (draw_grid)
        DrawGrid(n_grid, spacing_grid);

    EndMode3D();
}

void render_draw_grid(int n, float s) {
    draw_grid = 1;
    n_grid = n;
    spacing_grid = s;
}

void render_close(void) { render_unload_default_shader(); }

Camera3D render_get_current_cam(void) { return current_camera; }
