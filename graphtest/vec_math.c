#include "vec_math.h"
#include <math.h>
#include "stdio.h"
void mat_identity(mat4x4 m){
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m[i][j] = (i == j)? 1.f:0.f;
}

void mat_zero(mat4x4 m){
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m[i][j] = 0.f;
}
void mat_copy(mat4x4 to,const mat4x4 from)
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            to[i][j] = from[i][j];

}
void mat_print(const mat4x4 m)
{
    for(int i = 0; i < 4; i++)
    {
        printf("||%f %f %f %f||\n",m[i][0],m[i][1],m[i][2],m[i][3]);

    }
    printf("\n");
}
void mat_translate(mat4x4 m, const vec3* v){
    mat_identity(m);
    m[0][3] = v->x;
    m[1][3] = v->y;
    m[2][3] = v->z;
}
void mat_shift(mat4x4 m, const vec4* v){
    m[0][3] += v->x;
    m[1][3] += v->y;
    m[2][3] += v->z;
}
void mat_rotate_x(mat4x4 m, float x)
{
   float c=(float)cos(x);
   float s=(float)sin(x);
   mat4x4 m1={{1,0,0,0},{0,c,-s,0},{0,s,c,0},{0,0,0,1}};
   mat_copy(m,m1);

}
void mat_rotate_y(mat4x4 m, float x)
{
   float c=(float)cos(x);
   float s=(float)sin(x);
   mat4x4 m1={{c,0,s,0},{0,1,0,0},{-s,0,c,0},{0,0,0,1}};
   mat_copy(m,m1);
}

void mat_rotate_z(mat4x4 m, float x)
{
   float c=(float)cos(x);
   float s=(float)sin(x);
   mat4x4 m1={{c,-s,0,0},{s,c,0,0},{-0,0,1,0},{0,0,0,1}};
   mat_copy(m,m1);
}
void mat_rotate(mat4x4 m, float x, float y, float z){
   mat4x4 tmp;
   mat_identity(m);
   mat_rotate_x(tmp,x);
   mat_amul(m,tmp);
   mat_rotate_y(tmp,y);
   mat_amul(m,tmp);
   mat_rotate_z(tmp,z);
   mat_amul(m,tmp);

}
void mat_rotate_quaternion(mat4x4 m,vec4 q)
{
    mat4x4 m1=
   {{1-2*q.y*q.y-2*q.z*q.z, 2*q.x*q.y-2*q.z*q.w  , 2*q.x*q.z+2*q.y*q.w,  0},
    {  2*q.x*q.y+2*q.z*q.w, 1-2*q.x*q.x-2*q.z*q.z, 2*q.y*q.z-2*q.x*q.w,  0},
    {  2*q.x*q.z-2*q.y*q.w, 2*q.y*q.z+2*q.x*q.w  , 1-2*q.x*q.x-2*q.y*q.y,0},
    {                    0,                     0,                     0,1}};
    mat_copy(m,m1);

}
void mat_transpose(mat4x4 m)
{
    mat4x4 m1;
    mat_copy(m1,m);
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m[i][j] = m1[j][i];

}
void mat_viewport(mat4x4 m, float w, float h){ //натянуть плоскость проекции на экран
   mat4x4 m1={
   {w/2.0,0,0,w/2.0},
   {0,-h/2.0,0,h/2.0},
   {0,0,1,0},
   {0,0,0,1}
   };
   mat_copy(m,m1);
}

void mat_lookat(mat4x4 m, const vec3 *eye, const vec3 *center, const vec3 *up){
    // ... переход к системе координат камеры
    vec3 x,y,z;
    y=vec3_norm(*up);
    z=*eye;
    vec3_sub(&z,center);
    z=vec3_norm(z);
    vec3_cross(&x,&y,&z);
    x=vec3_norm(x);
    mat_basis(m,&x,&y,&z);
    mat_transpose(m);
    mat4x4 tmpm;
    vec3 mcenter;
    mcenter.x=-center->x;
    mcenter.y=-center->y;
    mcenter.z=-center->z;
    mat_translate(tmpm,&mcenter);
    mat_amul(m,tmpm);
}

void mat_mmul(mat4x4 res,  const mat4x4 m1, const mat4x4 m2){
    for (int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            res[i][j] = 0.f;
            for(int k = 0; k < 4; k++){
                res[i][j] += m1[i][k]*m2[k][j];
            }
        }
    }
}
void mat_amul(mat4x4 m1,  const mat4x4 m2){
    mat4x4 tmp;
    mat_copy(tmp,m1);
    for (int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            m1[i][j] = 0.f;
            for(int k = 0; k < 4; k++){
                m1[i][j] += tmp[i][k]*m2[k][j];
            }
        }
    }
}
void mat_cmul(float coef, mat4x4 m){
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m[i][j] *= coef;
}

void mat_vmul(vec4 *res, const mat4x4 m, const vec4 *v){
    res->x = m[0][0]*v->x + m[0][1]*v->y + m[0][2]*v->z + m[0][3]*v->w;
    res->y = m[1][0]*v->x + m[1][1]*v->y + m[1][2]*v->z + m[1][3]*v->w;
    res->z = m[2][0]*v->x + m[2][1]*v->y + m[2][2]*v->z + m[2][3]*v->w;
    res->w = m[3][0]*v->x + m[3][1]*v->y + m[3][2]*v->z + m[3][3]*v->w;
}
void mat_basis(mat4x4 m,const vec3 *f1,const vec3 *f2,const vec3 *f3)
{
    vec3 tmp[4];
    tmp[0]=*f1;
    tmp[1]=*f2;
    tmp[2]=*f3;
    tmp[3].x=0;
    tmp[3].y=0;
    tmp[3].z=0;
    int i;
    for (i=0;i<4;i++)
    {
        m[0][i]=tmp[i].x;
        m[1][i]=tmp[i].y;
        m[2][i]=tmp[i].z;
        m[3][i]=0;
    }
    m[3][3]=1;
}
void mat_reverse(mat4x4 res,const mat4x4 m)
{
    /*в случае, если m может быть представлена в виде блочной матрицы
    |U a| то обратной к ней будет |U_t (U_t*(-a))|
    |0 1|                         |0       1     |
    где U - ортогональная матрица матрица, U_t - она же транстпонированная
    a - произвольный вектор
    */
    mat_copy(res,m);
    vec4 ma=mvec4(-m[0][3],-m[1][3],-m[2][3],0.0);
    mat_shift(res,&ma);
    mat_transpose(res);
    vec4 b;
    mat_vmul(&b,res,&ma);
    mat_shift(res,&b);
}
void vec4_print(vec4 *v)
{
    printf("(%f %f %f %f)\n",v->x,v->y,v->z,v->w);
}
void vec3_print(vec3 *v)
{
    printf("(%f %f %f)\n",v->x,v->y,v->z);
}
void vec2_print(vec2 *v)
{
    printf("(%f %f)\n",v->x,v->y);
}
void vec4_add(vec4 *v1, const vec4 *v2){
    v1->x += v2->x;
    v1->y += v2->y;
    v1->z += v2->z;
    v1->w += v2->w;
}

void vec4_sub(vec4 *v1, const vec4 *v2){
    v1->x -= v2->x;
    v1->y -= v2->y;
    v1->z -= v2->z;
    v1->w -= v2->w;
}
void vec4_cmul(float coef, vec4 *v){
    v->x *= coef;
    v->y *= coef;
    v->z *= coef;
    v->w *= coef;
}
float vec3_scalar(const vec3 *v1,const vec3 *v2)
{
    return(v1->x*v2->x+v1->y*v2->y+v1->z*v2->z);
}
void vec3_add(vec3 *v1, const vec3 *v2){
    v1->x += v2->x;
    v1->y += v2->y;
    v1->z += v2->z;
}
void vec3_sub(vec3 *v1, const vec3 *v2){
    v1->x -= v2->x;
    v1->y -= v2->y;
    v1->z -= v2->z;
}
void vec3_cmul(float coef, vec3 *v){
    v->x *= coef;
    v->y *= coef;
    v->z *= coef;
}

void vec3_cross(vec3 *res, const vec3 *a, const vec3 *b){
    res->x=(a->y)*(b->z)-(a->z)*(b->y);
    res->y=(a->x)*(b->z)-(a->z)*(b->x);
    res->z=(a->x)*(b->y)-(a->y)*(b->x);
}

void vec4_proj(vec3 *pr, const vec4 *v){
    pr->x = v->x/v->w;
    pr->y = v->y/v->w;
    pr->z = v->z/v->w;
}
float vec3_len(const vec3* v)
{
    return(sqrt((v->x)*(v->x)+(v->y)*(v->y)+(v->z)*(v->z)));
}
float vec4_len(const vec4* v)
{
    return(sqrt((v->x)*(v->x)+(v->y)*(v->y)+(v->z)*(v->z)+(v->w)*(v->w)));
}
vec3 vec43_cvt(vec4 v)
{
    vec3 vv;
    vv.x=v.x;
    vv.y=v.y;
    vv.z=v.z;
    return(vv);
}
vec4 vec34_cvt(vec3 v,float w)
{
    vec4 vv;
    vv.x=v.x;
    vv.y=v.y;
    vv.z=v.z;
    vv.w=w;
    return(vv);
}
vec4 vec4_norm(vec4 v)
{
    float l=vec4_len(&v);
    vec4 vv;
    vv.x=v.x/l;
    vv.y=v.y/l;
    vv.z=v.z/l;
    vv.w=v.w/l;
    return(vv);
}
vec3 vec3_norm(vec3 v)
{
    float l=vec3_len(&v);
    vec3 vv;
    if (l<0.0001)
    {
        l=1;
        printf("trying to normalize zero vector\n");
    }
    vv.x=v.x/l;
    vv.y=v.y/l;
    vv.z=v.z/l;
    return(vv);
}
vec2 mvec2(float x,float y)
{
  vec2 t;
  t.x=x;
  t.y=y;
  return(t);
}
vec3 mvec3(float x,float y,float z)
{
  vec3 t;
  t.x=x;
  t.y=y;
  t.z=z;
  return(t);
}
vec4 mvec4(float x,float y,float z,float w)
{
  vec4 t;
  t.x=x;
  t.y=y;
  t.z=z;
  t.w=w;
  return(t);
}
