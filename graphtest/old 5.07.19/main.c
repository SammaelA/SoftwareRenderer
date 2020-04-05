#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "rasterization.h"
#include "mash_loading.h"
#include "vec_math.h"
#include "scene_worker.h"
#include "math.h"
vec3 camera[3];//
float camera_moving_speed=0.5;
SDL_Window* mWindow = NULL;
SDL_Surface* mScreenSurface = NULL;
SDL_Surface* mImage = NULL;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT =1080;
int convert_mesh_to_triangle_data(mash *m,triangle_data **td)
{//переводит 3d модель в готовый для отрисовки массив треугольников
 //где вершины треугольника имеют вид (x,y,z):
 //x,y - координаты тругольника на экране
 //*временно* если треугольник вылезает за экран, то он не рисуется вообще
 //z- z-координата треугольника в системе координат камеры
 //возвращает целое число - количество треугольников в массиве

 mat4x4 k;
 mat_lookat(k,&(camera[0]),&(camera[1]),&(camera[2]));
 printf("1\n");
 mat_print(k);
 mat_print(m->position);
  int c;
 //k - матрица перехода от глобальной системы координат к с.к. камеры
 //position - матрица перехода от с.к. меша к глобальной с.к.
 mat_amul(k,m->position);
 printf("2\n");
 //k=k*position - матрица перехода от с.к. меша к с.к. камеры
 mat_print(k);
 vec4 tmp_vecs[m->v_count+1];
 int i;
 vec3 tmp=camera[0];
 vec3_sub(&tmp,&camera[1]);
 float ck=vec3_len(&tmp);
 printf("camera position (%f,0,0)",ck);
 mat4x4 sm;
 mat_viewport(sm,(float)(SCREEN_HEIGHT),(float)(SCREEN_WIDTH));
 printf("3\n");
 //матрица перевода координат из с.к. камеры в с.к. экрана
 for (i=1;i<m->v_count+1;i++)
 {

     printf("masH:");
     vec4_print(&(m->vs[i-1]));
     mat_vmul(&(tmp_vecs[i]),k,&(m->vs[i-1]));
     printf("1 matr:");
     vec4_print(&(tmp_vecs[i]));
     //переводим все вершины в систему координат камеры
     //и проектируем все на плоскость OXY с.к. камеры
     float kk=ck/(ck-tmp_vecs[i].z);
     tmp_vecs[i].x*=kk;
     tmp_vecs[i].y*=kk;
     float x=tmp_vecs[i].x;
     float y=tmp_vecs[i].y;
     //компонента w показывает, находится ли точка в пределах видимости или нет
     printf("proe");
     vec4_print(&tmp_vecs[i]);
     if ((x>1)||(x<-1)||(y>1)||(y<-1))
        tmp_vecs[i].w=0.0;
     else
     {
         vec4 v;
         v=tmp_vecs[i];
         mat_vmul(&(tmp_vecs[i]),sm,&v);
     }
     printf("2 matr:");
     vec4_print(&(tmp_vecs[i]));
 }
 printf("4\n");
 int t_num=m->f_count;
 *td=(triangle_data*)(malloc(sizeof(triangle_data)*t_num));
 //выделяем память под треугольники
 vec2 tex[m->vt_count+1];
 //готовим текстурные векторы
 for (i=1;i<m->vt_count+1;i++)
 {
     tex[i].x=m->vts[i-1].x;
     tex[i].y=m->vts[i-1].y;
 }
 int triangles_count=0;
 triangle_data tmp_td;
 triangle_data *cur_td;
 cur_td=*td;
 printf("5\n");
 printf("%d",t_num);
 for (i=0;i<t_num;i++)
 {
     int ad=1;
     int a,b,j;
     for (j=0;j<3;j++)
     {
         a=m->polys[i].vs[j];
         b=m->polys[i].vts[j];
         ad=ad&&((a>0)&&(a<=m->v_count)&&(b>0)&&(b<=m->vt_count));
         vec4 tv;
         vec2 tt;
         tv=tmp_vecs[a];
         tt=tex[b];
         ad=ad&&(tv.w>0);
         tmp_td.tex[j]=tt;
         tmp_td.vert[j]=vec43_cvt(tv);
         printf("tmp_td_%d",j);
         vec3_print(&tmp_td.vert[j]);
     }
     if (ad)
     {
         printf("triangle %d added\n",i);
         *cur_td=tmp_td;
         cur_td++;
         triangles_count++;

     }
     else
     {
         printf("triangle %d not added\n",i);
     }

 }
 return(triangles_count);
}
void g_close()
{
    SDL_FreeSurface( mImage );
    mImage= NULL;
    SDL_DestroyWindow( mWindow );
    mWindow = NULL;
    SDL_Quit();
}
bool init()
{
    bool success = true;
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        mWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( mWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            mScreenSurface = SDL_GetWindowSurface( mWindow );
        }
    }

    return success;
}
bool loadTextures()
{
    bool success = true;
    SDL_Surface *temp_s = SDL_LoadBMP( "C:\\temp\\fish_texture.bmp" );
    if( temp_s == NULL )
    {
        printf( "Unable to load image\n");
        success = false;
    }
    mImage = SDL_ConvertSurface(temp_s, mScreenSurface->format, 0);
    SDL_FreeSurface(temp_s);

    return success;
}
float  initCamera()
{
    vec3 eye = {.x = 0.f, .y = 0.f, .z = -25.f},
    center = {.x = 0.f, .y = 0.f, .z = -20.f},
    up = {.x = 0.f, .y = 1.f, .z = 0.f};
    camera[0]=eye;
    camera[1]=center;
    camera[2]=up;
    vec3 tmp=camera[0];
    vec3_sub(&tmp,&camera[1]);
    float ck=vec3_len(&tmp);
    printf("camera position (%f,0,0)",ck);
 return(ck);
}

int emain(int argc, char** argv) {
    printf("hi!");
    ReadObjFile("C:\\temp\\cube.obj");
    int c;
    c=getchar();
    return(0);
}
void random_triangle(triangle_data *td, const SDL_Surface *surf){
    for(int j = 0; j < 3; j++){
        td->vert[j].x = rand()%surf->w;
        td->vert[j].y = rand()%surf->h;
        td->vert[j].z = (float)rand()/(float)RAND_MAX;
    }
}

long test(SDL_Surface *surf, SDL_Surface *tex, float *zbuf, triangle_data *td, int count){
    long t = SDL_GetTicks();
    for(int i = 0; i < count; i++){
        random_triangle(td, surf);
        td->tex[0] = (vec2){.x = 0.5f, .y = 0.f};
        td->tex[1] = (vec2){.x = 0.f, .y = 1.f};
        td->tex[2] = (vec2){.x = 1.f, .y = 1.f};
        td->color = SDL_MapRGB(surf->format, rand()%255, rand()%255, rand()%255);
        //draw_tex_triangle(surf, tex, zbuf, td);
    }
    return SDL_GetTicks() - t;
}

int main(int argc, char** argv) {
    printf("_%d_",sizeof(triangle_data));
    init();
    loadTextures();
    initCamera();
    float ck=initCamera();
    render_context  rc;
    mash *m = ReadObjFile("C:\\temp\\triangled_fish.obj");
    mash m1;
    mesh_copy(&m1,m);
    //mesh_set_default_center(m);
    //mesh_set_default_center(&m1);
    mat4x4 model, proj, temp_m, rot,rot1;
    vec3 v1={.x=0.f,.y=5.f,.z=0.f};
    vec3 v2={.x=0.f,.y=0.f,.z=0.f};
    mat_translate(rot,&v1);
    mat_translate(rot1,&v2);
    mat_rotate_y(model,M_PI_2);
    mat_amul(rot1,model);
    mat_amul(rot,model);
    mat_identity(model);
    mat_identity(m->position);
    mat_copy(m1.position,rot);
    scene s;
    init_scene(&s);
    add_mesh(&s,m);
    add_mesh(&s,&m1);
    point_light pl=mpoint_light_basic(mvec3(0.0,0.0,-10.0));
    add_point_light(&s,pl);
    printf("%d_",s.point_light_count);
    if(!init_context(&rc, mScreenSurface)){
        printf("Shit happens\n");
        return 1;
    }
    attach_texture(&rc, mImage);
    long time = 0, frames = 0, dt = 0;
    bool quit = false;
    //long time = 0, exps = 0;
    while(!quit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) quit = true;
            else if( event.type == SDL_KEYDOWN )
            {
               switch( event.key.keysym.sym )
               {
                    case SDLK_UP:
                    camera[0].y+=camera_moving_speed;
                    camera[1].y+=camera_moving_speed;
                    break;

                    case SDLK_DOWN:
                    camera[0].y-=camera_moving_speed;
                    camera[1].y-=camera_moving_speed;
                    break;

                    case SDLK_LEFT:
                    camera[0].x+=camera_moving_speed;
                    camera[1].x+=camera_moving_speed;
                    break;

                    case SDLK_RIGHT:
                    camera[0].x-=camera_moving_speed;
                    camera[1].x-=camera_moving_speed;
                    break;

                    case SDLK_MINUS:
                    camera[0].z-=camera_moving_speed;
                    camera[1].z-=camera_moving_speed;
                    break;

                    case SDLK_EQUALS:
                    camera[0].z+=camera_moving_speed;
                    camera[1].z+=camera_moving_speed;
                    break;
                    default:;
                    break;
               }
            }

        }
        dt = SDL_GetTicks();
        SDL_FillRect(mScreenSurface, NULL, SDL_MapRGB(mScreenSurface->format, 255, 200, 200));
        mat_identity(model);
        mat_rotate(temp_m,0.00,0.01,0.00);
        mat_amul(m->position,temp_m);
        scene_recalculate_all(&s);
        draw_scene(&rc,&s,camera);
       /* mat_lookat(proj, &camera[0], &camera[1], &camera[2]);
        mat_amul(model,rot1);
        mat_amul(model,m->position);
        mat_amul(proj,model);
        attach_mesh(&rc, m, proj, ck);
        draw_all(&rc);
        mat_lookat(proj, &camera[0], &camera[1], &camera[2]);
        mat_amul(proj,rot);
        attach_mesh(&rc, &m1, proj, ck);
        draw_all(&rc);*/
        SDL_UpdateWindowSurface(mWindow);
        dt = SDL_GetTicks() - dt;
        frames++;
        time += dt;
        float cur_time=1000/30-dt;
        if (cur_time<=0)
        {
            printf("lags!! %d\n",dt);
            cur_time=1;
        }
        SDL_Delay(cur_time);
    }
    printf("Average fps: %lf\n", ((double)(frames*1000))/time);
    free_context(&rc);
    SDL_Delay(50000);
    g_close();
    return 0;
}
