#define SCRIPT houseScript
#include "fluxScript.h"
#include "game_callbacks.h"

extern bool mailboxes[1000];
extern bool bubble[1000];
extern fluxTransform player_transform;
extern int n_remaining;

script_data{
    int* my_mailboxes;
    int n_mailboxes;
    fluxTransform transform;
    float size;
    int id;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    //assert(hstr_array_len(args) == 1);
    data->size = 20;
    data->my_mailboxes = malloc(sizeof(int) * hstr_array_len(args));
    data->n_mailboxes = hstr_array_len(args);
    for (int i = 0; i < data->n_mailboxes; i++){
        data->my_mailboxes[i] = atoi(hstr_unpack(hstr_array_get(args,i)));
    }
    TraceLog(INFO,"n mailboxes = %d",data->n_mailboxes);
    data->transform = flux_gameobject_get_transform(obj);
    data->id = n_remaining;
    n_remaining++;
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    data->transform.scale.x = data->size / 100.0f;
    data->transform.scale.y = data->size / 100.0f;
    data->transform.scale.z = data->size / 100.0f;
    flux_gameobject_set_transform(obj,data->transform);
}

fluxCallback afterUpdate(fluxGameObject obj, script_data* data){
    if (!flux_gameobject_is_visible(obj))return;
    Vector3 p1 = player_transform.pos;
    Vector3 p2 = data->transform.pos;
    p1.y = 0;
    p2.y = 0;
    Vector3 diff = Vector3Subtract(p1,p2);
    if (Vector3Length(diff) < 1.5f){
        TraceLog(INFO,"in house!");
        //for (int i = 0; i < 1000; i++){
        //    mailboxes[i] = false;
        //}
        for (int i = 0; i < data->n_mailboxes; i++){
            mailboxes[data->my_mailboxes[i]] = true;
        }
        RayCollision collision = GetRayCollisionSphere(GetMouseRay((Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f},render_get_current_cam()),data->transform.pos,data->transform.scale.x);
        if (collision.hit){
            TraceLog(LOG_INFO,"HIT");
            data->size -= GetFrameTime() * 10.0f;
        }
        //data->transform.pos = Vector3Add(data->transform.pos,Vector3Scale(Vector3Subtract(player_transform.pos,data->transform.pos),(1.0f/data->size) * 0.5f * 0.02f));
        //render_set_ka(0);
        if (data->size < 1.0f){
            TraceLog(INFO,"yes!");
            flux_gameobject_set_visible(obj,false);
            bubble[data->id] = false;
            n_remaining--;
        }
        render_disable_skybox();
    } //else {
      //  for (int i = 0; i < data->n_mailboxes; i++){
      //      mailboxes[data->my_mailboxes[i]] = false;
      //  }
    //}
}

fluxCallback onDestroy(fluxGameObject obj, script_data* data){
    free(data->my_mailboxes);
}