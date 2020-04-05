#ifndef MASH_LOADING_H_INCLUDED
#define MASH_LOADING_H_INCLUDED
#include "vec_math.h"
#include <stdbool.h>
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
    bool need_recalculation;
} mash;
mash *ReadObjFile(char path[]);
void *mesh_copy(mash *to,mash *from);
#endif // MASH_LOADING_H_INCLUDED
