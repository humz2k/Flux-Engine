#define SCRIPT mailboxScript
#include "fluxScript.h"
#include "game_callbacks.h"
#include "raymath.h"

extern bool mailboxes[1000];
extern fluxTransform player_transform;

script_data{
    int id;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    assert(hstr_array_len(args) == 1);
    data->id = atoi(hstr_unpack(hstr_array_get(args,0)));
    TraceLog(INFO,"I am mailbox %d",data->id);
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    flux_gameobject_set_visible(obj,mailboxes[data->id]);
    if (mailboxes[data->id]){

        fluxTransform transform = flux_gameobject_get_transform(obj);
        Vector3 p1 = player_transform.pos;
        p1.y = 0;
        Vector3 p2 = transform.pos;
        p2.y = 0;
        Vector3 diff = Vector3Subtract(p1,p2);
        if (Vector3LengthSqr(diff) < 3.0f){
            flux_send_signal(10);
        }
        diff = Vector3Scale(Vector3Normalize(diff),GetFrameTime() * 2.0);
        transform.pos = Vector3Add(transform.pos,diff);
        //Quaternion q = QuaternionFromMatrix(MatrixLookAt(transform.pos,player_transform.pos,(Vector3){0.0,1.0,0.0}));

        //transform.rot = QuaternionToEuler(q);
        flux_gameobject_set_transform(obj,transform);
    }
    mailboxes[data->id] = false;
}