#include <stdio.h>
int delsum(int a)
{
    int sum=0;
    for (int i=1;i<a;i++)
    {
        if (((a%i))==0) sum+=i;
    }
    return(sum);
}
int main()
{
    int n;
    scanf("%d",&n);
    printf("����������� �����:");
    for (int i=1;i<n;i++)
    {
        if (i==delsum(i)) printf("%d ",i);
    }
}
