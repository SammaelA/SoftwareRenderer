#ifndef RASTERIZATION_H_INCLUDED
#define RASTERIZATION_H_INCLUDED

#include <inttypes.h>
#include <SDL.h>
#include <stdbool.h>
#include "vec_math.h"
#include "mash_loading.h"
#include "scene_worker.h"
typedef struct {
    vec3 vert[3];
    vec3 normales[3];
    vec2 tex[3];
    uint32_t color;
    bool suspicious;
} triangle_data;

typedef struct {
    SDL_Surface *screen, *texture;
    float *zbuf;
    float *shadow_buf;//буфер теней, используется шейдерами, которые просчитывают тени
    //пока теней нет
    vec3 *camera;//камера - массив из трех векторов в следующем порядке: eye,center,up
    triangle_data cur_triangle;//данные о треугольнике, который рисуется в данный
    //момент, его вершины и нормали должны быть в глобальной системе коориднат
    scene *s;
    uint32_t(*shader)(void *,vec3, vec3, vec3,uint32_t);
    //шейдер, который применяется в данный момент
} render_context;
bool init_context(render_context *rc, SDL_Surface *window);
//bool init_context(render_context *rc, SDL_Surface *window, int max_triangles, int max_verts);
bool attach_mesh(render_context *rc, const mash *m, const mat4x4 transform, float dist);
void attach_texture(render_context *rc, SDL_Surface *tex);
void draw_all(render_context *rc);
void free_context(render_context *rc);


void draw_triangle(SDL_Surface *surf, triangle_data* td);
//Внимание! Для правильной работы, формат текстуры должен совпадать с форматом поверхности окна
//(что не происходит по умолчанию). Используйте SDL_ConvertSurface для этой цели
// zbuf - массив размера surf->w * surf->h;
// для правильной работы перед началом отрисовки 3d мира должен быть заполнен маленькими отрицательными числами
// (например -1e10)
bool draw_tex_triangle(render_context *rc, triangle_data *td);


#endif // RASTERIZATION_H_INCLUDED
