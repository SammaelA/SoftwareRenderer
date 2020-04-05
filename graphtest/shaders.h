#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED
#include <inttypes.h>
#include <SDL.h>
#include <stdbool.h>
#include "vec_math.h"
#include "mash_loading.h"
#include "scene_worker.h"
#include "rasterization.h"
/*��� ����������� ������ � ���������. ��� ������� �������� ������ ����� ����������
 ����� ������� � �������� ������:
 render_context rc - �� ��������� ��� ������� � ������ � ��������� � ����������
 ������� ��������� ���� ������������, ������� �������� �� ������ ������, ������ �,
 ��������, ��� �����-�� �����. �������������, ��� ������ �� ����� �������� render_context

 position - ������, ��� x, y ��������� �� ��������� �������������� ����� �� ������
 z - �� ���������� � ������� ��������� ������ (����� ��� z-������)

 normal - ������, ������� �������������� ��� ���������� ������ � ��������� �������
 � ������ ����� � ���������� ������� ���������.
 �������������, ��� ��� ����� ����� 1, � ����������� ��������� ������
 �� ��������� � ��������� ������� �� ������� ���� � ������, ���� ������ ��������
 ��������� �����

 bc_real - ���������������� ���������� ����� ������������ ���� ������������, ��������
 ��� �����������, � ���������� ������� ���������. �������������, ��� ����� ���������
 bc_real ����� 1, �� �� ������������� �� �����������������

 raw - �������� ������������� (RGB) ������� ������� �� ��������
 */
uint32_t no_light(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
//������ ������, �� ������ �����
uint32_t Gourand_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
uint32_t Phong_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
uint32_t zbuf_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
//uint32_t Phong_shading_with_shadows(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw); �� �����
#endif // SHADERS_H_INCLUDED
