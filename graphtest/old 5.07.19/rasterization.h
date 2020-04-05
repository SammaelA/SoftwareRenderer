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
    vec3 vert[3];//������� ������������, ������� �������� � ������ ������, � �������
    //��������� ������, ���������� ��� ����������� ���������� ��������
    scene *s;
} render_context;
bool init_context(render_context *rc, SDL_Surface *window);
//bool init_context(render_context *rc, SDL_Surface *window, int max_triangles, int max_verts);
bool attach_mesh(render_context *rc, const mash *m, const mat4x4 transform, float dist);
void attach_texture(render_context *rc, SDL_Surface *tex);
void draw_all(render_context *rc);
void free_context(render_context *rc);


void draw_triangle(SDL_Surface *surf, triangle_data* td);
//��������! ��� ���������� ������, ������ �������� ������ ��������� � �������� ����������� ����
//(��� �� ���������� �� ���������). ����������� SDL_ConvertSurface ��� ���� ����
// zbuf - ������ ������� surf->w * surf->h;
// ��� ���������� ������ ����� ������� ��������� 3d ���� ������ ���� �������� ���������� �������������� �������
// (�������� -1e10)
bool draw_tex_triangle(render_context *rc, triangle_data *td);


#endif // RASTERIZATION_H_INCLUDED
