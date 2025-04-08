//构建图的邻接矩阵、邻接表存储结构，实现图的深度优先搜索遍历、广度优先搜索遍历。 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define QueueElementType int
#define maxsize 100
#include"queue.h"
#define MAX_VERTEX_NUM 20
#define INFNITY 32768
#define VertexData char
typedef enum{DG,DN,UDG,UDN}GraphKind;
//DG有向图，DN有向网，UDG无向图，UDN无向网
typedef char EertexData;
int visited[MAX_VERTEX_NUM];
typedef struct ArcNode
{
    int adjvex;
    int weight;
    struct ArcNode *nextarc;
}ArcNode;
typedef struct VertexNode
{
    VertexData data;
    ArcNode *firstarc;
}VertexNode;
typedef struct
{
    VertexNode vertex[MAX_VERTEX_NUM];
    int vexnum,arcnum;//图的顶点数，边数
    GraphKind kind;
}AdjList;
int get_num(AdjList *graph,VertexData target)
{
    for(int i=1;i<=graph->vexnum;++i)
    {
        if(graph->vertex[i].data==target)
        return i;
    }
    return -1;
}
void build(AdjList *graph)
{
    printf("请输入图的顶点数:");
    scanf("%d",&(graph->vexnum));
    printf("请输入图各个顶点的名称:(不要带有空格)");
    for(int i=1;i<=graph->vexnum;++i)
    {
        char input;
        do scanf("%c",&input);while(input=='\n');
        graph->vertex[i].data=input;
        graph->vertex[i].firstarc=NULL;
    }
    printf("输入边的数量:");
    scanf("%d",&(graph->arcnum));
    printf("输入各个顶点之间的关系(边a) (边b) (边权):\n");
    for(int i=1;i<=graph->arcnum;++i)
    {
        char a,b;
        int c;
        scanf("\n%c %c %d",&a,&b,&c);
        int a_num=get_num(graph,a);
        int b_num=get_num(graph,b);
        if(a_num==-1||b_num==-1)
        {
            printf("输入数据错误,建图失败\n");
            return;
        }
        ArcNode *new_node=(ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL)
        {
            printf("内存分配失败\n");
            return;
        }
        new_node->nextarc=graph->vertex[a_num].firstarc;
        graph->vertex[a_num].firstarc=new_node;
        new_node->adjvex=b_num;
        new_node->weight=c;
    }
}
// 辅助函数：从指定顶点 v 开始进行深度优先搜索
void dfs(AdjList *graph, int v)
{
    // 打印当前顶点并标记为已访问
    printf("%c ", graph->vertex[v].data);
    visited[v] = 1;
    // 遍历当前顶点的邻接表
    ArcNode *arc = graph->vertex[v].firstarc;
    while (arc != NULL)
    {
        int adjVex = arc->adjvex;
        if (!visited[adjVex]) dfs(graph, adjVex);  // 递归访问相邻顶点
        arc = arc->nextarc;  // 继续下一个邻接顶点
    }
}
void bfs(AdjList *graph, int start)
{
    memset(visited,0,sizeof visited);
    printf("广度优先搜索遍历:\n");
    SeqQueue queue;
    InitQueue(&queue);
    Queue_Push(&queue, start);
    visited[start] = 1;
    while (!Queue_IsEmpty(&queue))
    {
        int v;
        Queue_Pop(&queue, &v);
        printf("%c ", graph->vertex[v].data);
        for (ArcNode *p = graph->vertex[v].firstarc; p != NULL; p = p->nextarc)
        {
            if (!visited[p->adjvex])
            {
                Queue_Push(&queue, p->adjvex);
                visited[p->adjvex] = 1;
            }
        }
    }
    printf("\n");
}
int main()
{
    AdjList g;
    build(&g);
    memset(visited, 0, sizeof(visited));
    printf("深度度优先搜索遍历：\n");
    for (int i=1;i<=g.vexnum;++i)
        if (!visited[i]) dfs(&g, i);
    printf("\n");
    bfs(&g,1);
    return 0;
}