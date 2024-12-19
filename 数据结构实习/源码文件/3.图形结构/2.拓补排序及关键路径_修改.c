#include <stdio.h>
#include <stdlib.h>
#define VertexData char
#define MAX_VERTEX_NUM 20
#define INF 32767

typedef struct ArcNode {
    int adjvex;                // The index of the adjacent vertex
    int weight;                // The weight of the edge
    struct ArcNode *nextarc;   // Pointer to the next adjacent vertex
} ArcNode;

typedef struct VertexNode {
    VertexData data;                 // Vertex data
    ArcNode *firstarc;         // Pointer to the first adjacent vertex
} VertexNode;

typedef struct {
    VertexNode vertex[MAX_VERTEX_NUM];
    int vexnum, arcnum;         // Number of vertices and edges
} AdjList;
int get_num(AdjList *graph, VertexData target) {
    for (int i = 1; i <= graph->vexnum; ++i) {
        if (graph->vertex[i].data == target)
            return i;
    }
    return -1;
}
// Function to initialize the graph
void initGraph(AdjList *graph) {
    graph->vexnum = 0;
    graph->arcnum = 0;
}

// Function to add an edge to the graph
void addEdge(AdjList *graph, int from, int to, int weight) {
    ArcNode *newNode = (ArcNode*)malloc(sizeof(ArcNode));
    newNode->adjvex = to;
    newNode->weight = weight;
    newNode->nextarc = graph->vertex[from].firstarc;
    graph->vertex[from].firstarc = newNode;
}

// Function to create the graph
// Function to print the critical path
void printCriticalPath(AdjList *graph, int *topoOrder, int *earliest, int *latest,int ans) {
    printf("关键路径:\n");
    for (int i = 0; i < graph->vexnum; i++) {
        int u = topoOrder[i];
        ArcNode *p = graph->vertex[u].firstarc;
        while (p != NULL) {
            int v = p->adjvex;
            int e = earliest[u];     // Earliest start time of u
            int l = latest[v] - p->weight-INF+ans; // Latest start time of v - weight of the edge (u, v)
            //printf("st:%d->%d:",e,l);
            if (e == l) 
                printf("%c->%c \n", graph->vertex[u].data, graph->vertex[v].data);
            
            p = p->nextarc;
        }
    }
    printf("\n");
}
void buildGraph(AdjList *graph) {
    printf("请输入顶点数: ");
    scanf("%d", &(graph->vexnum));
    printf("请输入各个顶点的名称(不要带空格): ");
    for (int i = 0; i < graph->vexnum; i++) {
        scanf(" %c", &(graph->vertex[i].data));
        graph->vertex[i].firstarc = NULL;
    }
    printf("请输入边的数量: ");
    scanf("%d", &(graph->arcnum));
    printf("请输入每条边的信息(起点 终点 权重):\n");
    for (int i = 0; i < graph->arcnum; i++) {
        char u, v;
        int w;
        scanf(" %c %c %d", &u, &v, &w);
        int from = -1, to = -1;
        for (int j = 0; j < graph->vexnum; j++) {
            if (graph->vertex[j].data == u) from = j;
            if (graph->vertex[j].data == v) to = j;
        }
        if (from != -1 && to != -1) {
            addEdge(graph, from, to, w);
        }
    }
}

// Function for topological sorting using Kahn's algorithm
void topologicalSort(AdjList *graph, int *topoOrder) {
    int inDegree[MAX_VERTEX_NUM] = {0};
    int queue[MAX_VERTEX_NUM], front = 0, rear = 0;
    int count = 0;

    // Calculate in-degree of each vertex
    for (int i = 0; i < graph->vexnum; i++) {
        ArcNode *p = graph->vertex[i].firstarc;
        while (p != NULL) {
            inDegree[p->adjvex]++;
            p = p->nextarc;
        }
    }

    // Enqueue vertices with in-degree 0
    for (int i = 0; i < graph->vexnum; i++) {
        if (inDegree[i] == 0) {
            queue[rear++] = i;
        }
    }

    // Process the queue
    while (front < rear) {
        int v = queue[front++];
        topoOrder[count++] = v;
        ArcNode *p = graph->vertex[v].firstarc;
        while (p != NULL) {
            inDegree[p->adjvex]--;
            if (inDegree[p->adjvex] == 0) {
                queue[rear++] = p->adjvex;
            }
            p = p->nextarc;
        }
    }

    if (count < graph->vexnum) {
        printf("图中存在环，无法进行拓扑排序。\n");
    } else {
        printf("拓扑排序结果: ");
        for (int i = 0; i < count; i++) {
            printf("%c ", graph->vertex[topoOrder[i]].data);
        }
        printf("\n");
    }
}

// Function to find the critical path
void findCriticalPath(AdjList *graph) {
    int topoOrder[MAX_VERTEX_NUM];
    int earliest[MAX_VERTEX_NUM] = {0};
    int latest[MAX_VERTEX_NUM];
    for (int i = 0; i < graph->vexnum; i++) {
        latest[i] = INF;
    }

    topologicalSort(graph, topoOrder);

    // Calculate earliest start times
    for (int i = 0; i < graph->vexnum; i++) {
        int u = topoOrder[i];
        ArcNode *p = graph->vertex[u].firstarc;
        while (p != NULL) {
            int v = p->adjvex;
            if (earliest[u] + p->weight > earliest[v]) {
                earliest[v] = earliest[u] + p->weight;
            }
            p = p->nextarc;
        }
    }

    // Calculate latest start times
    for (int i = graph->vexnum - 1; i >= 0; i--) {
        int u = topoOrder[i];
        ArcNode *p = graph->vertex[u].firstarc;
        while (p != NULL) {
            int v = p->adjvex;
            if (latest[v] - p->weight < latest[u]) {
                latest[u] = latest[v] - p->weight;
            }
            p = p->nextarc;
        }
    }
    printf("最长路径长度: %d\n", earliest[topoOrder[graph->vexnum - 1]]);
    printCriticalPath(graph, topoOrder, earliest, latest,earliest[topoOrder[graph->vexnum - 1]]);
}

int main() {
    AdjList graph;
    initGraph(&graph);
    buildGraph(&graph);
    findCriticalPath(&graph);

    return 0;
}