#include "shaders.h"
#include "rasterization.h"
#include "scene_worker.h"
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
uint32_t no_light(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw)
{

    return(raw);
}
uint32_t Gourand_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw)
{//применение фрагментного шейдера - тонировка Гуро
            const float max_light_q=1.5;
            Uint8 r=255,g=255,b=255;
            SDL_PixelFormat *tmp_form;
            SDL_GetRGB(raw,rc->texture->format,&r,&g,&b);
            float intens,light_k,light_sum=0.0;
            vec3 cr,l;
            vec3 real_pos;//положение точки в глобальной с.к.

            //его расчет. Сложно, долго, но куда деваться..
            triangle_data *td=&rc->cur_triangle;
            vec3 vs_x=mvec3(td->vert[0].x,td->vert[1].x,td->vert[2].x);
            vec3 vs_y=mvec3(td->vert[0].y,td->vert[1].y,td->vert[2].y);
            vec3 vs_z=mvec3(td->vert[0].z,td->vert[1].z,td->vert[2].z);
            float nx=vec3_scalar(&vs_x,&bc_real);
            float ny=vec3_scalar(&vs_y,&bc_real);
            float nz=vec3_scalar(&vs_z,&bc_real);
            real_pos=mvec3(nx,ny,nz);
            for (int i=0;i<rc->s->point_light_count;i++)
            {
                if (rc->s->point_lights[i].far)
                {
                    l=rc->s->point_lights[i].pos;
                    vec3_cmul(-1.0,&l);
                    intens=1.0;
                }
                else
                {//вектор l - вектор от источника света до точки, все в глобальной с.к.
                    l=real_pos;
                    vec3_sub(&l,&rc->s->point_lights[i].pos);
                    if (rc->s->point_lights[i].fading)
                    {
                        intens=rc->s->point_lights[i].intensity/(vec3_len(&l)+0.001);
                    }

                }

                l=vec3_norm(l);
                vec3_cmul(-1.0,&l);
                vec3_cross(&cr,&normal,&l);
                light_k=vec3_scalar(&l,&normal);
                //printf("111inten %f",intens);

                //if (light_k<0) light_k=-light_k;
                if (light_k<0) light_k=0.0;
                if (light_k>1) light_k=1.0;
                //light_k=(1.0-light_k)*intens;
                light_sum+=light_k*intens;
            }

            if (light_sum>max_light_q) light_sum=max_light_q;
            int t_r,t_g,t_b;
            t_r =light_sum*r;
            if (t_r>255) t_r=255;
            else if (t_r<0) t_r=0;
            t_g =light_sum*g;
            if (t_g>255) t_g=255;
            else if (t_g<0) t_g=0;
            t_b =light_sum*b;
            if (t_b>255) t_b=255;
            else if (t_b<0) t_b=0;
            r=t_r;
            g=t_g;
            b=t_b;

            return(SDL_MapRGB(rc->screen->format,r,g,b));
}
uint32_t Phong_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw)
{
    const float ambient_q=0.4,diffuse_q=0.8,specular_q=1.0;
    //коэффициенты фонового, рассеянного освещения и бликов соответственно. Настроил от балды.

    const float specular_power=10.0;
    //уровень "глянцевости".Чем больше степень, тем более яркими и сконцентрированными будут блики
    //По хорошому, это число зависит от материала и должно доставляться текстурой, но пока этого
    //не происходит

            const float max_diffuse_q=1.1;
            const float max_specuar_q=15.0;
            Uint8 r=255,g=255,b=255;
            SDL_PixelFormat *tmp_form;
            SDL_GetRGB(raw,rc->texture->format,&r,&g,&b);
            register float intens,light_k,light_sum=0.0,specular_k,specular_sum=0.0;
            vec3 cr,l,sp,ref;
            vec3 real_pos;//положение точки в глобальной с.к.

            //его расчет. Сложно, долго, но куда деваться..
            triangle_data *td=&rc->cur_triangle;
            vec3 vs_x=mvec3(td->vert[0].x,td->vert[1].x,td->vert[2].x);
            vec3 vs_y=mvec3(td->vert[0].y,td->vert[1].y,td->vert[2].y);
            vec3 vs_z=mvec3(td->vert[0].z,td->vert[1].z,td->vert[2].z);
            register float nx=vec3_scalar(&vs_x,&bc_real);
            register float ny=vec3_scalar(&vs_y,&bc_real);
            register float nz=vec3_scalar(&vs_z,&bc_real);
            real_pos=mvec3(nx,ny,nz);

            //sp - вектор, указывающий на наблюдателя (камеру) из точки
            sp=rc->camera[0];
            vec3_sub(&sp,&real_pos);
            sp=vec3_norm(sp);
            for (int i=0;i<rc->s->point_light_count;i++)
            {
                if (rc->s->point_lights[i].far)
                {
                    l=rc->s->point_lights[i].pos;
                    vec3_cmul(-1.0,&l);
                    intens=rc->s->point_lights[i].intensity;
                }
                else
                {//вектор l - вектор от источника света до точки, все в глобальной с.к.
                    l=real_pos;
                    vec3_sub(&l,&rc->s->point_lights[i].pos);
                    if (rc->s->point_lights[i].fading)
                    {
                        intens=rc->s->point_lights[i].intensity/(vec3_len(&l)+0.001);
                    }
                    else intens=rc->s->point_lights[i].intensity;


                }
                //ref - вектор направление отраженного от поверхности света
                ref=normal;
                vec3_cmul(2*vec3_scalar(&ref,&l),&ref);
                vec3_sub(&ref,&l);
                ref=vec3_norm(ref);
                specular_k=pow(vec3_scalar(&ref,&sp),specular_power);


                l=vec3_norm(l);
                vec3_cmul(-1.0,&l);
                vec3_cross(&cr,&normal,&l);
                light_k=vec3_scalar(&l,&normal);

                if (light_k<0)
                {
                    light_k=0.0;
                    specular_k=0.0;
                }
                if (light_k>1) light_k=1.0;
                light_sum+=light_k*intens;
                specular_sum+=specular_k*intens;
            }

            if (light_sum>max_diffuse_q) light_sum=max_diffuse_q;
            if (specular_sum>max_specuar_q) specular_sum=max_specuar_q;
            register float res=ambient_q+diffuse_q*light_sum+specular_q*specular_sum;
            register int t_r,t_g,t_b;
            t_r =res*r;
            if (t_r>255) t_r=255;
            else if (t_r<0) t_r=0;
            t_g =res*g;
            if (t_g>255) t_g=255;
            else if (t_g<0) t_g=0;
            t_b =res*b;
            if (t_b>255) t_b=255;
            else if (t_b<0) t_b=0;
            r=t_r;
            g=t_g;
            b=t_b;

            return(SDL_MapRGB(rc->screen->format,r,g,b));

}
uint32_t zbuf_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw)
{
    int a=(int)position.x,b=(int)(position.y);
    int indx = b*rc->screen->w + a;
    //printf("%f_\n",rc->zbuf[indx]);
    int z=(int)(10*(rc->zbuf[indx]+vec3_len(&rc->camera[0])));
    if (z<0) z=0;
    else if (z>255) z=255;
    return(SDL_MapRGB(rc->screen->format,z,z,z));
}
