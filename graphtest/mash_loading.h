#ifndef MASH_LOADING_H_INCLUDED
#define MASH_LOADING_H_INCLUDED
#include "vec_math.h"
#include <stdbool.h>
#include <SDL.h>
typedef struct polygon_s
{//тип полигон содержит количество вершин, массивы ссылок на входящие в него
 //вершины, текстуры и нормали
 int v_count;
 int vs[3];
 int vts[3],vns[3];
} polygon;
typedef struct line_s
{//тип линия содержит количество вершин, массивы ссылок на входящие в нее
 //вершины
    int v_count;
    vec4 *vs;

} line;
typedef struct mash_s
{
    mat4x4 position;
    vec3 scale;
    char *obj_name;//считаем, что в файле только один объект, игнорируем группы
    char *mtl_file_name;//название файла с текстурой
    char *material_name;//пока считаем, что материал только один
    char *unused_information;
    //сюда пойдут все строки исходного файла, которые не являются комментариями, но на
    //данный момент информация из них не используется:информация о группах и сглаживании
    int v_count,vt_count,vn_count,vp_count,f_count,l_count;
    //количество вершин, текстурных координат, нормалей, каких-то других вершин,
    //полигонов, линий соответственно
    vec4 *vs;//массив вершин
    vec3 *vts,*vns,*vps;//массивы текстурных координат, нормалей, каких-то других вершин
    polygon *polys;//массив полигонов
    line *lines;//массив линий
    SDL_Surface *texture;
    bool need_recalculation;
    bool no_light;
} mash;
mash *ReadObjFile(char path[]);
void mesh_copy(mash *to,mash *from);
void mesh_set_center_local(mash *m,vec3 new_center);
void mesh_set_default_center(mash *m);
void mesh_set_center_global(mash *m,vec3 new_center);
#endif // MASH_LOADING_H_INCLUDED
