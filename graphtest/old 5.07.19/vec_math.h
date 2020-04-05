#ifndef VEC_MATH_H_INCLUDED
#define VEC_MATH_H_INCLUDED

typedef float (mat4x4)[4][4];

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    float x, y, z, w;
} vec4;

typedef struct {
    float x, y;
} vec2;

void mat_identity(mat4x4);
void mat_zero(mat4x4);
void mat_copy(mat4x4 to,const mat4x4 from);
void mat_print(const mat4x4 m);
void mat_translate(mat4x4, const vec3*);
void mat_rotate_x(mat4x4 m, float x);
void mat_rotate_y(mat4x4 m, float x);
void mat_rotate_z(mat4x4 m, float x);
void mat_rotate_quaternion(mat4x4 m,vec4 q);
void mat_rotate(mat4x4, float, float, float);
void mat_viewport(mat4x4, float, float);
void mat_lookat(mat4x4, const vec3*, const vec3*, const vec3*);
void mat_mmul(mat4x4 res, const mat4x4 m1, const mat4x4 m2);
void mat_amul(mat4x4 m1,  const mat4x4 m2);
void mat_cmul(float coef, mat4x4 m);
void mat_vmul(vec4 *res, const mat4x4 m, const vec4 *v);
void mat_basis(mat4x4 m,const vec3 *f1,const vec3 *f2,const vec3 *f3);
void mat_transpose(mat4x4 m);

void vec4_add(vec4 *v1, const vec4 *v2);
void vec4_sub(vec4 *v1, const vec4 *v2);
void vec4_cmul(float coef, vec4 *v);
void vec4_proj(vec3 *pr, const vec4 *v);
float vec4_len(const vec4 *v);
vec4 vec4_norm(vec4 v);

void vec3_add(vec3 *v1, const vec3 *v2);
void vec3_sub(vec3 *v1, const vec3 *v2);
void vec3_cmul(float coef, vec3 *v);
void vec3_cross(vec3 *res, const vec3 *a, const vec3 *b);
float vec3_len(const vec3 *v);
float vec3_scalar(const vec3 *v1,const vec3 *v2);
vec3 vec3_norm(vec3 v);

void vec4_proj(vec3 *pr, const vec4 *v);
vec3 vec43_cvt(vec4 v);
vec4 vec34_cvt(vec3 v,float w);
vec2 mvec2(float x,float y);
vec3 mvec3(float x,float y,float z);
vec4 mvec4(float x,float y,float z,float w);
#endif // VEC_MATH_H_INCLUDED
