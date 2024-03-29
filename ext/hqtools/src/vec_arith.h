/*! \file vec_arith.h
 *  \brief vector macros
 *
 */

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CONCATIFY(x, y) x##y
#define CONCAT(x, y) CONCATIFY(x, y)

#define vzero CONCAT(v, zero)
#define vone CONCAT(v, one)
#define vlen2 CONCAT(v, len2)
#define vlen CONCAT(v, len)
#define vneg CONCAT(v, neg)
#define vnorm CONCAT(v, norm)
#define vvdist CONCAT(v, CONCAT(v, dist))
#define vvdist2 CONCAT(v, CONCAT(v, dist2))
#define vvadd CONCAT(v, CONCAT(v, add))
#define vfadd CONCAT(v, CONCAT(f, add))
#define vvsub CONCAT(v, CONCAT(v, sub))
#define vfsub CONCAT(v, CONCAT(f, sub))
#define vvmul CONCAT(v, CONCAT(v, mul))
#define vfmul CONCAT(v, CONCAT(f, mul))
#define vvdiv CONCAT(v, CONCAT(v, div))
#define vfdiv CONCAT(v, CONCAT(f, div))

#ifdef vec_t

#ifdef _vec_set

static inline vec_t vzero(void) { _vec_set(out, 0); }
static inline vec_t vone(void) { _vec_set(out, 1); }

#endif

#ifdef _vec_set_vec

static inline vec_t vneg(vec_t l) { _vec_set_vec(out, -l); }

#endif

#ifdef _vec_op

static inline vec_t vvadd(vec_t l, vec_t r) { _vec_op(+, out, l, r); }
static inline vec_t vvsub(vec_t l, vec_t r) { _vec_op(-, out, l, r); }
static inline vec_t vvmul(vec_t l, vec_t r) { _vec_op(*, out, l, r); }
static inline vec_t vvdiv(vec_t l, vec_t r) { _vec_op(/, out, l, r); }

#endif

#ifdef _vec_op_f

static inline vec_t vfadd(vec_t l, float r) { _vec_op_f(+, out, l, r); }
static inline vec_t vfsub(vec_t l, float r) { _vec_op_f(-, out, l, r); }
static inline vec_t vfmul(vec_t l, float r) { _vec_op_f(*, out, l, r); }
static inline vec_t vfdiv(vec_t l, float r) { _vec_op_f(*, out, l, r); }

#endif

#ifdef _vec_dot

#include <math.h>

static inline float vlen2(vec_t l) { _vec_dot(out, l, l); }
static inline float vlen(vec_t l) { return sqrtf(vlen2(l)); }

#ifdef _vec_op

static inline float vvdist(vec_t l, vec_t r) { return vlen(vvsub(l, r)); }
static inline float vvdist2(vec_t l, vec_t r) { return vlen2(vvsub(l, r)); }
static inline vec_t vnorm(vec_t l) { return vfdiv(l, vlen(l)); }

#endif

#endif

#endif

#undef vvadd
#undef vfadd
#undef vvsub
#undef vfsub
#undef vvmul
#undef vfmul
#undef vvdiv
#undef vfdiv
#undef vlen2
#undef vlen
#undef vneg
#undef vnorm
#undef vvdist
#undef vvdist2