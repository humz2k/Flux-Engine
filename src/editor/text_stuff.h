#ifndef _FLUX_EDITOR_TEXT_STUFF_H_
#define _FLUX_EDITOR_TEXT_STUFF_H_

#include "raylib.h"
#include "raymath.h"
#include "coords.h"

extern Font editor_font;

static inline void draw_text_in_editor_rect(editorRect rect, Vector2 offset, const char* text, Color color){
    Rectangle pix_rect = editor_rect_to_ray_rect(rect);
    pix_rect.x += offset.x;
    pix_rect.y += offset.y;
    Vector2 base_size = MeasureTextEx(editor_font,text,20,1);
    float text_width_mul = pix_rect.width / base_size.x;
    float text_height_mul = pix_rect.height / base_size.y;
    float text_mul = text_width_mul < text_height_mul ? text_width_mul : text_height_mul;
    text_mul *= 0.8;
    Vector2 good_size = MeasureTextEx(editor_font,text,20 * text_mul,1 * text_mul);
    Vector2 pos = (Vector2){pix_rect.x + pix_rect.width * 0.5f,pix_rect.y + pix_rect.height * 0.5f};

    pos.x -= good_size.x * 0.5;
    pos.y -= good_size.y * 0.5;

    DrawTextEx(editor_font,text,pos,20 * text_mul, 1 * text_mul,color);
}

static inline void draw_text_in_editor_rect_left_aligned(editorRect rect, Vector2 offset, const char* text, Color color){
    Rectangle pix_rect = editor_rect_to_ray_rect(rect);
    pix_rect.x += offset.x;
    pix_rect.y += offset.y;
    Vector2 base_size = MeasureTextEx(editor_font,text,20,1);
    float text_width_mul = pix_rect.width / base_size.x;
    float text_height_mul = pix_rect.height / base_size.y;
    float text_mul = text_width_mul < text_height_mul ? text_width_mul : text_height_mul;

    Vector2 good_size = MeasureTextEx(editor_font,text,20 * text_mul,1 * text_mul);
    Vector2 pos = (Vector2){pix_rect.x,pix_rect.y + pix_rect.height * 0.5f};

    pos.y -= good_size.y * 0.5;

    DrawTextEx(editor_font,text,pos,20 * text_mul, 1 * text_mul,color);
}

#endif