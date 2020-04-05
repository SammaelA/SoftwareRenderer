#include "mash_loading.h"
#include "vec_math.h"
#include <inttypes.h>
#include <stdio.h>
#include <float.h>
#define false 0
#define true 1
#include <stdlib.h>
 FILE *fp;
int read_next_str(char res[])
//читает из объектного файла следующую строку,
//возвращает true, если файл еще не закночился
{
    static int str_count=0;
    int c;
    while ((((c = getc(fp)) != EOF))&&(c!='\n'))
    {
        *res=c;
        res++;
    }
    *res='\0';
    if (c==EOF) str_count=0;
    return(c!= EOF);
}
int starts_with(char *a,char *b)
{//начинается ли строка a со строки b
    while ((*b!='\0')&&(*a!='\0')&&(*a==*b))
    {
        a++;
        b++;
    }
    return(*b=='\0');
}
char* space_split(char* from,char* to,char splitter)
{//копирует в строку to начало строки from до первого вхождения символа
 //splitter или конца строки.
 // Возвращает ссылку на следующий после него элемент
 //или признак конца строки
    while ((*from!=splitter)&&(*from!='\0'))
    {
        *to++=*from++;
    }
    *to='\0';
    while (*from==splitter)
    {
        from++;
    }
    return(from);
}
int string_to_unsigned_int(char* from)
{//переводит строку в целое беззнаковое число,
 //если строка не является таким числом, то возвращает -1
    int a=0,d=0;
    int correct =true;
    while ((correct)&&(*from!='\0'))
    {
        d=*from-'0';
        if ((d<0)||(d>9)) correct=false;
        else
        {
            a=a*10+d;
            from++;
        }
    }
    if (!correct) a=-1;
    return(a);

}
float string_to_float(char* from)
{//переводит строку, в которой записана десятичная дробь с
 //точкой в качестве разделителя в соответствующее число
 //в случае, если строка не является корректной записью
 //дробного числа, возвращает число FLT_MAX
    char dot=false;
    char correct=true;
    char digit=false;
    float res=0.0,pow=1.0;
    int sign=1;
    if (*from=='-')
    {sign=-1;
    from++;
    }
    while (correct&&(*from!='\0'))
    {
        char c=*from;
        if (c=='.')
        {
            if (digit&&(!dot)) dot=true;
            else correct=false;
        }
        else if ((c>='0')&&(c<='9'))
        {

            int k=c-'0';
            digit=true;
            res=res*10+k;
            if (dot) pow=pow*10;

        }
        else
        {
            correct=false;
        }
        from++;

    }
    correct=correct&&digit;
    if (!correct) res=FLT_MAX;
    else res=sign*res/pow;
    return(res);
}
vec4 read_v_from_string(char* from)
{
    char tmp[1000];
    char* tt;
    vec4 v;
    tt=space_split(from,tmp,' ');
    //printf("reading v:%s",tmp);
    tt=space_split(tt,tmp,' ');
    v.x=string_to_float(tmp);
    tt=space_split(tt,tmp,' ');
    v.y=string_to_float(tmp);
    tt=space_split(tt,tmp,' ');
    v.z=string_to_float(tmp);
    if (*tt!='\0')
    {
      tt=space_split(tt,tmp,' ');
      v.w=string_to_float(tmp);
    }
    else
    {
        v.w=1.0;
    }
    //printf("%f %f %f %f",v.x,v.y,v.z,v.w);
    //printf("\n");
    return(v);

}
vec3 read_vt_from_string(char* from)
{
    char tmp[1000];
    char* tt;
    vec3 v;
    tt=space_split(from,tmp,' ');
    //printf("reading vt:%s",tmp);
    int i=0;
    tt=space_split(tt,tmp,' ');
    v.x=string_to_float(tmp);
    tt=space_split(tt,tmp,' ');
    v.y=string_to_float(tmp);
    if (*tt!='\0')
    {
      tt=space_split(tt,tmp,' ');
      v.z=string_to_float(tmp);
    }
    else
    {
        v.z=0.0;
    }
    //printf("%f %f %f ",v.x,v.y,v.z);
    //printf("\n");
    return(v);
}
vec3 read_vn_from_string(char* from)
{
    char tmp[1000];
    char* tt;
    vec3 v;
    tt=space_split(from,tmp,' ');
    //printf("reading vn:%s",tmp);
    int i=0;
    tt=space_split(tt,tmp,' ');
    v.x=string_to_float(tmp);
    tt=space_split(tt,tmp,' ');
    v.y=string_to_float(tmp);
    if (*tt!='\0')
    {
      tt=space_split(tt,tmp,' ');
      v.z=string_to_float(tmp);
    }
    else
    {
        v.z=0.0;
        printf("error!!!Broken normal vecor");
    }
    //printf("%f %f %f ",v.x,v.y,v.z);
    //printf("\n");
    return(v);
}
vec3 read_vp_from_string(char* from)
{
    char tmp[1000];
    char* tt;
    vec3 v;
    tt=space_split(from,tmp,' ');
   // printf("reading vp:%s",tmp);
    int i=0;
    tt=space_split(tt,tmp,' ');
    v.x=string_to_float(tmp);
    if (*tt!='\0')
    {
      tt=space_split(tt,tmp,' ');
      v.y=string_to_float(tmp);
    }
    else
    {
        v.y=0.0;
    }
    if (*tt!='\0')
    {
      tt=space_split(tt,tmp,' ');
      v.z=string_to_float(tmp);
    }
    else
    {
        v.z=0.0;
    }
    //printf("%f %f %f ",v.x,v.y,v.z);
    //printf("\n");
    return(v);
}
polygon read_polygon_from_string(char* from)
{
    polygon pol;
    pol.v_count=3;
    int i;
    char tmp[1000];
    char* tt;
    tt=space_split(from,tmp,' ');
    for (i=0;i<3;i++)
    {
        tt=space_split(tt,tmp,' ');
        int num;
        char tmp2[1000];
        char *tt2;
        tt2=space_split(tmp,tmp2,'/');
        pol.vs[i]=string_to_unsigned_int(tmp2);
        tt2=space_split(tt2,tmp2,'/');
        pol.vts[i]=string_to_unsigned_int(tmp2);
        tt2=space_split(tt2,tmp2,'/');
        pol.vns[i]=string_to_unsigned_int(tmp2);
        //printf("%d_",pol.vts[i]);

    }

    //printf("\n");
    return(pol);
}
void delete_mash(mash *m)
{//освобождение памяти. Надеюсь, утечек не будет...
free(m->vs);
free(m->vts);
free(m->vns);
free(m->vps);
free(m->polys);
}
void mesh_set_center_local(mash *m,vec3 new_center)
{
    //устанавливает новый центр в точке new_center в системе координат меша
    //vec4 v=vec34_cvt(new_center,0.0);
    vec4 v=vec34_cvt(new_center,0.0);
    mat_shift(m->position,&v);
    vec3_cmul(-1.0,&new_center);
    v=vec34_cvt(new_center,0.0);
    for (int i=0;i<m->v_count;i++)
    {
        vec4 t=m->vs[i];
        vec4_add(&t,&v);
        m->vs[i]=t;

    }
}
void mesh_set_center_global(mash *m,vec3 new_center)
{
    //устанавливает новый центр в точке new_center в глобальной с.к.
    mat4x4 rev;
    mat_reverse(rev,m->position);
    vec4 tm1,tm2=vec34_cvt(new_center,1.0);
    mat_vmul(&tm1,rev,&tm2);
    vec4_print(&tm1);
    //vec3 tt=mvec3(tm1.x/(m->scale.x),tm1.y/(m->scale.y),tm1.z/(m->scale.z));
    mesh_set_center_local(m,vec43_cvt(tm1));
}
void mesh_set_default_center(mash *m)
{//устанавливает центр по умолчанию - среднее арифметическое всех вершин
    float x=0.0,y=0.0,z=0.0;
    for (int i=0;i<m->v_count;i++)
    {
        x+=m->vs[i].x;
        y+=m->vs[i].y;
        z+=m->vs[i].z;
    }
    x=x/(m->v_count+0.0);
    y=y/(m->v_count+0.0);
    z=z/(m->v_count+0.0);
    mesh_set_center_local(m,mvec3(x,y,z));
}
void mesh_copy(mash *to,mash *from)
{
    delete_mash(to);
    to->v_count=from->v_count;
    to->vn_count=from->vn_count;
    to->vt_count=from->vt_count;
    to->vp_count=from->vp_count;
    to->f_count=from->f_count;

    mat_copy(to->position,from->position);

    to->vs=(vec4*)(malloc(sizeof(vec4)*to->v_count));
    to->vns=(vec3*)(malloc(sizeof(vec3)*to->vn_count));
    to->vts=(vec3*)(malloc(sizeof(vec3)*to->vt_count));
    to->vps=(vec3*)(malloc(sizeof(vec3)*to->vp_count));
    to->polys=(polygon*)(malloc(sizeof(polygon)*to->f_count));
    int i;
    for (i=0;i<to->v_count;i++)
    {
        to->vs[i]=from->vs[i];
    }
    for (i=0;i<to->vn_count;i++)
    {
        to->vns[i]=from->vns[i];
    }
    for (i=0;i<to->vt_count;i++)
    {
        to->vts[i]=from->vts[i];
    }
    for (i=0;i<to->vp_count;i++)
    {
        to->vps[i]=from->vps[i];
    }
    for (i=0;i<to->f_count;i++)
    {
        to->polys[i]=from->polys[i];
    }

}
int counter(mash *m)
{
/*считает количество вершин, нормалей и т.д., выделяет соответствующее
количество памяти под массивы этих элементов и присваивает массивам
из mash указатели на эту память */
char tmp[1000];
m->v_count=0;
m->vt_count=0;
m->vn_count=0;
m->vp_count=0;
m->f_count=0;
m->l_count=0;
while (read_next_str(tmp))
{
    if  (starts_with(tmp,"vp "))
        (m->vp_count)++;
    else if (starts_with(tmp,"vt "))
        (m->vt_count)++;
    else if (starts_with(tmp,"vn "))
        (m->vn_count)++;
    else if (starts_with(tmp,"v "))
        (m->v_count)++;
    else if (starts_with(tmp,"f "))
        (m->f_count)++;
    else if (starts_with(tmp,"l "))
        (m->l_count)++;
}
m->vs=(vec4*)(malloc(sizeof(vec4)*m->v_count));
m->vns=(vec3*)(malloc(sizeof(vec3)*m->vn_count));
m->vts=(vec3*)(malloc(sizeof(vec3)*m->vt_count));
m->vps=(vec3*)(malloc(sizeof(vec3)*m->vp_count));
m->polys=(polygon*)(malloc(sizeof(polygon)*m->f_count));
printf("Reading .obj file... %d %d %d %d %d %d \n",m->v_count,m->vt_count,m->vn_count,m->vp_count,m->f_count,m->l_count);
return(0);
}
int reader(mash *m)
{
    //
    char tmp[1000];
    vec4 *cur_v=m->vs;
    vec3 *cur_vt=m->vts;
    vec3 *cur_vn=m->vns;
    vec3 *cur_vp=m->vps;
    polygon *cur_poly=m->polys;
    while (read_next_str(tmp))
{
    if  (starts_with(tmp,"vp "))
        {
            *cur_vp=read_vp_from_string(tmp);
            cur_vp++;

        }
    else if (starts_with(tmp,"vt "))
        {
            *cur_vt=read_vt_from_string(tmp);
            cur_vt++;

        }
    else if (starts_with(tmp,"vn "))
        {
            *cur_vn=vec3_norm(read_vn_from_string(tmp));
            cur_vn++;

        }
    else if (starts_with(tmp,"v "))
        {
            *cur_v=read_v_from_string(tmp);
            cur_v++;

        }
    else if (starts_with(tmp,"f "))
        {
            *cur_poly=read_polygon_from_string(tmp);
            cur_poly++;

        }
    else if (starts_with(tmp,"l "));
        //(m->l_count)++;
}
    return(0);
}
mash *ReadObjFile(char path[])
{
   mash mn;
   fp = fopen(path, "r");
   if (fp==NULL)
   {
       printf("cannot find file %s \n",path);
       return NULL;
   }
   else
   {
       mash *tm;
       tm=(mash*)(malloc(sizeof(mash)));
       counter(tm);
       fclose(fp);
       fp = fopen(path, "r");
       reader(tm);
       mat_identity(tm->position);
       tm->scale=mvec3(1.0,1.0,1.0);
       return tm;
   }
}
