/*! \file vec3.h
 *  \brief vector3
 *
 */

#ifndef _HQ_VEC3_H_
#define _HQ_VEC3_H_

typedef struct vec3 {
    float x, y, z;
} vec3;

static inline vec3 v3(float x, float y, float z) {
    vec3 out;
    out.x = x;
    out.y = y;
    out.z = z;
    return out;
}

#define _vec_op(op, out, l, r)                                                 \
    vec3 out;                                                                  \
    out.x = l.x op r.x;                                                        \
    out.y = l.y op r.y;                                                        \
    out.z = l.z op r.z;                                                        \
    return out
#define _vec_op_f(op, out, l, f)                                               \
    vec3 out;                                                                  \
    out.x = l.x op f;                                                          \
    out.y = l.y op f;                                                          \
    out.z = l.z op f;                                                          \
    return out
#define _vec_set(out, l)                                                       \
    vec3 out;                                                                  \
    out.x = l;                                                                 \
    out.y = l;                                                                 \
    out.z = l;                                                                 \
    return out
#define _vec_set_vec(out, l)                                                   \
    vec3 out;                                                                  \
    out.x = l.x;                                                               \
    out.y = l.y;                                                               \
    out.z = l.z;                                                               \
    return out
#define _vec_dot(out, l, r)                                                    \
    float out = l.x * r.x + l.y * r.y + l.z * r.z;                             \
    return out

#define vec_t vec3
#define v v3

#include "vec_arith.h"

#undef vec_t
#undef v
#undef _vec_op
#undef _vec_op_f
#undef _vec_set
#undef _vec_dot
#undef _vec_set_vec

#endif