#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED
#include <inttypes.h>
#include <SDL.h>
#include <stdbool.h>
#include "vec_math.h"
#include "mash_loading.h"
#include "scene_worker.h"
#include "rasterization.h"
/*Ёто специальный раздел с шейдерами. ¬се функции шейдеров должны иметь одинаковый
 набор входных и выходных данных:
 render_context rc - он необходим дл€ доступа к данным о положении в глобальной
 системе координат того треугольника, который рисуетс€ на данный момент, камере и,
 возможно, еще каким-то вещам. √арантируетс€, что шейдер не будет измен€ть render_context

 position - вектор, где x, y указывают на положение отрисовываемой точки на экране
 z - ее координата в системе координат камеры (нужна дл€ z-буфера)

 normal - вектор, который воспринимаетс€ как нормальный вектор к плоскости объекта
 в данной точке в глобальной системе координат.
 √арантируетс€, что его длина равна 1, а направление указывает наружу
 по отношению к рисуемому объекту по крайней мере в случае, если объект €вл€етс€
 замкнутым телом

 bc_real - барицентрические координаты точки относительно того треугольника, которому
 она принадлежит, в глобальной системе координат. √арантируетс€, что сумма компонент
 bc_real равна 1, но не гарантируетс€ их неотрицательность

 raw - цветовое представление (RGB) данного пиксел€ на текстуре
 */
uint32_t no_light(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
//пустой шейдер, не мен€ет цвета
uint32_t Gourand_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
uint32_t Phong_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
uint32_t zbuf_shading(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw);
//uint32_t Phong_shading_with_shadows(render_context *rc,vec3 position,vec3 normal,vec3 bc_real,uint32_t raw); не готов
#endif // SHADERS_H_INCLUDED
