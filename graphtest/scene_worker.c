#include <stdio.h>
#include <stdbool.h>
#include "scene_worker.h"
#include "vec_math.h"
#include "mash_loading.h"
SDL_Surface* basic_light_texture= NULL;
point_light mpoint_light_basic(vec3 position)
{
    point_light pl;
    pl.pos=position;
    pl.intensity=1.0;
    pl.far=true;
    pl.fading=false;
    return(pl);
}
point_light mpoint_light(vec3 position,float intensity,bool fading,bool far)
{
    point_light pl;
    pl.pos=position;
    pl.intensity=intensity;
    pl.far=far;
    pl.fading=fading;

    pl.model=ReadObjFile("C:\\temp\\sphere.obj");
    mesh_set_default_center(pl.model);
    pl.model->texture=basic_light_texture;
    pl.model->scale=mvec3(0.25,0.25,0.25);
    mat_translate(pl.model->position,&position);
    pl.model->no_light=true;
    return(pl);
}
vec3 *scene_recalculate_mesh(scene *s,mash *m,vec3 verts[])
{
    free(verts);
    verts=(vec3*)(malloc(sizeof(vec3)*m->v_count));
    vec4 tmp;
    for (int i=0;i<m->v_count;i++)
    {
        vec4 tmp2=mvec4(m->vs[i].x*m->scale.x,m->vs[i].y*m->scale.y,m->vs[i].z*m->scale.z,m->vs[i].w);
        mat_vmul(&tmp,m->position,&tmp2);
        verts[i]=vec43_cvt(tmp);
        //printf("recalculating %d",m->v_count);
    }
    m->need_recalculation=false;
    return(verts);

}
void init_scene(scene *s)
{
    s->mesh_count=0;
    s->point_light_count=0;
}
bool add_mesh(scene *s,mash *m)
{
    if (s->mesh_count<MAX_MESHES)
    {
        s->meshes[s->mesh_count]=m;
        s->prepared_verts[s->mesh_count]=scene_recalculate_mesh(s,s->meshes[s->mesh_count],s->prepared_verts[s->mesh_count]);
        (s->mesh_count)++;
        return(true);
    }
    else
    {
        return(false);
    }


}
void scene_recalculate_all(scene *s)
{
    for (int i=0;i<s->mesh_count;i++)
    {
       s->prepared_verts[i]= scene_recalculate_mesh(&s,s->meshes[i],s->prepared_verts[i]);
    }
    for (int i=0;i<s->point_light_count;i++)

    {
        s->prepared_verts[s->mesh_count+i]= scene_recalculate_mesh(&s,s->point_lights[i].model,s->prepared_verts[s->mesh_count+i]);
    }
}
void scene_recalculate(scene *s)
{
    for (int i=0;i<s->mesh_count;i++)
    {
       if (s->meshes[i]->need_recalculation)
        s->prepared_verts[i]= scene_recalculate_mesh(&s,s->meshes[i],s->prepared_verts[i]);
    }
}
bool add_point_light(scene *s,point_light pl)
{
    if (s->point_light_count<MAX_POINT_LIGHTS)
    {
        int k=(s->point_light_count);
        (s->point_lights)[k]=pl;
        s->point_light_count++;
        return(true);
    }
    else
    {
        return(false);
    }
}
