#include<stdio.h>
#define maxsize 100
#define QueueElementType int
#include"Queue.h"
void Yang_Hui_Triangle(int N)
{
    SeqQueue q;
    InitQueue(&q);
    Queue_Push(&q,1);//第一行元素入队
    for(int n=2;n<=N;++n)
    {
        Queue_Push(&q,1);//第n行的第一个元素入队
        for(int i=1;i<=n-2;++i)
        {
            QueueElementType tmp1,tmp2;
            Queue_Pop(&q,&tmp1);
            printf("%d ",tmp1);
            Queue_Front(&q,&tmp2);
            tmp2+=tmp1;
            Queue_Push(&q,tmp2);
        }
        QueueElementType x;
        Queue_Pop(&q,&x);
        printf("%d ",x);
        Queue_Push(&q,1);
        printf("\n");
    }
    while(!Queue_IsEmpty(&q))
    {
        QueueElementType x;
        Queue_Pop(&q,&x);
        printf("%d ",x);
    }
}
int main()
{
    Yang_Hui_Triangle(20);
    return 0;
}