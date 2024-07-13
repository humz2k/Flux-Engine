#define SCRIPT mailboxScript
#include "fluxScript.h"
#include "game_callbacks.h"
#include "raymath.h"
#include "display_size.h"

extern bool mailboxes[1000];
extern fluxTransform player_transform;
extern hstr killed_by;

script_data {
    int id;
    hstr name;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args) {
    assert(hstr_array_len(args) == 2);
    data->id = atoi(hstr_unpack(hstr_array_get(args, 0)));
    data->name = hstr_incref(hstr_array_get(args, 1));
    TraceLog(INFO, "I am mailbox %d, %s", data->id, hstr_unpack(data->name));
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data) {
    flux_gameobject_set_visible(obj, mailboxes[data->id]);
    if (mailboxes[data->id]) {

        fluxTransform transform = flux_gameobject_get_transform(obj);
        Vector3 p1 = player_transform.pos;
        p1.y = 0;
        Vector3 p2 = transform.pos;
        p2.y = 0;
        Vector3 diff = Vector3Subtract(p1, p2);
        if (Vector3LengthSqr(diff) < 3.0f) {
            killed_by = hstr_incref(data->name);
            flux_send_signal(10);
        }
        Vector3 diffn = Vector3Normalize(diff);
        diff = Vector3Scale(diffn, GetFrameTime() * 2.0);
        transform.pos = Vector3Add(transform.pos, diff);

        diffn.y = 0;
        transform.rot.y =
            Vector2LineAngle((Vector2){p2.x, p2.z}, (Vector2){p1.x, p1.z}) +
            1.57;

        flux_gameobject_set_transform(obj, transform);
    }
    mailboxes[data->id] = false;
}

fluxCallback onDestroy(fluxGameObject obj, script_data* data) {
    hstr_decref(data->name);
}

fluxCallback onDraw(fluxGameObject obj, script_data* data) {}

fluxCallback onDraw2D(fluxGameObject obj, script_data* data) {
    if (!flux_gameobject_is_visible(obj))
        return;
    Camera3D current_cam = render_get_current_cam();
    Vector3 pos = flux_gameobject_get_transform(obj).pos;
    Matrix cam_mat = GetCameraMatrix(current_cam);
    Vector3 transformed = Vector3Transform(pos, cam_mat);
    if (transformed.z <= 0) {
        pos.y += 1;
        Vector2 screen_pos = GetWorldToScreenEx(pos, current_cam, GetDisplayWidth(), GetDisplayHeight());
        Color color = RED;
        float dist2 = Vector3DistanceSqr(pos, current_cam.position);
        if (dist2 > 100.0f) {
            color.a = (255.0f * 100.0f) / dist2;
        }
        const char* text = hstr_unpack(data->name);
        int fontsize = 10;
        DrawText(text, screen_pos.x - (MeasureText(text, fontsize) / 2),
                 screen_pos.y, fontsize, color);
    }
}