#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define QueueElementType int
#define maxsize 100
#include"queue.h"
#define MAX_VERTEX_NUM 20
#define INFNITY 32768
#define VertexData char
typedef enum { DG, DN, UDG, UDN } GraphKind;
// DG有向图，DN有向网，UDG无向图，UDN无向网
typedef struct ArcNode
{
    int adjvex;
    int weight;
    struct ArcNode *nextarc;
} ArcNode;
typedef struct VertexNode
{
    VertexData data;
    ArcNode *firstarc;
} VertexNode;

typedef struct {
    VertexNode vertex[MAX_VERTEX_NUM];
    int vexnum, arcnum; // 图的顶点数，边数
    GraphKind kind;
} AdjList;

typedef struct {
    int u, v;
    int weight;
} Edge;

int visited[MAX_VERTEX_NUM];

int get_num(AdjList *graph, VertexData target) {
    for (int i = 1; i <= graph->vexnum; ++i) {
        if (graph->vertex[i].data == target)
            return i;
    }
    return -1;
}

void build(AdjList *graph) {
    printf("请输入图的顶点数:");
    scanf("%d", &(graph->vexnum));
    printf("请输入图各个顶点的名称:(不要带有空格)");
    for (int i = 1; i <= graph->vexnum; ++i) {
        char input;
        do scanf("%c", &input); while (input == '\n');
        graph->vertex[i].data = input;
        graph->vertex[i].firstarc = NULL;
    }
    printf("输入边的数量:");
    scanf("%d", &(graph->arcnum));
    printf("输入各个顶点之间的关系(边a) (边b) (边权):\n");
    for (int i = 1; i <= graph->arcnum; ++i) {
        char a, b;
        int c;
        scanf("\n%c %c %d", &a, &b, &c);
        int a_num = get_num(graph, a);
        int b_num = get_num(graph, b);
        if (a_num == -1 || b_num == -1) {
            printf("输入数据错误,建图失败\n");
            return;
        }
        ArcNode *new_node = (ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL) {
            printf("内存分配失败\n");
            return;
        }
        new_node->nextarc = graph->vertex[a_num].firstarc;
        graph->vertex[a_num].firstarc = new_node;
        new_node->adjvex = b_num;
        new_node->weight = c;
        new_node = (ArcNode*)malloc(sizeof(ArcNode));
        if (new_node == NULL) {
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
void dfs(AdjList *graph, int v) {
    // 打印当前顶点并标记为已访问
    printf("%c ", graph->vertex[v].data);
    visited[v] = 1;
    // 遍历当前顶点的邻接表
    ArcNode *arc = graph->vertex[v].firstarc;
    while (arc != NULL) {
        int adjVex = arc->adjvex;
        if (!visited[adjVex]) dfs(graph, adjVex);  // 递归访问相邻顶点
        arc = arc->nextarc;  // 继续下一个邻接顶点
    }
}

void bfs(AdjList *graph, int start) {
    memset(visited, 0, sizeof visited);
    printf("广度优先搜索遍历:\n");
    SeqQueue queue;
    InitQueue(&queue);
    Queue_Push(&queue, start);
    visited[start] = 1;
    while (!Queue_IsEmpty(&queue)) {
        int v;
        Queue_Pop(&queue, &v);
        printf("%c ", graph->vertex[v].data);
        for (ArcNode *p = graph->vertex[v].firstarc; p != NULL; p = p->nextarc) {
            if (!visited[p->adjvex]) {
                Queue_Push(&queue, p->adjvex);
                visited[p->adjvex] = 1;
            }
        }
    }
    printf("\n");
}

// 辅助函数：查找并查集的根节点
int find(int parent[], int i) {
    while (parent[i] != i) {
        i = parent[i];
    }
    return i;
}

// 辅助函数：合并两个集合
void union_sets(int parent[], int rank[], int x, int y)
{
    int xroot = find(parent, x);
    int yroot = find(parent, y);
    parent[xroot] = yroot;
}

// 计算最小生成树的边权之和
int kruskal(AdjList *graph) {
    int parent[MAX_VERTEX_NUM];
    int rank[MAX_VERTEX_NUM];
    int edge_count = 0, min_cost = 0;

    Edge edges[MAX_VERTEX_NUM * MAX_VERTEX_NUM];
    int edge_index = 0;

    // 初始化并查集
    for (int i = 1; i <= graph->vexnum; ++i) {
        parent[i] = i;
        rank[i] = 0;

        // 将图中的所有边存储到 edges 数组中
        for (ArcNode *arc = graph->vertex[i].firstarc; arc != NULL; arc = arc->nextarc) {
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
            printf("u,v:%d %d %d\n",edges[i].u,edges[i].v,edges[i].weight);
        }
    }
    return min_cost;
}
int main()
{
    AdjList g;
    build(&g);
    memset(visited, 0, sizeof(visited));
    printf("深度优先搜索遍历：\n");
    for (int i = 1; i <= g.vexnum; ++i)
        if (!visited[i]) dfs(&g, i);
    printf("\n");
    bfs(&g, 1);
    printf("最小生成树序列：\n");
    int mst_cost = kruskal(&g);
    printf("最小生成树的边权之和：%d\n", mst_cost);
    return 0;
}