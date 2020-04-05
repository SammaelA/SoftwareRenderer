#ifndef SCENE_WORKER_H_INCLUDED
#define SCENE_WORKER_H_INCLUDED
#include "vec_math.h"
#include <stdbool.h>
#include "mash_loading.h"
#define MAX_POINT_LIGHTS 1000
#define MAX_MESHES 1000
SDL_Surface* basic_light_texture;
typedef struct{
vec3 pos;//позиция источника света
float intensity;//интенсивность света,
bool fading;//если false - интенсивность не используется в расчетах
bool far;//если true - при расчете считается, что вектор направления света
//в любой точке всегда равен -pos, использовать, если нужно равномерно
//осветить сцену якобы находящимся очень далеко источником
mash *model;
//объект, внутри которого будет заключен источник света. При создвнии
//источника выбирается меш по умолчанию - sphere.obj
} point_light;
typedef struct {
    /*сцена - это структура, содержащая в себе информацию о всех
      мешах, источниках света и прочих объектах на экране
      одновременно может существовать несколько сцен, но только одна
      из них main_scene будет загружаться.
    */
mash *meshes[MAX_MESHES];
//ссылки на меши, имеющиеся в сцене, в процессе работы не изменяются без
//прямого вызова функций по изменению мешей
vec3 *prepared_verts[MAX_MESHES+MAX_POINT_LIGHTS];
//набор вершин в глобальной с.к. для каждого из включенных в
//сцену мешей.
point_light point_lights[MAX_POINT_LIGHTS];
//массив точечных источников света (см. ниже)
int mesh_count,point_light_count;
bool active,need_recalculation;
} scene;
void init_scene(scene *s);
bool add_mesh(scene *s,mash *m);//добавляет меш, запускает пересчет вершин нового меша
//возвращет true, если не превышено максимальное количество мешей
void scene_recalculate_all(scene *s);//принудительный пересчет координат всех вершин сцены
void scene_recalculate(scene *s);
point_light mpoint_light_basic(vec3 position);
bool add_point_light(scene *s,point_light pl);
point_light mpoint_light(vec3 position,float intensity,bool fading,bool far);
#endif // SCENE_WORKER_H_INCLUDED
