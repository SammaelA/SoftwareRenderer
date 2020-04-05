/*#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "rasterization.h"
#include "mash_loading.h"
#include "vec_math.h"
#include "math.h"
#include "random_stuff.h"
const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 780;
long test(SDL_Surface *surf, triangle_data *td, int count){
    long t = SDL_GetTicks();
    for(int i = 0; i < count; i++){
        for(int j = 0; j < 3; j++) {
            td->vert[j].x = rand() % surf->w;
            assert(0 <= td->vert[j].x && td->vert[j].x < surf->w);
            td->vert[j].y = rand() % surf->h;
            assert(0 <= td->vert[j].y && td->vert[j].y  < surf->w);

        }
        td->color = SDL_MapRGB(surf->format, rand()%255, rand()%255, rand()%255);
        draw_triangle(surf, td);
    }
    return SDL_GetTicks() - t;
}
void draw_coin()
{
    static a=10,b=10;
    SDL_Rect dstrect;
    dstrect.x = a;
    dstrect.y = a;
    a=(a+1)%SCREEN_HEIGHT;
    dstrect.w = 32;
    dstrect.h = 32;
    if (SDL_BlitSurface( mImage, NULL, mScreenSurface, &dstrect))
    {
        printf("coin loaded");
    }
}
long triangle_fractal(int depth,triangle_data *td)
{

    if (depth==0)
    {
        SDL_Surface *n;
        n=SDL_ConvertSurface(mImage,mScreenSurface->format,mScreenSurface->flags);
        draw_tex_triangle(mScreenSurface,n,td);
        printf("drawn");
        return 1;
    }
    else
    {
        triangle_data tt1,tt2,tt3,*t1,*t2,*t3;
        t1=&tt1;
        t2=&tt2;
        t3=&tt3;
        int x1=(td->vert[1].x+td->vert[2].x)/2;
        int y1=(td->vert[1].y+td->vert[2].y)/2;
        int x2=(td->vert[0].x+td->vert[2].x)/2;
        int y2=(td->vert[0].y+td->vert[2].y)/2;
        int x3=(td->vert[0].x+td->vert[1].x)/2;
        int y3=(td->vert[0].y+td->vert[1].y)/2;
        t1->color=SDL_MapRGB(mScreenSurface->format, rand()%255, rand()%255, rand()%255);;
        t2->color=SDL_MapRGB(mScreenSurface->format, rand()%255, rand()%255, rand()%255);;
        t3->color=SDL_MapRGB(mScreenSurface->format, rand()%255, rand()%255, rand()%255);;
        t1->vert[0]=td->vert[0];
        t1->vert[1].x=x3;
        t1->vert[1].y=y3;
        t1->vert[2].x=x2;
        t1->vert[2].y=y2;

        t2->vert[1]=td->vert[1];
        t2->vert[0].x=x3;
        t2->vert[0].y=y3;
        t2->vert[2].x=x1;
        t2->vert[2].y=y1;

        t3->vert[2]=td->vert[2];
        t3->vert[0].x=x2;
        t3->vert[0].y=y2;
        t3->vert[1].x=x1;
        t3->vert[1].y=y1;
        printf("depth:%d \n",depth);
        long res=triangle_fractal(depth-1,t1)+triangle_fractal(depth-1,t2)+triangle_fractal(depth-1,t3);
        return (res);
     }
}
int graphmain(int argc, char** argv) {
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            draw_coin();
        }
            SDL_UpdateWindowSurface( mWindow );
            bool quit = false;
            bool rect_moved=false;;
    long time = 0, exps = 0;
    SDL_Rect mov_rect={.x=295,.y=215,.w=50,.h=50};
    while(!quit){
        SDL_Event event;
        rect_moved=false;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) quit = true;
            else if( event.type == SDL_KEYDOWN )
            {
               switch( event.key.keysym.sym )
               {
                  case SDLK_UP:
                  mov_rect.y--;
                    break;

                    case SDLK_DOWN:
                    mov_rect.y++;
                    break;

                    case SDLK_LEFT:
                    mov_rect.x--;
                    break;

                    case SDLK_RIGHT:
                    mov_rect.x++;
                    break;

                    case SDLK_MINUS:
                    mov_rect.h--;
                    mov_rect.w--;
                    break;

                    case SDLK_EQUALS:
                    mov_rect.h++;
                    mov_rect.w++;
                    break;
                    default:;
                    break;
               }
            }

        }
        //draw_coin();
        SDL_Delay(1000/60);
        //SDL_FillRect(mScreenSurface, NULL, SDL_MapRGB(mScreenSurface->format, 255, 255, 255));
        //time += test(surf, &triangle, 1000);
        exps++;
        SDL_FillRect(mScreenSurface, NULL, SDL_MapRGB(mScreenSurface->format, 0, 0, 0));
        SDL_FillRect(mScreenSurface, &mov_rect, SDL_MapRGB(mScreenSurface->format, 0, 255, 255));
        SDL_UpdateWindowSurface(mWindow);
    }
    printf("Average time: %lf\n", ((double)time)/exps);
    triangle_data *tst,tstt;
    tst=&tstt;
    tst->vert[0].x=700;
    tst->vert[0].y=10;
    tst->vert[1].x=10;
    tst->vert[1].y=750;
    tst->vert[2].x=1400;
    tst->vert[2].y=750;
    tst->color = SDL_MapRGB(mScreenSurface->format, rand()%255, rand()%255, rand()%255);
    tst->tex[0].x=0;
    tst->tex[0].y=0;
    tst->tex[1].x=100;
    tst->tex[1].y=0;
    tst->tex[2].x=0;
    tst->tex[2].y=30;
    SDL_FillRect(mScreenSurface, NULL, SDL_MapRGB(mScreenSurface->format, 0, 255, 255));
    triangle_fractal(0 ,tst);
    SDL_UpdateWindowSurface(mWindow);
    SDL_Delay(3000);
    g_close();
    return 0;
        }
    return 2;
}*/
