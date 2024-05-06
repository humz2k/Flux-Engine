/**
 * @file pipeline.c
 * @brief This file implements the rendering pipeline for a 3D graphics engine using Raylib.
 *        It provides functions to manage and render 3D models with support for multiple
 *        instances, bounding box calculations, and a custom shader system. It uses advanced
 *        rendering techniques including matrix transformations for model positioning and
 *        viewing, bounding box visibility checks, and dynamic memory management for render
 *        models.
 **/

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

/**
 * @brief Converts a Vector3 to an array format, typically for OpenGL interoperation.
 * @param vec Vector3 structure to be converted.
 */
#define Vec32Array(vec)                                                        \
    { vec.x, vec.y, vec.z }

// Current camera view in the 3D scene, typically updated per frame or on event.
static Camera3D current_camera;

/**
 * @struct render_object
 * @brief Represents a 3D object including its model data and transformation attributes.
 *
 * @var Model model
 * @var Vector3 pos Position vector.
 * @var Vector3 scale Scaling vector.
 * @var Color tint Color tinting.
 * @var Vector3 rotation_axis Axis of rotation.
 * @var float rotation_amount Amount of rotation.
 * @var Matrix transform Transformation matrix applied to the object.
 */
struct render_object {
    Model model;
    Vector3 pos;
    Vector3 scale;
    Color tint;
    Vector3 rotation_axis;
    float rotation_amount;
    Matrix transform;
};

/**
 * @typedef betterBBox
 * @brief Enhanced bounding box structure with eight corners defined in 4D space.
 */
typedef struct betterBBox {
    Vector4 c1, c2, c3, c4, c5, c6, c7, c8;
} betterBBox;


/**
 * @struct renderModelInternal
 * @brief Manages a model and its instances for batch rendering, including transformations and bounding boxes.
 *
 * @var Model model
 * @var int n_instances Number of instances of the model.
 * @var Color tint Color tint applied to all instances.
 * @var Matrix transforms Array of transformation matrices for each instance.
 * @var betterBBox* mesh_bounding_boxes Pointer to bounding boxes for each mesh in the model.
 */
typedef struct renderModelInternal {
    Model model;
    int n_instances;
    Color tint;
    Matrix transforms[RENDER_MAX_INSTANCES];
    betterBBox* mesh_bounding_boxes;
} renderModelInternal;

/**
 * @brief Converts a Vector3 to a Vector4 by adding a w component set to 1.
 * @param vec Vector3 to convert.
 * @return Vector4 with added w component.
 */
static Vector4 Vector32Vector4(Vector3 vec) {
    Vector4 out;
    out.x = vec.x;
    out.y = vec.y;
    out.z = vec.z;
    out.w = 1;
    return out;
}

/**
 * @brief Converts a Vector4 back to a Vector3 by dropping the w component.
 * @param vec Vector4 to convert.
 * @return Vector3 resulting from dropping w.
 */
static Vector3 Vector4toVector3(Vector4 vec) {
    Vector3 out;
    out.x = vec.x;
    out.y = vec.y;
    out.z = vec.z;
    return out;
}

/**
 * @brief Converts a standard BoundingBox to a betterBBox format with corners defined in 4D space.
 * @param bbox Standard BoundingBox to convert.
 * @return Converted betterBBox structure.
 */
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

/**
 * @brief Transforms a Vector4 using a specified matrix.
 * @param v Vector4 to transform.
 * @param mat Matrix used for transformation.
 * @return Transformed Vector4.
 */
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

/**
 * @brief Transforms a betterBBox using a specified matrix.
 * @param bbox betterBBox to transform.
 * @param transform Matrix used for transformation.
 * @return Transformed betterBBox.
 */
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

/**
 * @brief Draws a 3D bounding box in the scene.
 * @param bbox betterBBox to draw.
 */
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

/**
 * @brief Checks if any part of a bounding box is visible within the view.
 * @param bbox betterBBox to check for visibility.
 * @return True if visible, false otherwise.
 */
static bool bboxVisible(betterBBox bbox) {
    Vector4* points = (Vector4*)&bbox;
    bool out = false;
    for (int i = 0; i < 8; i++) {
        if (points[i].z <= 0)
            return true;
    }
    return out;
}

/**
 * @var static int n_objects
 * @brief Counter for the number of render objects currently managed by the pipeline.
 */
static int n_objects = 0;

/**
 * @var static int n_rmodels
 * @brief Counter for the number of render models currently registered for rendering.
 */
static int n_rmodels = 0;

/**
 * @var static struct render_object objects[RENDERER_MAX_OBJECTS]
 * @brief Array of render objects, storing up to RENDERER_MAX_OBJECTS items for rendering.
 */
static struct render_object objects[RENDERER_MAX_OBJECTS];

/**
 * @var static renderModel rmodels[RENDERER_MAX_OBJECTS]
 * @brief Array of render models, capable of storing up to RENDERER_MAX_OBJECTS models for batch rendering.
 */
static renderModel rmodels[RENDERER_MAX_OBJECTS];

/**
 * @var static Shader default_shader
 * @brief Default shader used for rendering when no other shader is specified.
 */
static Shader default_shader;

/**
 * @var static int draw_grid
 * @brief Flag indicating whether a grid should be drawn in the scene (1) or not (0).
 */
static int draw_grid = 0;

/**
 * @var static int n_grid
 * @brief Number of grid lines to draw when draw_grid is enabled.
 */
static int n_grid;

/**
 * @var static float spacing_grid
 * @brief Spacing between grid lines when draw_grid is enabled.
 */
static float spacing_grid;

/**
 * @var static int visible_meshes
 * @brief Counter for the number of meshes that are currently visible in the scene.
 */
static int visible_meshes = 0;

/**
 * @brief Creates a new render model from a given model, initializing its bounding boxes and setting instance count to zero.
 * @param model The base model from which to create the render model.
 * @return Newly created renderModel with initialized fields and allocated bounding boxes for each mesh.
 * @note This function asserts that memory allocation succeeds and logs the creation process.
 */
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
    TraceLog(LOG_INFO, "made model, %d meshes", model.meshCount);
    return out;
}

/**
 * @brief Computes the transformation matrix for a model based on specified rotation, scale, and translation parameters.
 * @param model The model to transform.
 * @param transform The fluxTransform structure containing rotation (in radians), scale, and translation vectors.
 * @return The combined transformation matrix resulting from applying the scale, rotation, and translation to the model.
 * @details This function first converts quaternion rotations to axis-angle, then constructs individual transformation matrices for scale, rotation, and translation, and finally combines them in the correct order (scale, then rotation, then translation) to form the final transformation matrix.
 */
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

/**
 * @brief Resets the number of instances for a specific model to zero.
 * @param model Render model to reset.
 */
void render_reset_instances(renderModel model) {
    assert(model);
    model->n_instances = 0;
}

/**
 * @brief Adds a model instance to the rendering queue with a specified transformation.
 * @param model Render model to modify.
 * @param transform Transformation to apply to the model instance.
 */
void render_add_model_instance(renderModel model, fluxTransform transform) {
    assert(model);
    assert(model->n_instances < RENDER_MAX_INSTANCES);
    model->transforms[model->n_instances] =
        get_mesh_transform(model->model, transform);
    model->n_instances++;
    // TraceLog(LOG_INFO,"adding model instances, %g %g %g, %g %g %g, %g %g
    // %g",transform.pos.x,transform.pos.y,transform.pos.z,transform.rot.x,transform.rot.y,transform.rot.z,transform.scale.x,transform.scale.y,transform.scale.z);
}

/**
 * @brief Registers a render model for rendering, setting its tint color and adding it to the rendering queue.
 * @param rmodel The render model to be rendered.
 * @param tint The color tint to apply to the model.
 * @note Asserts that the render model is not NULL and that the total number of render models does not exceed the maximum allowed.
 */
void render_rmodel(renderModel rmodel, Color tint) {
    assert(rmodel);
    assert(n_rmodels < RENDERER_MAX_OBJECTS);
    rmodel->tint = tint;
    rmodels[n_rmodels] = rmodel;
    n_rmodels++;
}

/**
 * @brief Frees allocated memory for a render model.
 * @param model Render model to free.
 */
void render_free_model(renderModel model) {
    assert(model);
    assert(model->mesh_bounding_boxes);
    free(model->mesh_bounding_boxes);
    free(model);
}

/**
 * @brief Initializes render models, loads the default shader, and sets initial rendering configurations.
 */
void render_init(void) {
    render_load_default_shader();
    default_shader = render_get_default_shader();
}

/**
 * @brief Renders a model using a specified shader and camera, applying viewport transformations.
 * @param rmodel Render model to draw.
 * @param shader Shader to use for rendering.
 * @param camera Camera to use for the current view.
 * @param vp Combined view and projection matrix.
 */
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
            bboxes[i];

        for (int j = 0; j < rmodel->n_instances; j++) {

            Matrix transform = rmodel->transforms[j];
            betterBBox transformed = bboxTransform(bbox, transform);
            betterBBox projected = bboxTransform(transformed, vp);

            if (bboxVisible(projected)) {
                visible_meshes++;
                DrawMesh(model.meshes[i],
                         model.materials[model.meshMaterial[i]], transform);
            }
        }

        model.materials[model.meshMaterial[i]]
            .maps[MATERIAL_MAP_DIFFUSE]
            .color = color;
    }

    model.materials[0].shader = old_shader;
}

/**
 * @brief Draws all registered models using a default shader and custom camera setup.
 * @param camera Custom camera configuration.
 */
void render_draw_all_no_shader(Camera3D camera) {
    Matrix view = GetCameraMatrix(camera);
    for (int i = 0; i < n_rmodels; i++) {
        draw_rmodel(rmodels[i], render_get_empty_shader(), camera, view);
    }
}

/**
 * @brief Starts the rendering process, updating the camera position and resetting rendering counters.
 * @param camera Camera configuration for the current frame.
 */
void render_begin(Camera3D camera) {
    current_camera = camera;
    render_set_cam_pos(current_camera.position);
    n_objects = 0;
    draw_grid = 0;
    n_rmodels = 0;
}

/**
 * @brief Draws all render models with the current settings and updates the display.
 * @param camera Current camera configuration.
 */
static void draw_all(Camera3D camera) {
    Matrix view = GetCameraMatrix(camera);
    visible_meshes = 0;
    for (int i = 0; i < n_rmodels; i++) {
        draw_rmodel(rmodels[i], default_shader, camera, view);
    }
}

/**
 * @brief Returns the number of meshes currently visible in the scene.
 * @return Integer representing the number of visible meshes.
 */
int render_get_visible_meshes(void) { return visible_meshes; }

/**
 * @brief Ends the rendering frame, handles post-processing tasks, and updates the viewport.
 */
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

/**
 * @brief Configures the drawing grid parameters for visual guides in the scene.
 * @param n Number of grid lines.
 * @param s Spacing between grid lines.
 */
void render_draw_grid(int n, float s) {
    draw_grid = 1;
    n_grid = n;
    spacing_grid = s;
}

/**
 * @brief Cleans up rendering resources and unloads shaders at application closure.
 */
void render_close(void) { render_unload_default_shader(); }

/**
 * @brief Retrieves the current camera configuration used in the rendering pipeline.
 * @return Current camera setup.
 */
Camera3D render_get_current_cam(void) { return current_camera; }
