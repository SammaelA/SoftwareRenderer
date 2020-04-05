#ifndef SCENE_WORKER_H_INCLUDED
#define SCENE_WORKER_H_INCLUDED
#include "vec_math.h"
#include <stdbool.h>
#include "mash_loading.h"
#define MAX_POINT_LIGHTS 1000
#define MAX_MESHES 1000
typedef struct{
vec3 pos;//позици€ источника света
float intensity;//интенсивность света,
bool fading;//если false - интенсивность не используетс€ в расчетах
bool far;//если true - при расчете считаетс€, что вектор направлени€ света
//в любой точке всегда равен -pos, использовать, если нужно равномерно
//осветить сцену €кобы наход€щимс€ очень далеко источником
//¬–≈ћ≈ЌЌќ: все источники считаютс€ far, другие считать пока не умеем
} point_light;
typedef struct {
    /*сцена - это структура, содержаща€ в себе информацию о всех
      мешах, источниках света и прочих объектах на экране
      одновременно может существовать несколько сцен, но только одна
      из них main_scene будет загружатьс€.
    */
mash *meshes[MAX_MESHES];
//ссылки на меши, имеющиес€ в сцене, в процессе работы не измен€ютс€ без
//пр€мого вызова функций по изменению мешей
vec3 *prepared_verts[MAX_MESHES];
//набор вершин в глобальной с.к. дл€ каждого из включенных в
//сцену мешей.
point_light point_lights[MAX_POINT_LIGHTS];
//массив точечных источников света (см. ниже)
int mesh_count,point_light_count;
bool active,need_recalculation;
} scene;
void init_scene(scene *s);
bool add_mesh(scene *s,mash *m);//добавл€ет меш, запускает пересчет вершин нового меша
//возвращет true, если не превышено максимальное количество мешей
void scene_recalculate_all(scene *s);//принудительный пересчет координат всех вершин сцены
point_light mpoint_light_basic(vec3 position);
bool add_point_light(scene *s,point_light pl);
#endif // SCENE_WORKER_H_INCLUDED
