#ifndef MASH_LOADING_H_INCLUDED
#define MASH_LOADING_H_INCLUDED
#include "vec_math.h"
#include <stdbool.h>
#include <SDL.h>
typedef struct polygon_s
{//��� ������� �������� ���������� ������, ������� ������ �� �������� � ����
 //�������, �������� � �������
 int v_count;
 int vs[3];
 int vts[3],vns[3];
} polygon;
typedef struct line_s
{//��� ����� �������� ���������� ������, ������� ������ �� �������� � ���
 //�������
    int v_count;
    vec4 *vs;

} line;
typedef struct mash_s
{
    mat4x4 position;
    vec3 scale;
    char *obj_name;//�������, ��� � ����� ������ ���� ������, ���������� ������
    char *mtl_file_name;//�������� ����� � ���������
    char *material_name;//���� �������, ��� �������� ������ ����
    char *unused_information;
    //���� ������ ��� ������ ��������� �����, ������� �� �������� �������������, �� ��
    //������ ������ ���������� �� ��� �� ������������:���������� � ������� � �����������
    int v_count,vt_count,vn_count,vp_count,f_count,l_count;
    //���������� ������, ���������� ���������, ��������, �����-�� ������ ������,
    //���������, ����� ��������������
    vec4 *vs;//������ ������
    vec3 *vts,*vns,*vps;//������� ���������� ���������, ��������, �����-�� ������ ������
    polygon *polys;//������ ���������
    line *lines;//������ �����
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
