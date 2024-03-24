#include "raylib.h"
#include "raymath.h"
#include "coords.h"
#include "display_size.h"

editorPos make_pos_pixel(float value){
    editorPos coord;
    coord.t = pos_pixel;
    coord.value = value;
    return coord;
}

editorPos make_pos_relative(float value){
    editorPos coord;
    coord.t = pos_relative;
    coord.value = value;
    return coord;
}

editorCoord make_coord(editorPos x, editorPos y){
    editorCoord out;
    out.x = x;
    out.y = y;
    return out;
}

Vector2 coord_to_vector2(editorCoord coord){
    Vector2 out;
    if (coord.x.t == pos_pixel){
        out.x = coord.x.value;
    } else {
        out.x = coord.x.value * ((float)GetDisplayWidth());
    }
    if (coord.y.t == pos_pixel){
        out.y = coord.y.value;
    } else {
        out.y = coord.y.value * ((float)GetDisplayHeight());
    }
    return out;
}

editorRect make_rect(editorCoord p0, editorCoord p1){
    editorRect out;
    out.p0 = p0;
    out.p1 = p1;
    return out;
}

Rectangle editor_rect_to_ray_rect(editorRect rect){
    Rectangle out;
    Vector2 p0 = coord_to_vector2(rect.p0);
    Vector2 p1 = coord_to_vector2(rect.p1);
    Vector2 size = Vector2Subtract(p1,p0);
    out.x = p0.x;
    out.y = p0.y;
    out.width = size.x;
    out.height = size.y;
    return out;
}

/*editorRect get_relative_editor_rect(editorRect parect, editorRect child){
    editorRect out;

}*/

void draw_editor_rect(editorRect rect, Color color, Vector2 offset){
    Rectangle pix_rect = editor_rect_to_ray_rect(rect);
    pix_rect.x += offset.x;
    pix_rect.y += offset.y;
    DrawRectangleRec(pix_rect,color);
}

void draw_editor_rect_shadow(editorRect rect, Color color, Vector2 offset, editorPos shadow_size, bool top, bool bottom, bool left, bool right){
    Rectangle pix_rect = editor_rect_to_ray_rect(rect);
    pix_rect.x += offset.x;
    pix_rect.y += offset.y;
    //TraceLog(LOG_INFO,"???");
    DrawRectangleRec(pix_rect,color);
    float pix_shadow_x, pix_shadow_y;
    if (shadow_size.t == pos_pixel){
        pix_shadow_x = shadow_size.value;
        pix_shadow_y = shadow_size.value;
    } else {
        pix_shadow_x = ((float)GetDisplayWidth()) * shadow_size.value;
        pix_shadow_y = ((float)GetDisplayHeight()) * shadow_size.value;
    }
    if (top){
        Rectangle top_shadow_rect = pix_rect;
        top_shadow_rect.y -= pix_shadow_y;
        top_shadow_rect.height = pix_shadow_y;
        //printf("%g\n",top_shadow_rect.height);
        //DrawRectangleRec(top_shadow_rect,BLACK);
        DrawRectangleGradientEx(top_shadow_rect,(Color){0,0,0,0},BLACK,BLACK,(Color){0,0,0,0});
    }
    if (bottom){
        Rectangle bottom_shadow_rect = pix_rect;
        bottom_shadow_rect.y += bottom_shadow_rect.height;
        bottom_shadow_rect.height = pix_shadow_y;
        DrawRectangleGradientEx(bottom_shadow_rect,BLACK,(Color){0,0,0,0},(Color){0,0,0,0},BLACK);
    }
    if (right){
        Rectangle right_shadow_rect = pix_rect;
        right_shadow_rect.x += right_shadow_rect.width;
        right_shadow_rect.width = pix_shadow_x;
        DrawRectangleGradientEx(right_shadow_rect,BLACK,BLACK,(Color){0,0,0,0},(Color){0,0,0,0});
    }
    if (left){
        Rectangle left_shadow_rect = pix_rect;
        left_shadow_rect.x -= pix_shadow_x;
        left_shadow_rect.width = pix_shadow_x;
        DrawRectangleGradientEx(left_shadow_rect,(Color){0,0,0,0},(Color){0,0,0,0},BLACK,BLACK);
    }
    if (right && bottom){
        Rectangle rb_corner_rect = pix_rect;
        rb_corner_rect.x += rb_corner_rect.width;
        rb_corner_rect.y += rb_corner_rect.height;
        rb_corner_rect.width = pix_shadow_x;
        rb_corner_rect.height = pix_shadow_y;
        DrawRectangleGradientEx(rb_corner_rect,BLACK,(Color){0,0,0,0},(Color){0,0,0,0},(Color){0,0,0,0});
    }
    if (right && top){
        Rectangle rt_corner_rect = pix_rect;
        rt_corner_rect.x += rt_corner_rect.width;
        rt_corner_rect.y -= pix_shadow_y;
        rt_corner_rect.width = pix_shadow_x;
        rt_corner_rect.height = pix_shadow_y;
        DrawRectangleGradientEx(rt_corner_rect,(Color){0,0,0,0},BLACK,(Color){0,0,0,0},(Color){0,0,0,0});
    }
    if (left && bottom){
        Rectangle lb_corner_rect = pix_rect;
        lb_corner_rect.x -= pix_shadow_x;
        lb_corner_rect.y += lb_corner_rect.height;
        lb_corner_rect.width = pix_shadow_x;
        lb_corner_rect.height = pix_shadow_y;
        DrawRectangleGradientEx(lb_corner_rect,(Color){0,0,0,0},(Color){0,0,0,0},(Color){0,0,0,0},BLACK);
    }

    if (left && top){
        Rectangle lt_corner_rect = pix_rect;
        lt_corner_rect.x -= pix_shadow_x;
        lt_corner_rect.y -= pix_shadow_y;
        lt_corner_rect.width = pix_shadow_x;
        lt_corner_rect.height = pix_shadow_y;
        DrawRectangleGradientEx(lt_corner_rect,(Color){0,0,0,0},(Color){0,0,0,0},BLACK,(Color){0,0,0,0});
    }
}

bool vector_in_editor_rect(editorRect rect, Vector2 offset, Vector2 coord){
    Rectangle pix_rect = editor_rect_to_ray_rect(rect);
    pix_rect.x += offset.x;
    pix_rect.y += offset.y;
    if (pix_rect.width > 0){
        if (coord.x < pix_rect.x)return false;
        if ((coord.x - pix_rect.x) > pix_rect.width)return false;
    } else {
        if (coord.x > pix_rect.x)return false;
        if ((coord.x - pix_rect.x) < pix_rect.width)return false;
    }
    if (pix_rect.height > 0){
        if (coord.y < pix_rect.y)return false;
        if ((coord.y - pix_rect.y) > pix_rect.height)return false;
    } else {
        if (coord.y < pix_rect.y)return false;
        if ((coord.y - pix_rect.y) < pix_rect.height)return false;
    }
    return true;
}