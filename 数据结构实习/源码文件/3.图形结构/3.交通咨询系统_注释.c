#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define QueueElementType int
// 定义队列元素的类型为整型
#define maxsize 100
// 定义队列的最大尺寸
#include "Queue.h"
// 引入自定义的循环队列头文件
#define MAX_VERTEX_NUM 20
// 图的最大顶点数量
#define INFNITY 32768
// 定义一个表示无穷大的值
#define VertexData char
// 顶点的数据类型为字符类型
typedef enum { DG, DN, UDG, UDN } GraphKind;
// 图的类型：DG有向图，DN有向网，UDG无向图，UDN无向网
int visited[maxsize];
// 访问标记数组
// 图的邻接表中边的结点定义
typedef struct ArcNode
{
    int adjvex; // 该边所指向的顶点的位置
    int weight; // 边的权重
    struct ArcNode *nextarc; // 指向下一条边的指针
} ArcNode;
// 图的邻接表中顶点的结点定义
typedef struct VertexNode
{
    VertexData data; // 顶点的数据
    ArcNode *firstarc; // 指向该顶点第一条边的指针
} VertexNode;
// 图的邻接表定义
typedef struct
{
    VertexNode vertex[MAX_VERTEX_NUM]; // 顶点数组
    int vexnum, arcnum; // 图的顶点数和边数
    GraphKind kind; // 图的类型
} AdjList;
// 边的信息结构体，用于存储边的两个顶点和权重
typedef struct
{
    int u, v;
    int weight;
} Edge;
// 根据顶点数据查找其在图中的索引
int get_num(AdjList *graph, VertexData target)
{
    for (int i = 1; i <= graph->vexnum; ++i)
    {
        if (graph->vertex[i].data == target)
            return i;
    }
    return -1; // 如果找不到则返回-1
}
// 函数原型声明
// 构建图的邻接表
void build(AdjList *graph)
{
    printf("请输入图的顶点数:");
    scanf("%d", &(graph->vexnum));
    printf("请输入图各个顶点的名称:(不要带有空格)");
    for (int i = 1; i <= graph->vexnum; ++i) 
    {
        char input;
        do scanf("%c", &input); while (input == '\n'); 
        // 读取顶点名称
        graph->vertex[i].data = input;
        graph->vertex[i].firstarc = NULL; 
        // 初始化边表为空
    }
    printf("输入边的数量:");
    scanf("%d", &(graph->arcnum));
    printf("输入各个顶点之间的关系(边a) (边b) (边权):\n");
    for (int i = 1; i <= graph->arcnum; ++i) {
        char a, b;
        int c;
        scanf("\n%c %c %d", &a, &b, &c); 
        // 读取边的起点、终点和权重
        int a_num = get_num(graph, a);
        int b_num = get_num(graph, b);
        if (a_num == -1 || b_num == -1)
        {
            printf("输入数据错误,建图失败\n");
            return;
        }
        // 为起点a添加边
        ArcNode *new_node = (ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL)
        {
            printf("内存分配失败\n");
            return;
        }
        new_node->nextarc = graph->vertex[a_num].firstarc;
        graph->vertex[a_num].firstarc = new_node;
        new_node->adjvex = b_num;
        new_node->weight = c;
        // 为终点b添加边（如果是无向图）
        new_node = (ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL) 
        {
            printf("内存分配失败\n");
            return;
        }
        new_node->nextarc = graph->vertex[b_num].firstarc;
        graph->vertex[b_num].firstarc = new_node;
        new_node->adjvex = a_num;
        new_node->weight = c;
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
// 广度优先搜索
void bfs(AdjList *graph, int start) 
{
    memset(visited, 0, sizeof visited); // 初始化访问标记
    printf("广度优先搜索遍历:\n");
    SeqQueue queue; // 声明队列
    InitQueue(&queue); // 初始化队列
    Queue_Push(&queue, start); // 起点入队
    visited[start] = 1;
    while (!Queue_IsEmpty(&queue)) 
    {
        int v;
        Queue_Pop(&queue, &v); // 出队并访问
        printf("%c ", graph->vertex[v].data);
        // 遍历邻接表
        for (ArcNode *p = graph->vertex[v].firstarc; p != NULL; p = p->nextarc) 
        {
            if (!visited[p->adjvex]) 
            {
                Queue_Push(&queue, p->adjvex); // 未访问的邻接点入队
                visited[p->adjvex] = 1; // 标记为已访问
            }
        }
    }
    printf("\n");
}
// Dijkstra 算法，计算源点 source 到目标点 target 的最短路径
void dijkstra(AdjList *graph, char t1, char t2)
{
    int source=get_num(graph,t1);
    int target=get_num(graph,t2);
    int dist[MAX_VERTEX_NUM];  
    // 存储源点到各个顶点的最短路径
    int path[MAX_VERTEX_NUM];  
    // 存储最短路径的前驱节点
    int visited[MAX_VERTEX_NUM];  
    // 标记顶点是否已访问
    // 初始化
    for (int i = 1; i <= graph->vexnum; ++i) 
    {
        dist[i] = INFNITY; 
        // 设置初始距离为无穷大
        path[i] = -1;      
        // 初始前驱节点为-1
        visited[i] = 0;   
        // 所有顶点均未访问
    }
    dist[source] = 0;  
    // 源点到自己的距离为0
    // 执行 Dijkstra 算法
    for (int i = 1; i <= graph->vexnum; ++i) 
    {
        // 找到未访问的顶点中距离源点最近的顶点
        int u = -1;
        for (int j = 1; j <= graph->vexnum; ++j) 
        {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) u=j;
        }
        if (dist[u] == INFNITY) break; 
        // 剩余顶点不可达
        visited[u] = 1; // 标记为已访问
        // 更新顶点 u 的邻接点的距离
        for (ArcNode *arc = graph->vertex[u].firstarc; arc != NULL; arc = arc->nextarc) 
        {
            int v = arc->adjvex;
            if (!visited[v] && dist[u] + arc->weight < dist[v]) 
            {
                dist[v] = dist[u] + arc->weight;
                path[v] = u; // 更新前驱节点
            }
        }
    }
    // 输出源点到目标点的最短路径及其距离
    if (dist[target] == INFNITY) printf("从顶点 %d 到顶点 %d 没有路径\n", source, target);
    else
    {
        printf("从顶点 %d 到顶点 %d 的最短路径长度是 %d\n", source, target, dist[target]);
        // 输出路径
        printf("路径: ");
        int node = target;
        while (node != -1) 
        {
            printf("%c ",graph->vertex[node].data);
            node = path[node];
        }
        printf("\n");
    }
}
// Floyd-Warshall 算法，计算所有顶点对之间的最短路径
void floyd(AdjList *graph) 
{
    int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM]; // 存储顶点之间的最短路径距离
    int i, j, k;
    // 初始化距离矩阵
    for (i = 1; i <= graph->vexnum; ++i) 
    {
        for (j = 1; j <= graph->vexnum; ++j) 
        {
            if (i == j) dist[i][j] = 0; // 自己到自己的距离为0
            else dist[i][j] = INFNITY; // 初始化为无穷大
        }
    }
    // 根据图的邻接表设置初始距离
    for (i = 1; i <= graph->vexnum; ++i) 
    {
        for (ArcNode *arc = graph->vertex[i].firstarc; arc != NULL; arc = arc->nextarc) 
        {
            dist[i][arc->adjvex] = arc->weight; // 邻接点的距离即为边的权重
        }
    }
    // Floyd-Warshall 核心算法，动态规划
    for (k = 1; k <= graph->vexnum; ++k) 
    {
        for (i = 1; i <= graph->vexnum; ++i) 
        {
            for (j = 1; j <= graph->vexnum; ++j) 
            {
                if (dist[i][k] + dist[k][j] < dist[i][j]) 
                {
                    dist[i][j] = dist[i][k] + dist[k][j]; // 更新最短路径
                }
            }
        }
    }
    // 输出结果矩阵
    printf("Floyd-Warshall 结果矩阵:\n");
    for (i = 1; i <= graph->vexnum; ++i) 
    {
        for (j = 1; j <= graph->vexnum; ++j) 
        {
            if (dist[i][j] == INFNITY) printf("INF ");
            else printf("%3d ", dist[i][j]);
        }
        printf("\n");
    }
}
int main() 
{
    AdjList g;
    build(&g);
    // 选择源点和目标点进行测试
    int source, target;
    printf("请输入源点: ");
    do scanf("%c", &source);while(source=='\n');
    printf("请输入目标点: ");
    do scanf("%c", &target);while(target=='\n');
    dijkstra(&g, source, target);
    printf("\n以下是floyd的输出结果\n");
    floyd(&g);
    return 0;
}