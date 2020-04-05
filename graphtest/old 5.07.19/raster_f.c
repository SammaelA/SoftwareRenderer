#include "rasterization.h"
#include "scene_worker.h"
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
float ck=1.0;
    //расстояние от камеры до плоскости z=0 в системе координат камеры
    //расчитывается один раз для каждой отрисовки сцены и используется в
    //дальнейшем при отрисовкe suspicious треугольника
static inline void swap(vec3 *p1, vec3 *p2){
    vec3 p3 = *p1;
    *p1 = *p2;
    *p2 = p3;
}

static inline void swap2(vec2 *p1, vec2 *p2){
    vec2 p3 = *p1;
    *p1 = *p2;
    *p2 = p3;
}

static inline int min(int x, int y) {return (x<=y)?x:y;}
static inline int max(int x, int y) {return (x>=y)?x:y;}

static inline void sort_vs(triangle_data *td){
    if(td->vert[0].y > td->vert[1].y){
        swap(&td->vert[0], &td->vert[1]);
        swap2(&td->tex[0], &td->tex[1]);
    }
    if(td->vert[1].y > td->vert[2].y) {
        swap(&td->vert[1], &td->vert[2]);
        swap2(&td->tex[1], &td->tex[2]);
    }
    if(td->vert[0].y > td->vert[1].y) {
        swap(&td->vert[0], &td->vert[1]);
        swap2(&td->tex[0], &td->tex[1]);
    }
}

static inline float absf(float v) { return (v > 0)? v:(-v); }

static bool is_degenerate(triangle_data *td){
    if(absf(td->vert[0].x - td->vert[1].x) < 0.5f && absf(td->vert[1].x - td->vert[2].x) < 0.5f) return true;
    if(absf(td->vert[0].y - td->vert[1].y) < 0.5f && absf(td->vert[1].y - td->vert[2].y) < 0.5f) return true;
    return false;
}

static inline vec3 mid_vertex(const vec3 *v0, const vec3 *v1, float y){
    vec3  res;
    res.y = y;
    res.x = (y - v0->y)*(v1->x - v0->x)/(v1->y - v0->y) + v0->x;
    res.z = (y - v0->y)*(v1->z - v0->z)/(v1->y - v0->y) + v0->z;
    return res;
}


void draw_triangle(SDL_Surface *surf, triangle_data *td){
    sort_vs(td);
    if(td->vert[0].y == td->vert[1].y && td->vert[1].y == td->vert[2].y) return;
    if(td->vert[0].y != td->vert[1].y){
        vec3 temp = mid_vertex(&td->vert[0], &td->vert[2], td->vert[1].y);
        swap(&temp, &td->vert[2]);
        float x1 = td->vert[0].x, x2 = x1;
        float c1 = (td->vert[1].x - x1)/(td->vert[1].y - td->vert[0].y),
             c2 = (td->vert[2].x - x1)/(td->vert[2].y - td->vert[0].y);
        if(td->vert[1].x > td->vert[2].x){
            float temp = c1;
            c1 = c2;
            c2 = temp;
        }
        for(int y = td->vert[0].y; y <= td->vert[1].y; y++){
            for(int x = x1; x <= x2; x++){
                ((uint32_t*)surf->pixels)[y*surf->w + x] = td->color;
            }
            x1 += c1;
            x2 += c2;
        }
        td->vert[0] = td->vert[2];
        td->vert[2] = temp;

    }
    if(td->vert[0].y != td->vert[2].y) {
        float x1 = td->vert[0].x, x2 = td->vert[1].x;
        float c1 = (td->vert[2].x - x1)/(td->vert[2].y - td->vert[0].y),
            c2 = (td->vert[2].x - x2)/(td->vert[2].y - td->vert[1].y);
        if(td->vert[0].x > td->vert[1].x){
            float temp = c1;
            c1 = c2;
            c2 = temp;
        }
        for(int y = td->vert[0].y; y <= td->vert[2].y; y++){
            for(int x = x1; x <= x2; x++){
                ((uint32_t*)surf->pixels)[y*surf->w + x] = td->color;
            }
            x1 += c1;
            x2 += c2;
        }
    }
}

static uint32_t get_pixel(const SDL_Surface *s, vec2 v){
    int y = min((int)(v.y * s->h), s->h-1);
    int x = min((int)(v.x * s->w), s->w-1);
    int indx = y*s->w + x;
    if(indx > 0) // Костыль! Нужно разобраться, откуда вообще берутся отрицательные координаты
        return ((uint32_t*)s->pixels)[indx];
    return ((uint32_t*)s->pixels)[0];
}
void put_pixel(render_context *rc,vec3 position,vec2 texture_vector,vec3 normal)
{
    int a=(int)position.x,b=(int)(position.y);
    int indx = b*rc->screen->w + a;
            if (rc->zbuf[indx] < position.z){
                float light_k;
            uint32_t raw=get_pixel(rc->texture, texture_vector);
            Uint8 r,g,b;
            vec3 cr;
            vec3 l=rc->s->point_lights[0].pos;
            vec3_cmul(-1.0,&l);
            l=vec3_norm(l);
            vec3_cross(&cr,&normal,&l);
            light_k=vec3_len(&cr);
            if (light_k<0) light_k=-light_k;
            if (light_k>1) light_k=1.0;
            light_k=1.0-light_k;
            Uint8 t=(Uint8)(light_k*255);
            uint32_t res=SDL_MapRGB(rc->screen->format,t,t,t);
                ((uint32_t*)rc->screen->pixels)[indx] = res;
                rc->zbuf[indx] = position.z;

            }

}
void draw_tex_top(render_context *rc, triangle_data *td){


    float x1 = td->vert[0].x, x2 = x1, c1, c2;
    vec2 tex_row = td->tex[0], line_step, side_step;
    float z_row = td->vert[0].z, zside_step, zline_step;
    int min_bound = min(td->vert[0].x, min(td->vert[1].x, td->vert[2].x));
    int max_bound = max(td->vert[0].x, max(td->vert[1].x, td->vert[2].x));
    if(td->vert[1].x < td->vert[2].x){
        c1 = (td->vert[1].x - td->vert[0].x)/(td->vert[1].y - td->vert[0].y);
        c2 = (td->vert[2].x - td->vert[0].x)/(td->vert[2].y - td->vert[0].y);
        side_step.x = (td->tex[1].x - td->tex[0].x)/(td->vert[1].y - td->vert[0].y);
        side_step.y = (td->tex[1].y - td->tex[0].y)/(td->vert[1].y - td->vert[0].y);
        line_step.x = (td->tex[2].x - td->tex[1].x)/(td->vert[2].x - td->vert[1].x);
        line_step.y = (td->tex[2].y - td->tex[1].y)/(td->vert[2].x - td->vert[1].x);
        zside_step = (td->vert[1].z - td->vert[0].z)/(td->vert[1].y - td->vert[0].y);
        zline_step = (td->vert[2].z - td->vert[1].z)/(td->vert[2].x - td->vert[1].x);
    } else {
        c1 = (td->vert[2].x - td->vert[0].x)/(td->vert[2].y - td->vert[0].y);
        c2 = (td->vert[1].x - td->vert[0].x)/(td->vert[1].y - td->vert[0].y);
        side_step.x = (td->tex[2].x - td->tex[0].x)/(td->vert[2].y - td->vert[0].y);
        side_step.y = (td->tex[2].y - td->tex[0].y)/(td->vert[2].y - td->vert[0].y);
        line_step.x = (td->tex[1].x - td->tex[2].x)/(td->vert[1].x - td->vert[2].x);
        line_step.y = (td->tex[1].y - td->tex[2].y)/(td->vert[1].x - td->vert[2].x);
        zside_step = (td->vert[2].z - td->vert[0].z)/(td->vert[2].y - td->vert[0].y);
        zline_step = (td->vert[1].z - td->vert[2].z)/(td->vert[1].x - td->vert[2].x);

    }
    //printf("%f %f\n", zside_step, zline_step);
    for(int y = td->vert[0].y; y <= td->vert[2].y; y++){
        vec2 tex = tex_row;
        float z = z_row;
        for(int x = max(x1, min_bound); x <= min(x2, max_bound); x++){//Костыль
            //Почему x1 и x2 выходят за границы треугольника и где?
            put_pixel(rc,mvec3(x,y,z),tex,td->normales[0]);
            tex.x += line_step.x;
            tex.y += line_step.y;
            z += zline_step;
        }
        z_row += zside_step;
        x1 += c1;
        x2 += c2;
        tex_row.x += side_step.x;
        tex_row.y += side_step.y;
    }
}

void draw_tex_bot(render_context *rc, triangle_data *td){
    float x1, x2, c1, c2;
    vec2 tex_row, line_step, side_step;
    float z_row, zside_step, zline_step;
    int min_bound = min(td->vert[0].x, min(td->vert[1].x, td->vert[2].x));
    int max_bound = max(td->vert[0].x, max(td->vert[1].x, td->vert[2].x));

    if(td->vert[0].x < td->vert[1].x){
        x1 = td->vert[0].x;
        x2 = td->vert[1].x;
        tex_row = td->tex[0];
        z_row = td->vert[0].z;
        c1 = (td->vert[2].x - td->vert[0].x)/(td->vert[2].y - td->vert[0].y);
        c2 = (td->vert[2].x - td->vert[1].x)/(td->vert[2].y - td->vert[1].y);
        side_step.x = (td->tex[2].x - td->tex[0].x)/(td->vert[2].y - td->vert[0].y);
        side_step.y = (td->tex[2].y - td->tex[0].y)/(td->vert[2].y - td->vert[0].y);
        line_step.x = (td->tex[1].x - td->tex[0].x)/(td->vert[1].x - td->vert[0].x);
        line_step.y = (td->tex[1].y - td->tex[0].y)/(td->vert[1].x - td->vert[0].x);
        zside_step = (td->vert[2].z - td->vert[0].z)/(td->vert[2].y - td->vert[0].y);
        zline_step = (td->vert[1].z - td->vert[0].z)/(td->vert[1].x - td->vert[0].x);
    } else {
        x1 = td->vert[1].x;
        x2 = td->vert[0].x;
        tex_row = td->tex[1];
        z_row = td->vert[1].z;
        c1 = (td->vert[2].x - td->vert[1].x)/(td->vert[2].y - td->vert[1].y);
        c2 = (td->vert[2].x - td->vert[0].x)/(td->vert[2].y - td->vert[0].y);
        side_step.x = (td->tex[2].x - td->tex[1].x)/(td->vert[2].y - td->vert[1].y);
        side_step.y = (td->tex[2].y - td->tex[1].y)/(td->vert[2].y - td->vert[1].y);
        line_step.x = (td->tex[0].x - td->tex[1].x)/(td->vert[0].x - td->vert[1].x);
        line_step.y = (td->tex[0].y - td->tex[1].y)/(td->vert[0].x - td->vert[1].x);
        zside_step = (td->vert[2].z - td->vert[1].z)/(td->vert[2].y - td->vert[1].y);
        zline_step = (td->vert[0].z - td->vert[1].z)/(td->vert[0].x - td->vert[1].x);
    }
    for(int y = td->vert[0].y; y <= td->vert[2].y; y++){
        vec2 tex = tex_row;
        float z = z_row;
        for(int x = max(x1, min_bound); x <= min(x2, max_bound); x++){ //Аналогичный костыль
            put_pixel(rc,mvec3(x,y,z),tex,td->normales[0]);
            tex.x += line_step.x;
            tex.y += line_step.y;
            z += zline_step;
        }
        x1 += c1;
        x2 += c2;
        tex_row.x += side_step.x;
        tex_row.y += side_step.y;
        z_row += zside_step;
    }
}
float fmax(float a,float b)
{
    if (a>b) return(a);
    else return(b);
}
float fmin(float a,float b)
{
    if (a<b) return(a);
    else return(b);

}
vec3 barycentric(triangle_data *td, vec3 P) {
    //барицентрические координаты точки P в двумерной системе координат
    vec3 a,b,u;
    a=mvec3(td->vert[2].x-td->vert[0].x, td->vert[1].x-td->vert[0].x, td->vert[0].x-P.x);
    b=mvec3(td->vert[2].y-td->vert[0].y, td->vert[1].y-td->vert[0].y, td->vert[0].y-P.y);
    vec3_cross(&u,&a,&b);
    if (absf(u.z)<=1.0)
    {

        return mvec3(-1.0,1.0,1.0); // triangle is degenerate, in this case return smth with negative coordinates
    }
    a=mvec3(1.0-(u.x-u.y)/u.z, -u.y/u.z, u.x/u.z);
    return a;
}

bool draw_suspicious_tex_triangle(render_context *rc, triangle_data *td)
{
    /*метод отрисовки треугольников с помощью заключения их в прямоугольник
    и расчета барицентрических координат каждой точки. На данный момент используется
    как основной метод отрисовки треугольников, т.к. именно на его основе относительно
    легко проводить линейную интерполяцию и рассчитывать освещенность и прочие шейдеры
    методы Павла более быстрые, но я не разобрался, как на их основе проводить интерполяцию
    */
    SDL_Surface *surf=rc->screen;
    if ((td->vert[0].x<=0)&&(td->vert[1].x<=0)&&(td->vert[2].x<=0)) return(true);
    if ((td->vert[0].x>=surf->w)&&(td->vert[1].x>=surf->w)&&(td->vert[2].x>=surf->w)) return(true);
    if ((td->vert[0].y<=0)&&(td->vert[1].y<=0)&&(td->vert[2].y<=0)) return(true);
    if ((td->vert[0].y>=surf->h)&&(td->vert[1].y>=surf->h)&&(td->vert[2].y>=surf->h)) return(true);
    vec2 bboxmin=mvec2(surf->w-1,  surf->h-1);
    vec2 bboxmax=mvec2(0.0, 0.0);
    vec2 clamp=mvec2(surf->w-1,  surf->h-1);
    for (int i=0; i<3; i++)
    {
            bboxmin.x = fmax(0,        fmin(bboxmin.x, td->vert[i].x));
            bboxmax.x = fmin(clamp.x, fmax(bboxmax.x, td->vert[i].x));

            bboxmin.y = fmax(0,        fmin(bboxmin.y, td->vert[i].y));
            bboxmax.y = fmin(clamp.y, fmax(bboxmax.y, td->vert[i].y));
    }
    if ((bboxmax.x>=surf->w-2)&&(bboxmax.y>=surf->h-2)) return(true);
    vec3 P,position;
    P.z=0;
    vec2 tex;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            vec3 bc_screen  = barycentric(td, P);
            //барицентрические координаты относительно треугольника с экранными координатами
            //они не могут использоваться для интерполяции текcтуры и z-координаты
            if (bc_screen.x<-0.05 || bc_screen.y<-0.05 || bc_screen.z<-0.05) continue;
            else
            {
                vec3 bc_real;//а вот сейчас мы рассчитаем барицентрицеские координаты относительно
                //уже треугольника в системе координат камеры. Это необходимо, т.к. преобразование
                //проекции из с.к. камеры на экран НЕЛИНЕЙНОЕ
                /*float r=-1/ck;
                vec3 tv=mvec3(r*rc->vert[0].z+1.0,r*rc->vert[1].z+1.0,r*rc->vert[2].z+1.0);

                float cf=1/vec3_scalar(&tv,&bc_screen);
                bc_real=mvec3(bc_screen.x/tv.x,bc_screen.y/tv.y,bc_screen.z/tv.z);
                vec3_cmul(cf,&bc_real);*/
                bc_real=bc_screen;
                vec3 verts_z=mvec3(td->vert[0].z,td->vert[1].z,td->vert[2].z);
                vec3 texs_x=mvec3(td->tex[0].x,td->tex[1].x,td->tex[2].x);
                vec3 texs_y=mvec3(td->tex[0].y,td->tex[1].y,td->tex[2].y);
                vec3 norms_x=mvec3(td->normales[0].x,td->normales[1].x,td->normales[2].x);
                vec3 norms_y=mvec3(td->normales[0].y,td->normales[1].y,td->normales[2].y);
                vec3 norms_z=mvec3(td->normales[0].z,td->normales[1].z,td->normales[2].z);
                float nx=vec3_scalar(&norms_x,&bc_real);
                float ny=vec3_scalar(&norms_y,&bc_real);
                float nz=vec3_scalar(&norms_z,&bc_real);
                     position=mvec3(P.x,P.y,vec3_scalar(&verts_z,&bc_real));
                     tex=mvec2(vec3_scalar(&texs_x,&bc_real),vec3_scalar(&texs_y,&bc_real));

                     put_pixel(rc,position,tex,mvec3(nx,ny,nz));
            }

        }
    }
    return(false);
}
bool draw_tex_triangle(render_context *rc, triangle_data *td){
    SDL_Surface *surf=rc->screen;
    if(is_degenerate(td)) return true;
    if (td->suspicious)
    {
        return draw_suspicious_tex_triangle(rc, td);
        //return(false);
    }

    sort_vs(td);
    if(td->vert[0].y != td->vert[1].y) {
        float alpha = (td->vert[1].y - td->vert[0].y)/(td->vert[2].y - td->vert[0].y);
        vec3 v;
        vec2 t;
        v.y = td->vert[1].y;
        v.x = td->vert[0].x + alpha*(td->vert[2].x - td->vert[0].x);
        v.z = td->vert[0].z + alpha*(td->vert[2].z - td->vert[0].z);
        t.x = td->tex[0].x + alpha*(td->tex[2].x - td->tex[0].x);
        t.y = td->tex[0].y + alpha*(td->tex[2].y - td->tex[0].y);
        swap(&v, &td->vert[2]);
        swap2(&t, &td->tex[2]);
        draw_tex_top(rc, td);
        td->vert[0] = td->vert[2];
        td->tex[0] = td->tex[2];
        td->vert[2] = v;
        td->tex[2] = t;
    }
    if(td->vert[0].y == td->vert[2].y) return false;

    draw_tex_bot(rc, td);
    return false;
}
/*
bool init_context(render_context *rc, SDL_Surface *window, int max_triangles, int max_verts){
    rc->screen = window;
    rc->max_triangles = max_triangles;
    rc->texture = NULL;
    rc->triangles_count = 0;
    rc->zbuf = calloc(window->w * window->h, sizeof(float));
    if(rc->zbuf == NULL) return false;
    rc->triangles = calloc(max_triangles, sizeof(triangle_data));
    if(rc->triangles == NULL) return false;
    rc->max_verts = max_verts;
    rc->verts = calloc(max_verts, sizeof(vec4));
    if(rc->verts == NULL) return false;
    return true;
}

bool attach_mesh(render_context *rc, const mash *m, const mat4x4 transform,float dist){
    if(rc->max_verts < m->v_count) return false;
    if(rc->max_triangles < m->f_count) return false;

    rc->triangles_count = m->f_count;
    vec4 temp;//перевод в экранную с.к.
    for(int i = 0; i < m->v_count; i++){
        mat_vmul(&temp, transform, &m->vs[i]);
        float kk=dist/(dist-temp.z);
        rc->verts[i].x = (temp.x*kk)*rc->screen->h/2.f + rc->screen->w/2.f;
        rc->verts[i].y = -(temp.y*kk)*rc->screen->h/2.f + rc->screen->h/2.f;
        rc->verts[i].z = temp.z;
    }

    for(int i = 0; i < m->f_count; i++){
        rc->triangles[i].color = 0;
        rc->triangles[i].suspicious=false;
        for(int j = 0; j < 3; j++){
            rc->triangles[i].vert[j] = rc->verts[m->polys[i].vs[j] - 1];
            rc->triangles[i].tex[j].x = m->vts[m->polys[i].vts[j] - 1].x;
            rc->triangles[i].tex[j].y = m->vts[m->polys[i].vts[j] - 1].y;
            float x=rc->triangles[i].vert[j].x;
            float y=rc->triangles[i].vert[j].y;
            if ((x>=(rc->screen->w))||(x<=0)||(y>=(rc->screen->h))||(y<=0))
            {
                rc->triangles[i].suspicious=true;
            }
        }
    }
    return true;
}



void draw_all(render_context *rc){
    //printf("Drawing %d triangles\n", rc->triangles_count);
    for(int i = 0; i < rc->screen->w*rc->screen->h; i++) rc->zbuf[i] = -1e10;
    for(int i = 0; i < rc->triangles_count; i++){
        draw_tex_triangle(rc->screen, rc->texture, rc->zbuf, &rc->triangles[i]);
    }
}*/
bool init_context(render_context *rc, SDL_Surface *window){
    rc->texture = NULL;
    rc->screen=window;
    rc->zbuf = calloc(window->w * window->h, sizeof(float));
    if(rc->zbuf == NULL) return false;
    return true;
}
void attach_texture(render_context *rc, SDL_Surface *tex){
    rc->texture = tex;
}
void draw_scene(render_context *rc,scene *s,vec3 camera[])
{//рисуем все меши на сцене
    rc->s=s;
mat4x4 transform;
mat_lookat(transform, &camera[0], &camera[1], &camera[2]);
vec3 tmp=camera[0];
    vec3_sub(&tmp,&camera[1]);
    ck=vec3_len(&tmp);
//матрица перехода от глобальной с.к. к с.к. камеры
for(int i = 0; i < rc->screen->w*rc->screen->h; i++) rc->zbuf[i] = -1e10;
//очищаем z-буфер
for (int k=0;k<s->mesh_count;k++)
{
    mash *m=s->meshes[k];
    triangle_data td;
    for(int i = 0; i < m->f_count; i++){

        td.color = 0;
        td.suspicious=true;
        vec3 vert[3];
        for(int j = 0; j < 3; j++){
            vec4 temp,raw;

            vert[j]=s->prepared_verts[k][m->polys[i].vs[j] - 1];
            //j-ая вершина треугольника в глобальной с.к.
            raw=vec34_cvt(vert[j],1.0);

            mat_vmul(&temp,transform,&raw);//переход в с.к. камеры
            //vert[j]=vec43_cvt(temp);
            float kk=ck/(ck-temp.z);
            td.vert[j].x = (temp.x*kk)*rc->screen->h/2.f + rc->screen->w/2.f;
            td.vert[j].y = -(temp.y*kk)*rc->screen->h/2.f + rc->screen->h/2.f;
            td.vert[j].z = temp.z;//переход в экранную с.к.

            td.tex[j].x = m->vts[m->polys[i].vts[j] - 1].x;//загркжаем текстуру
            td.tex[j].y = m->vts[m->polys[i].vts[j] - 1].y;


            vec3 tempn=vec3_norm(m->vns[m->polys[i].vts[j] - 1]);
            mat_vmul(&td.normales[j],m->position,&tempn);//Загружвем нормали
            mat_vmul(&tempn,transform,&td.normales[j]);
            td.normales[j]=tempn;
            //vec3_print(td.normales[0]);
            //vec3_print(td.normales[1]);
            //vec3_print(td.normales[2]);
            float x=td.vert[j].x;//проверка на подозрительность
            float y=td.vert[j].y;
            if ((x>=(rc->screen->w))||(x<=0)||(y>=(rc->screen->h))||(y<=0))
            {
                td.suspicious=true;
            }
        }
        rc->vert[0]=vert[0];
        rc->vert[1]=vert[1];
        rc->vert[2]=vert[2];

        vec3 a,b,norm;
        a=vert[0];
        b=vert[0];
        vec3_sub(&a,&(vert[1]));
        vec3_sub(&b,&(vert[2]));
        vec3_cross(&norm,&a,&b);
        norm=vec3_norm(norm);
        //td.normales[0]=norm;
        //td.normales[1]=norm;
        //td.normales[2]=norm;
        draw_tex_triangle(rc,&td);
    }
}
    return true;
}
void free_context(render_context *rc){
    free(rc->zbuf);
}
