#include "rasterization.h"
#include "scene_worker.h"
#include "shaders.h"
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
uint32_t(*main_shader)(void *,vec3, vec3, vec3,uint32_t)=Phong_shading;
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



static uint32_t get_pixel(const SDL_Surface *s, vec2 v){
    int y = max(0,min((int)(v.y * s->h), s->h-1));
    int x = max(0,min((int)(v.x * s->w), s->w-1));
    int indx = y*s->w + x;
    //if(indx > 0) // Костыль! Нужно разобраться, откуда вообще берутся отрицательные координаты
        return ((uint32_t*)s->pixels)[indx];
    //return ((uint32_t*)s->pixels)[0];
}

void put_pixel(render_context *rc,vec3 position,vec3 bc_real,uint32_t (*shader)(void *,vec3, vec3, vec3,uint32_t))
{
    /*
    Здесь происходит расчет текстурной координаты и нормали в каждой точке с помощью
    барицентрических координат и вызов фрагментных шейдеров (пока - только тонировка Гуро)
    position - вектор, где x, y указывают на положение отрисовываемой точки на экране
    z - ее координата в системе координат камеры (нужна для z-буфера)
    */
    triangle_data *td=&rc->cur_triangle;
    vec3 texs_x=mvec3(td->tex[0].x,td->tex[1].x,td->tex[2].x);
    vec3 texs_y=mvec3(td->tex[0].y,td->tex[1].y,td->tex[2].y);
    //vec3_print(&texs_x);
    //vec3_print(&texs_y);
    vec3 norms_x=mvec3(td->normales[0].x,td->normales[1].x,td->normales[2].x);
    vec3 norms_y=mvec3(td->normales[0].y,td->normales[1].y,td->normales[2].y);
    vec3 norms_z=mvec3(td->normales[0].z,td->normales[1].z,td->normales[2].z);
    float nx=vec3_scalar(&norms_x,&bc_real);
    float ny=vec3_scalar(&norms_y,&bc_real);
    float nz=vec3_scalar(&norms_z,&bc_real);

    vec2 texture_vector=mvec2(vec3_scalar(&texs_x,&bc_real),vec3_scalar(&texs_y,&bc_real));
    //vec3_print(&texs_x);
    //vec3_print(&bc_real);
    //vec2_print(&texture_vector);
    vec3 normal=mvec3(nx,ny,nz);
    int a=(int)position.x,b=(int)(position.y);
    int indx = b*rc->screen->w + a;
        if (rc->zbuf[indx] < position.z)
        {

            rc->zbuf[indx] = position.z;
            uint32_t raw=get_pixel(rc->texture, texture_vector);
        ((uint32_t*)rc->screen->pixels)[indx] = (*shader)(rc,position,normal,bc_real,raw);


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
void swap_floats(float *a, float *b)
{
    float c;
    c = *a;
    *a = *b;
    *b = c;
}

void draw_line(render_context *rc, triangle_data *td, vec3 a, vec3 b)
{
    bool steep = false;
    if (absf(b.x - a.x) < absf(b.y - a.y))
    {
        swap_floats(&a.x, &a.y);
        swap_floats(&b.x, &b.y);
        steep = true;
    }
    if (a.x > b.x)
    {
        swap_floats(&a.x, &b.x);
        swap_floats(&a.y, &b.y);
    }
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float derrory = absf(dy/dx);
    float errory = 0;
    int y = a.y;
    for (int x=a.x; x<=b.x; x++)
    {
        vec3 verts_z = mvec3(td->vert[0].z + 1,td->vert[1].z + 1,td->vert[2].z + 1);
        if (steep)
        {
            vec3 bc_cord = barycentric(td, mvec3(y, x, 1));
            bc_cord = vec3_norm(bc_cord);
            put_pixel(rc, mvec3(y, x, vec3_scalar(&verts_z, &bc_cord)), bc_cord,rc->shader);
        }
        else
        {
            vec3 bc_cord = barycentric(td, mvec3(x, y, 1));
            bc_cord = vec3_norm(bc_cord);
            put_pixel(rc, mvec3(x, y, vec3_scalar(&verts_z, &bc_cord)), bc_cord,rc->shader);
        }

        errory += derrory;
        if (errory > 0.5)
        {
            y += (b.y > a.y?1:-1);
            errory -= 1;
        }
    }
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

    //back-face culling. Расчитываем нормаль к треугольнику и смотрим на знак
    //скалярного произведения между ней и направлением из камеры к точке треуг.

    vec3 cam=rc->camera[0];
    vec3 a,b,norm;
        a=rc->cur_triangle.vert[0];
        b=rc->cur_triangle.vert[0];
        vec3_sub(&a,&(rc->cur_triangle.vert[1]));
        vec3_sub(&b,&(rc->cur_triangle.vert[2]));
        vec3_cross(&norm,&a,&b);
        norm=vec3_norm(norm);
    vec3_sub(&cam,&rc->cur_triangle.vert[0]);
    cam=vec3_norm(cam);
    float sc=vec3_scalar(&cam,&norm);
    if (sc<-0.05) return(true);

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

    //draw_line(rc,td, td->vert[0], td->vert[1]);
    //draw_line(rc,td, td->vert[0], td->vert[2]);
    //draw_line(rc,td, td->vert[1], td->vert[2]);

    float kaef=2.5;//костыль...костыль
    float size=fmax(bboxmax.x-bboxmin.x,bboxmax.y-bboxmin.y);
    kaef/=size;
    kaef=fmax(0.03,kaef);
    vec3 P,position;
    P.z=0;
    vec2 tex;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            vec3 bc_screen  = barycentric(td, P);
            //барицентрические координаты относительно треугольника с экранными координатами
            //они не могут использоваться для интерполяции текcтуры и z-координаты
            //vec3_print(&bc_screen);
            if (bc_screen.x<-kaef || bc_screen.y<-kaef|| bc_screen.z<-kaef) continue;
            else
            {
                vec3 bc_real;//а вот сейчас мы рассчитаем барицентрицеские координаты относительно
                //уже треугольника в системе координат камеры. Это необходимо, т.к. преобразование
                //проекции из с.к. камеры на экран НЕЛИНЕЙНОЕ
                float r=-1/ck;
                vec3 tv=mvec3(r*rc->cur_triangle.vert[0].z+1.0,r*rc->cur_triangle.vert[1].z+1.0,r*rc->cur_triangle.vert[2].z+1.0);

                float cf=1/vec3_scalar(&tv,&bc_screen);
                bc_real=mvec3(bc_screen.x/tv.x,bc_screen.y/tv.y,bc_screen.z/tv.z);
                vec3_cmul(cf,&bc_real);
                float k=bc_real.x+bc_real.y+bc_real.z;
                vec3_cmul(1/k,&bc_real);
                //if (k<=0)
                {
                    bc_real=bc_screen;
                    //bc_real=mvec3(0.000,1.0,0.0);
                }

                vec3 verts_z=mvec3(td->vert[0].z,td->vert[1].z,td->vert[2].z);

                     position=mvec3(P.x,P.y,vec3_scalar(&verts_z,&bc_real));
                     put_pixel(rc,position,bc_real,rc->shader);
            }

        }
    }
    return(false);
}
bool draw_tex_triangle(render_context *rc, triangle_data *td){
    if(is_degenerate(td)) return true;
        return draw_suspicious_tex_triangle(rc, td);
}
bool init_context(render_context *rc, SDL_Surface *window){
    rc->texture = NULL;
    rc->screen=window;
    rc->zbuf = calloc(window->w * window->h, sizeof(float));
        rc->shadow_buf=NULL;
    if(rc->zbuf == NULL) return false;
    return true;
}
void attach_texture(render_context *rc, SDL_Surface *tex){
    rc->texture = tex;
}
/*void calculate_shadows(render_context *rc,scene *s,vec3 camera[],float *shadow_buf)
{
    uint32_t(*tmp_shader)(void *,vec3, vec3, vec3,uint32_t)=main_shader;
    main_shader=zbuf_shading;
    free(shadow_buf);
    int sz=rc->screen->w * rc->screen->h;
    shadow_buf=calloc(sz*s->point_light_count, sizeof(float));
    //Теневой буфер.
    //Занимает до чЕрта места при большом количестве иcточников света
    for (int i=0;i<s->point_light_count;i++)
    {
        draw_scene(rc,s,camera);
        for (int j=0;j<sz;j++) shadow_buf[i*sz+j]=rc->zbuf[j];
    }
    main_shader=tmp_shader;
}*/
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
for (int k=0;k<s->mesh_count+s->point_light_count;k++)
{
    mash *m;
    if (k<s->mesh_count)
    {
        m=s->meshes[k];
    }
    else
    {
        m=s->point_lights[k-s->mesh_count].model;
    }
    rc->texture=m->texture;
    if (m->no_light)
    {
        rc->shader=no_light;
    }
    else
    {
        rc->shader=main_shader;
    }
    triangle_data td;
    for(int i = 0; i < m->f_count; i++){

        td.color = 0;
        td.suspicious=true;
        vec3 vert[3];
        for(int j = 0; j < 3; j++){
            vec4 temp,raw,raw1,raw2;
            vert[j]=s->prepared_verts[k][m->polys[i].vs[j] - 1];
            //j-ая вершина треугольника в глобальной с.к.
            raw=vec34_cvt(vert[j],1.0);
            mat_vmul(&temp,transform,&raw);//переход в с.к. камеры
            float kk=ck/(ck-temp.z);
            td.vert[j].x = (temp.x*kk)*rc->screen->h/2.f + rc->screen->w/2.f;
            td.vert[j].y = -(temp.y*kk)*rc->screen->h/2.f + rc->screen->h/2.f;
            td.vert[j].z = temp.z;//переход в экранную с.к.

            td.tex[j].x = m->vts[m->polys[i].vts[j] - 1].x;//загркжаем текстуру
            td.tex[j].y = m->vts[m->polys[i].vts[j] - 1].y;
            //printf("%d_",m->polys[i].vts[j] - 1);
            //vec2_print(&td.tex[0]);
            //загружаем нормали
            raw2=vec34_cvt(m->vns[m->polys[i].vns[j]-1],0.0);
            mat_vmul(&raw1,m->position,&raw2);
            td.normales[j]=vec43_cvt(raw1);
            td.normales[j]=vec3_norm(td.normales[j]);
            float x=td.vert[j].x;//проверка на подозрительность
            float y=td.vert[j].y;
            if ((x>=(rc->screen->w))||(x<=0)||(y>=(rc->screen->h))||(y<=0))
            {
                td.suspicious=true;
            }
        //vec2_print(&td.tex[0]);
        }
        //vec2_print(&td.tex[0]);
        rc->cur_triangle=td;

        rc->cur_triangle.vert[0]=vert[0];
        rc->cur_triangle.vert[1]=vert[1];
        rc->cur_triangle.vert[2]=vert[2];

        draw_tex_triangle(rc,&td);
    }
}
return true;
}
void free_context(render_context *rc){
    free(rc->zbuf);
}
