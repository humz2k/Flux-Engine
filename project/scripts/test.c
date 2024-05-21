#define SCRIPT test
#include "fluxScript.h"
#include "game_callbacks.h"

extern float move_speed;
extern float sensitivity;
extern fluxTransform player_transform;
extern int n_remaining;

#define MOVE_SPEED (move_speed)
#define SENSITIVITY (sensitivity)

script_data{
    fluxTransform transform;
};

fluxCallback onInit(fluxGameObject obj, script_data* data, hstrArray args){
    data->transform = flux_gameobject_get_transform(obj);
    player_transform = data->transform;
    DisableCursor();
}

fluxCallback onUpdate(fluxGameObject obj, script_data* data){
    //TraceLog(LOG_INFO,"UPDATE...");

    Vector3 forward = Vector3Normalize(Vector3RotateByAxisAngle((Vector3){0.0f,0.0f,1.0f},(Vector3){0.0f,1.0f,0.0f},data->transform.rot.y));
    Vector3 left = Vector3Normalize(Vector3RotateByAxisAngle((Vector3){1.0f,0.0f,0.0f},(Vector3){0.0f,1.0f,0.0f},data->transform.rot.y));


    Vector3 outdir = Vector3Zero();

    if (IsKeyDown(KEY_W)){
        outdir = Vector3Add(outdir,forward);
    }
    if (IsKeyDown(KEY_S)){
        outdir = Vector3Subtract(outdir,forward);
    }
    if (IsKeyDown(KEY_D)){
        outdir = Vector3Subtract(outdir,left);
    }
    if (IsKeyDown(KEY_A)){
        outdir = Vector3Add(outdir,left);
    }

    outdir = Vector3Scale(Vector3Normalize(outdir),MOVE_SPEED * GetFrameTime());
    data->transform.pos = Vector3Add(data->transform.pos,outdir);

    if (IsKeyDown(KEY_SPACE)){
        data->transform.pos.y += MOVE_SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_LEFT_SHIFT)){
        data->transform.pos.y -= MOVE_SPEED * GetFrameTime();
    }

    Vector2 delta = GetMouseDelta();
    data->transform.rot.y -= delta.x * SENSITIVITY;
    data->transform.rot.x -= delta.y * SENSITIVITY;
    data->transform.rot.y = Wrap(data->transform.rot.y,0,2*M_PI);
    data->transform.rot.x = Clamp(data->transform.rot.x,-M_PI*0.5f + 0.5,M_PI*0.5f - 0.5);
    flux_gameobject_set_transform(obj,data->transform);

    player_transform = data->transform;

    //TraceLog(INFO,"test sending signal 10");
    //flux_send_signal(10);
}

fluxCallback onDraw(fluxGameObject obj, script_data* data){
    //DrawCircle(GetScreenWidth()*0.5,GetScreenHeight()*0.5,10,RED);
}

fluxCallback onDraw2D(fluxGameObject obj, script_data* data){
    DrawCircle(GetScreenWidth()*0.5,GetScreenHeight()*0.5,10,RED);
    char text[100];
    sprintf(text,"%d",n_remaining);
    DrawText(text,100,100,30,BLUE);
}