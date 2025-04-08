#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define QueueElementType int
#define maxsize 100
#include"Queue.h"
#define MAX_VERTEX_NUM 20
#define INFNITY 32768
#define VertexData char
typedef enum { DG, DN, UDG, UDN } GraphKind;
int visited[maxsize];
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
int get_num(AdjList *graph, VertexData target) {
    for (int i = 1; i <= graph->vexnum; ++i) {
        if (graph->vertex[i].data == target)
            return i;
    }
    return -1;
}
// 函数原型声明
void build(AdjList *graph);
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

// Dijkstra 算法，计算源点 source 到目标点 target 的最短路径
void dijkstra(AdjList *graph, char t1, char t2) {


    int source=get_num(graph,t1);
    int target=get_num(graph,t2);
    int dist[MAX_VERTEX_NUM];  // 存储源点到各个顶点的最短路径
    int path[MAX_VERTEX_NUM];  // 存储最短路径的前驱节点
    int visited[MAX_VERTEX_NUM];  // 标记顶点是否已访问

    // 初始化
    for (int i = 1; i <= graph->vexnum; ++i) {
        dist[i] = INFNITY; // 设置初始距离为无穷大
        path[i] = -1;      // 初始前驱节点为-1
        visited[i] = 0;    // 所有顶点均未访问
    }
    dist[source] = 0;  // 源点到自己的距离为0

    // 执行 Dijkstra 算法
    for (int i = 1; i <= graph->vexnum; ++i) {
        // 找到未访问的顶点中距离源点最近的顶点
        int u = -1;
        for (int j = 1; j <= graph->vexnum; ++j) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }
        
        if (dist[u] == INFNITY) break; // 剩余顶点不可达

        visited[u] = 1; // 标记为已访问

        // 更新顶点 u 的邻接点的距离
        for (ArcNode *arc = graph->vertex[u].firstarc; arc != NULL; arc = arc->nextarc) {
            int v = arc->adjvex;
            if (!visited[v] && dist[u] + arc->weight < dist[v]) {
                dist[v] = dist[u] + arc->weight;
                path[v] = u; // 更新前驱节点
            }
        }
    }

    // 输出源点到目标点的最短路径及其距离
    if (dist[target] == INFNITY) {
        printf("从顶点 %d 到顶点 %d 没有路径\n", source, target);
    } else {
        printf("从顶点 %d 到顶点 %d 的最短路径长度是 %d\n", source, target, dist[target]);
        
        // 输出路径
        printf("路径: ");
        int node = target;
        while (node != -1) {
            printf("%c ",graph->vertex[node]);
            node = path[node];
        }
        printf("\n");
    }
}

int main() {
    AdjList g;
    build(&g);

    // 选择源点和目标点进行测试
    int source, target;
    printf("请输入源点: ");
    do scanf("%c", &source);while(source=='\n');
    printf("请输入目标点: ");
    do scanf("%c", &target);while(target=='\n');

    dijkstra(&g, source, target);

    return 0;
}