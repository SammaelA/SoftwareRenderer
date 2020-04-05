#ifndef SCENE_WORKER_H_INCLUDED
#define SCENE_WORKER_H_INCLUDED
#include "vec_math.h"
#include <stdbool.h>
#include "mash_loading.h"
#define MAX_POINT_LIGHTS 1000
#define MAX_MESHES 1000
SDL_Surface* basic_light_texture;
typedef struct{
vec3 pos;//������� ��������� �����
float intensity;//������������� �����,
bool fading;//���� false - ������������� �� ������������ � ��������
bool far;//���� true - ��� ������� ���������, ��� ������ ����������� �����
//� ����� ����� ������ ����� -pos, ������������, ���� ����� ����������
//�������� ����� ����� ����������� ����� ������ ����������
mash *model;
//������, ������ �������� ����� �������� �������� �����. ��� ��������
//��������� ���������� ��� �� ��������� - sphere.obj
} point_light;
typedef struct {
    /*����� - ��� ���������, ���������� � ���� ���������� � ����
      �����, ���������� ����� � ������ �������� �� ������
      ������������ ����� ������������ ��������� ����, �� ������ ����
      �� ��� main_scene ����� �����������.
    */
mash *meshes[MAX_MESHES];
//������ �� ����, ��������� � �����, � �������� ������ �� ���������� ���
//������� ������ ������� �� ��������� �����
vec3 *prepared_verts[MAX_MESHES+MAX_POINT_LIGHTS];
//����� ������ � ���������� �.�. ��� ������� �� ���������� �
//����� �����.
point_light point_lights[MAX_POINT_LIGHTS];
//������ �������� ���������� ����� (��. ����)
int mesh_count,point_light_count;
bool active,need_recalculation;
} scene;
void init_scene(scene *s);
bool add_mesh(scene *s,mash *m);//��������� ���, ��������� �������� ������ ������ ����
//��������� true, ���� �� ��������� ������������ ���������� �����
void scene_recalculate_all(scene *s);//�������������� �������� ��������� ���� ������ �����
void scene_recalculate(scene *s);
point_light mpoint_light_basic(vec3 position);
bool add_point_light(scene *s,point_light pl);
point_light mpoint_light(vec3 position,float intensity,bool fading,bool far);
#endif // SCENE_WORKER_H_INCLUDED
