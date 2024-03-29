/*! \file vec2.h
 *  \brief vector2
 *
 */

#ifndef _HQ_VEC2_H_
#define _HQ_VEC2_H_

typedef struct vec2 {
    float x, y;
} vec2;

static inline vec2 v2(float x, float y) {
    vec2 out;
    out.x = x;
    out.y = y;
    return out;
}

#define _vec_op(op, out, l, r)                                                 \
    vec2 out;                                                                  \
    out.x = l.x op r.x;                                                        \
    out.y = l.y op r.y;                                                        \
    return out
#define _vec_op_f(op, out, l, f)                                               \
    vec2 out;                                                                  \
    out.x = l.x op f;                                                          \
    out.y = l.y op f;                                                          \
    return out
#define _vec_set(out, l)                                                       \
    vec2 out;                                                                  \
    out.x = l;                                                                 \
    out.y = l;                                                                 \
    return out
#define _vec_set_vec(out, l)                                                   \
    vec2 out;                                                                  \
    out.x = l.x;                                                               \
    out.y = l.y;                                                               \
    return out
#define _vec_dot(out, l, r)                                                    \
    float out = l.x * r.x + l.y * r.y;                                         \
    return out

#define vec_t vec2
#define v v2

#include "vec_arith.h"

#undef vec_t
#undef v
#undef _vec_op
#undef _vec_op_f
#undef _vec_set
#undef _vec_dot
#undef _vec_set_vec

#endif