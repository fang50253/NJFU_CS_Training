#include<stdio.h>
#include<string.h>
#include<stdlib.h>
// 定义队列相关常量与函数（需要queue.h文件）
#define QueueElementType int
#define maxsize 100
#include"queue.h"
#define MAX_VERTEX_NUM 20
#define INFNITY 32768
// 定义顶点数据类型
#define VertexData char
// 图的种类：DG-有向图，DN-有向网，UDG-无向图，UDN-无向网
typedef enum { DG, DN, UDG, UDN } GraphKind;
// 边结点结构定义
typedef struct ArcNode
{
    int adjvex;              // 该边指向的顶点下标
    int weight;              // 边的权重
    struct ArcNode *nextarc; // 指向下一个边结点
} ArcNode;
// 顶点结点结构定义
typedef struct VertexNode
{
    VertexData data;        // 顶点数据
    ArcNode *firstarc;      // 指向第一条依附该顶点的边
} VertexNode;
// 图的邻接表结构定义
typedef struct
{
    VertexNode vertex[MAX_VERTEX_NUM]; // 顶点数组
    int vexnum, arcnum;                // 顶点数，边数
    GraphKind kind;                    // 图的种类
} AdjList;
// Kruskal算法中使用的边结构
typedef struct
{
    int u, v;              // 边的两个顶点
    int weight;            // 边的权重
} Edge;
int visited[MAX_VERTEX_NUM]; // 访问标志数组
// 根据顶点数据查找其在图中的下标
int get_num(AdjList *graph, VertexData target)
{
    for (int i = 1; i <= graph->vexnum; ++i)
    {
        if (graph->vertex[i].data == target)
            return i;
    }
    return -1;
}
// 构建图的邻接表结构
void build(AdjList *graph)
{
    printf("请输入图的顶点数:");
    scanf("%d", &(graph->vexnum));

    printf("请输入图各个顶点的名称:(不要带有空格)");
    for (int i = 1; i <= graph->vexnum; ++i) {
        char input;
        do scanf("%c", &input); while (input == '\n'); // 过滤掉换行符
        graph->vertex[i].data = input;
        graph->vertex[i].firstarc = NULL; // 初始化每个顶点的边链表为空
    }
    printf("输入边的数量:");
    scanf("%d", &(graph->arcnum));
    printf("输入各个顶点之间的关系(边a) (边b) (边权):\n");
    for (int i = 1; i <= graph->arcnum; ++i) {
        char a, b;
        int c;
        scanf("\n%c %c %d", &a, &b, &c); // 输入两个顶点及其边的权重
        int a_num = get_num(graph, a);
        int b_num = get_num(graph, b);
        if (a_num == -1 || b_num == -1) {
            printf("输入数据错误,建图失败\n");
            return;
        } 
        // 创建从a到b的边
        ArcNode *new_node = (ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL)//处理内存分配失败的异常
        {
            printf("内存分配失败\n");
            return;
        }
        //建立第一条边
        new_node->nextarc = graph->vertex[a_num].firstarc;
        graph->vertex[a_num].firstarc = new_node;
        new_node->adjvex = b_num;
        new_node->weight = c;
        // 创建从b到a的边（因为是无向图）
        new_node = (ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL)
        {
            printf("内存分配失败\n");
            return;
        }
        //建立第二个边
        new_node->nextarc = graph->vertex[b_num].firstarc;
        graph->vertex[b_num].firstarc = new_node;
        new_node->adjvex = a_num;
        new_node->weight = c;
    }
}
// 深度优先搜索（DFS）遍历
void dfs(AdjList *graph, int v)
{
    printf("%c ", graph->vertex[v].data);
    // 输出当前顶点
    visited[v] = 1;
    // 标记当前顶点为已访问
    ArcNode *arc = graph->vertex[v].firstarc;
    while (arc != NULL)
    {
        int adjVex = arc->adjvex;
        if (!visited[adjVex]) dfs(graph, adjVex);
        // 递归访问相邻顶点
        arc = arc->nextarc;
        //还原标记
    }
}
// 广度优先搜索（BFS）遍历
void bfs(AdjList *graph, int start)
{
    memset(visited, 0, sizeof visited);
    // 重置访问标志数组
    printf("广度优先搜索遍历:\n");
    SeqQueue queue;
    //定义一个队列实现广搜
    InitQueue(&queue); // 初始化队列
    Queue_Push(&queue, start);
    visited[start] = 1;
    //第一个数据点开始
    while (!Queue_IsEmpty(&queue))
    {
        int v;
        Queue_Pop(&queue, &v); // 弹出队列头部元素
        printf("%c ", graph->vertex[v].data);
        for (ArcNode *p = graph->vertex[v].firstarc; p != NULL; p = p->nextarc)
        {
            if (!visited[p->adjvex])
            {
                Queue_Push(&queue, p->adjvex);
                //将这一层的数据推入队列
                visited[p->adjvex] = 1;
            }
        }
    }
    printf("\n");
}
// 并查集查找操作：查找元素的根节点
int find(int parent[], int i)
{
    while (parent[i] != i) i = parent[i];
    //实现路径压缩过程
    return i;
}
// 并查集合并操作：合并两个集合
void union_sets(int parent[], int rank[], int x, int y)
{
    int xroot = find(parent, x);
    int yroot = find(parent, y);
    parent[xroot] = yroot;
    //将两个集合合并
}
// Kruskal算法实现：计算最小生成树（MST）的边权之和
int kruskal(AdjList *graph)
{
    int parent[MAX_VERTEX_NUM];
    int rank[MAX_VERTEX_NUM];
    int edge_count = 0, min_cost = 0;
    Edge edges[MAX_VERTEX_NUM * MAX_VERTEX_NUM];
    //定义两个数组，用来存储边的信息，避免访问全局变量
    int edge_index = 0;
    // 初始化并查集
    for (int i = 1; i <= graph->vexnum; ++i)
    {
        parent[i] = i;
        rank[i] = 0;
        // 将图中的所有边存储到 edges 数组中
        for (ArcNode *arc = graph->vertex[i].firstarc; arc != NULL; arc = arc->nextarc)
        {
            edges[edge_index].u = i;
            edges[edge_index].v = arc->adjvex;
            edges[edge_index].weight = arc->weight;
            edge_index++;
        }
    }
    // 按边权升序排序
    for (int i = 0; i < edge_index - 1; i++)
    {
        for (int j = 0; j < edge_index - i - 1; j++)
        {
            if (edges[j].weight > edges[j + 1].weight)
            {
                Edge temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
    // Kruskal算法选择边构建最小生成树
    for (int i = 0; i < edge_index && edge_count < graph->vexnum - 1; i++)
    {
        int u = find(parent, edges[i].u);
        int v = find(parent, edges[i].v);
        if (u != v)
        {
            union_sets(parent, rank, u, v);
            min_cost += edges[i].weight;
            edge_count++;
            printf("选择的边: %c-%c, 权重: %d\n", graph->vertex[edges[i].u].data, graph->vertex[edges[i].v].data, edges[i].weight);
        }
    }
    //返回答案
    return min_cost;
}
int main()
{
    AdjList g;
    //初始化一张图
    build(&g);
    //输入图的数据
    memset(visited, 0, sizeof(visited));
    //清空已经被访问的数组
    printf("深度优先搜索遍历：\n");
    for (int i = 1; i <= g.vexnum; ++i)
        if (!visited[i]) dfs(&g, i);
    //这么做是为了防止出现第1个点不在图中，导致图无法被遍历
    //但事实上不会存在这种可能性，因为后面需要建立最小生成树
    //如果这个点不在途中，那么最小生成树一定不存子
    printf("\n");
    //深搜的结果已经输出，输出一个换行，接下来开始广搜
    bfs(&g, 1);
    printf("最小生成树序列：\n");
    int mst_cost = kruskal(&g);
    printf("最小生成树的边权之和：%d\n", mst_cost);
    //输出结果
    return 0;
}