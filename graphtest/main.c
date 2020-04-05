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
SDL_Surface* white_texture= NULL,*rainbow_texture= NULL,*light_texture= NULL;
SDL_Surface *fish_texture= NULL,*gray_texture= NULL;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT =1080;

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
    SDL_Surface *temp_s = SDL_LoadBMP( "C:\\temp\\test2.bmp" );
    if( temp_s == NULL )
    {
        printf( "Unable to load rainbow_texture\n");
        success = success&&false;
    }
    rainbow_texture = SDL_ConvertSurface(temp_s, mScreenSurface->format, 0);
    SDL_FreeSurface(temp_s);

    temp_s = SDL_LoadBMP( "C:\\temp\\test1.bmp" );
    if( temp_s == NULL )
    {
        printf( "Unable to load white_texture\n");
        success = success&&false;
    }
    white_texture = SDL_ConvertSurface(temp_s, mScreenSurface->format, 0);
    SDL_FreeSurface(temp_s);

    temp_s = SDL_LoadBMP( "C:\\temp\\test3.bmp" );
    if( temp_s == NULL )
    {
        printf( "Unable to load gray_texture\n");
        success = success&&false;
    }
    gray_texture = SDL_ConvertSurface(temp_s, mScreenSurface->format, 0);
    SDL_FreeSurface(temp_s);

    temp_s = SDL_LoadBMP( "C:\\temp\\fish_texture.bmp" );
    if( temp_s == NULL )
    {
        printf( "Unable to load fish_texture\n");
        success = success&&false;
    }
    fish_texture = SDL_ConvertSurface(temp_s, mScreenSurface->format, 0);
    SDL_FreeSurface(temp_s);

    basic_light_texture=white_texture;
    return success;
}
float  initCamera()
{
    vec3 eye = {.x = 0.f, .y = 0.f, .z = -55.f},
    center = {.x = 0.f, .y = 0.f, .z = -50.f},
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

int main(int argc, char** argv) {
    init();
    loadTextures();
    initCamera();
    float ck=initCamera();
    render_context  rc;
    rc.camera=&camera[0];
    mash *m1= ReadObjFile("C:\\temp\\stone1.obj");
    mash *m2 =ReadObjFile("C:\\temp\\stone1.obj");
    mash *m3 =ReadObjFile("C:\\temp\\stone1.obj");
    mash *m4 =ReadObjFile("C:\\temp\\stone1.obj");
    mesh_set_default_center(m1);
    mesh_set_default_center(m2);
    mesh_set_default_center(m3);
    mesh_set_default_center(m4);
    m1->texture=gray_texture;
    m2->texture=gray_texture;
    m3->texture=gray_texture;
    m4->texture=gray_texture;
    m1->scale=mvec3(0.33,0.33,0.33);
    m2->scale=mvec3(0.33,0.33,0.33);
    m3->scale=mvec3(0.33,0.33,0.33);
    m4->scale=mvec3(0.33,0.33,0.33);
    mat4x4 model, proj, temp_m, rot,rot1;

    vec3 v1={.x=5.f,.y=0.f,.z=0.f};
    vec3 v2={.x=10.f,.y=0.f,.z=0.f};
    vec3 v3={.x=0.f,.y=0.f,.z=0.f};
    vec3 v4={.x=15.f,.y=0.f,.z=0.f};

    mat_translate(m1->position,&v1);
    mat_translate(m2->position,&v2);
    mat_translate(m3->position,&v3);
    mat_translate(m4->position,&v4);

    //mesh_set_center_global(m1,mvec3(0.0,0.0,0.0));
    //mesh_set_center_global(m2,mvec3(0.0,0.0,0.0));
    //mesh_set_center_global(m4,mvec3(0.0,0.0,0.0));

    scene s;
    init_scene(&s);
    add_mesh(&s,m1);
    add_mesh(&s,m2);
    add_mesh(&s,m3);
    add_mesh(&s,m4);
    point_light pl;
    for (int j=-51;j<=50;j+=50)
    {
       pl=mpoint_light(mvec3(0.0,j,0.0),15.0,true,false);
       add_point_light(&s,pl);
    }

    printf("%d_",s.point_light_count);
    if(!init_context(&rc, mScreenSurface)){
        printf("Shit happens\n");
        return 1;
    }
    //attach_texture(&rc, mImage);
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

                    case SDLK_p:
                    vec3_print(&camera[1]);
                    break;

                    case SDLK_f:
                    printf("Average fps: %lf\n", ((double)(frames*1000))/time);
                    break;
                    default:;
                    break;
               }
            }

        }

        dt=0;
        dt = SDL_GetTicks();
        SDL_FillRect(mScreenSurface, NULL, SDL_MapRGB(mScreenSurface->format, 255, 200, 200));
        mat_identity(model);
        mat_rotate(temp_m,0.03,0.03,0.03);

        mat_amul(m1->position,temp_m);
        mat_amul(m2->position,temp_m);
        //mat_amul(m3->position,temp_m);
        mat_amul(m4->position,temp_m);
        mat_reverse(temp_m,m1->position);
        m1->need_recalculation=true;
        scene_recalculate_all(&s);
        draw_scene(&rc,&s,camera);
        SDL_UpdateWindowSurface(mWindow);
        dt = SDL_GetTicks() - dt;
        frames++;
        time += dt;
        //float cur_time=1000/30-dt;
        //printf("render time %d\n",dt);

    }
    printf("Average fps: %lf\n", ((double)(frames*1000))/time);
    free_context(&rc);
    SDL_Delay(50000);
    g_close();
    return 0;
}
