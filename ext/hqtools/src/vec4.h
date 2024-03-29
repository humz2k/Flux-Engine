/*! \file vec4.h
 *  \brief vector4
 *
 */

#ifndef _HQ_VEC4_H_
#define _HQ_VEC4_H_

typedef struct vec4 {
    float x, y, z, w;
} vec4;

static inline vec4 v4(float x, float y, float z, float w) {
    vec4 out;
    out.x = x;
    out.y = y;
    out.z = z;
    out.w = w;
    return out;
}

#define _vec_op(op, out, l, r)                                                 \
    vec4 out;                                                                  \
    out.x = l.x op r.x;                                                        \
    out.y = l.y op r.y;                                                        \
    out.z = l.z op r.z;                                                        \
    out.w = l.z op r.w;                                                        \
    return out
#define _vec_op_f(op, out, l, f)                                               \
    vec4 out;                                                                  \
    out.x = l.x op f;                                                          \
    out.y = l.y op f;                                                          \
    out.z = l.z op f;                                                          \
    out.w = l.z op f;                                                          \
    return out
#define _vec_set(out, l)                                                       \
    vec4 out;                                                                  \
    out.x = l;                                                                 \
    out.y = l;                                                                 \
    out.z = l;                                                                 \
    out.w = l;                                                                 \
    return out
#define _vec_set_vec(out, l)                                                   \
    vec4 out;                                                                  \
    out.x = l.x;                                                               \
    out.y = l.y;                                                               \
    out.z = l.z;                                                               \
    out.w = l.w;                                                               \
    return out
#define _vec_dot(out, l, r)                                                    \
    float out = l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;                 \
    return out

#define vec_t vec4
#define v v4

#include "vec_arith.h"

#undef vec_t
#undef v
#undef _vec_op
#undef _vec_op_f
#undef _vec_set
#undef _vec_dot
#undef _vec_set_vec

#endif