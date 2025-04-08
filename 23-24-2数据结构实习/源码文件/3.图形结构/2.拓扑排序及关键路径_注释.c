#include <stdio.h>
#include <stdlib.h>
#define VertexData char           
// 定义顶点数据类型为字符
#define MAX_VERTEX_NUM 20         
// 最大顶点数量
#define INF 32767                 
// 表示无穷大，用于初始化最晚开始时间
// 边节点结构体
typedef struct ArcNode 
{
    int adjvex;                
    // 邻接顶点的索引
    int weight;                
    // 边的权重
    struct ArcNode *nextarc;   
    // 指向下一条边的指针
} ArcNode;
// 顶点节点结构体
typedef struct VertexNode 
{
    VertexData data;           
    // 顶点的数据（字符）
    ArcNode *firstarc;         
    // 指向第一条边的指针
} VertexNode;
// 邻接表结构体
typedef struct 
{
    VertexNode vertex[MAX_VERTEX_NUM];   
    // 顶点数组
    int vexnum, arcnum;         
    // 顶点数和边数
} AdjList;
// 获取指定顶点在图中的索引
int get_num(AdjList *graph, VertexData target) 
{
    for (int i = 1; i <= graph->vexnum; ++i) 
    {
        if (graph->vertex[i].data == target)
            return i;           
            // 找到目标顶点，返回索引
    }
    return -1;                  
    // 没找到目标顶点，返回-1
}

// 初始化图
void initGraph(AdjList *graph) 
{
    graph->vexnum = 0;          
    // 初始化顶点数为0
    graph->arcnum = 0;          
    // 初始化边数为0
}

// 添加边到图中
void addEdge(AdjList *graph, int from, int to, int weight) 
{
    ArcNode *newNode = (ArcNode*)malloc(sizeof(ArcNode));  
    // 为新边分配内存
    newNode->adjvex = to;       
    // 设置邻接顶点
    newNode->weight = weight;   
    // 设置边的权重
    newNode->nextarc = graph->vertex[from].firstarc;  
    // 将新边插入到当前顶点的边表中
    graph->vertex[from].firstarc = newNode;  
    // 更新顶点的第一条边
}
// 创建图
void buildGraph(AdjList *graph) 
{
    printf("请输入顶点数: ");
    scanf("%d", &(graph->vexnum));  
    // 输入顶点数量
    printf("请输入各个顶点的名称(不要带空格): ");
    for (int i = 0; i < graph->vexnum; i++) 
    {
        scanf(" %c", &(graph->vertex[i].data));  
        // 输入每个顶点的名称
        graph->vertex[i].firstarc = NULL;        
        // 初始化每个顶点的边表为空
    }
    printf("请输入边的数量: ");
    scanf("%d", &(graph->arcnum));  
    // 输入边的数量
    printf("请输入每条边的信息(起点 终点 权重):\n");
    for (int i = 0; i < graph->arcnum; i++) 
    {
        char u, v;
        int w;
        scanf(" %c %c %d", &u, &v, &w);  
        // 输入每条边的起点、终点和权重
        int from = -1, to = -1;
        for (int j = 0; j < graph->vexnum; j++) 
        {
            if (graph->vertex[j].data == u) from = j;  
            // 获取起点的索引
            if (graph->vertex[j].data == v) to = j;    
            // 获取终点的索引
        }
        if (from != -1 && to != -1) 
        {
            addEdge(graph, from, to, w);  
            // 如果起点和终点都存在，添加这条边
        }
    }
}
// 拓扑排序（Kahn算法）
void topologicalSort(AdjList *graph, int *topoOrder) 
{
    int inDegree[MAX_VERTEX_NUM] = {0};  
    // 初始化所有顶点的入度为0
    int queue[MAX_VERTEX_NUM], front = 0, rear = 0;  
    // 队列用于存储入度为0的顶点
    int count = 0;
    // 计算每个顶点的入度
    for (int i = 0; i < graph->vexnum; i++) 
    {
        ArcNode *p = graph->vertex[i].firstarc;
        while (p != NULL) 
        {
            inDegree[p->adjvex]++;  
            // 邻接顶点的入度加1
            p = p->nextarc;         
            // 遍历所有边
        }
    }
    // 将入度为0的顶点入队
    for (int i = 0; i < graph->vexnum; i++) 
    {
        if (inDegree[i] == 0) 
        {
            queue[rear++] = i;  
            // 入度为0，入队
        }
    }
    // 处理队列
    while (front < rear) 
    {
        int v = queue[front++];  
        // 出队一个顶点
        topoOrder[count++] = v;  
        // 将其加入拓扑排序结果
        ArcNode *p = graph->vertex[v].firstarc;
        while (p != NULL) 
        {
            inDegree[p->adjvex]--;  
            // 邻接顶点的入度减1
            if (inDegree[p->adjvex] == 0) 
            {
                queue[rear++] = p->adjvex;  
                // 如果入度为0，入队
            }
            p = p->nextarc;  
            // 继续处理下一个邻接顶点
        }
    }
    // 检查是否存在环
    if (count < graph->vexnum) 
    {
        printf("图中存在环，无法进行拓扑排序。\n");
    } 
    else 
    {
        printf("拓扑排序结果: ");
        for (int i = 0; i < count; i++) 
        {
            printf("%c ", graph->vertex[topoOrder[i]].data);  
            // 输出拓扑排序结果
        }
        printf("\n");
    }
}
// 查找关键路径
void findCriticalPath(AdjList *graph) 
{
    int topoOrder[MAX_VERTEX_NUM];
    int earliest[MAX_VERTEX_NUM] = {0};  
    // 初始化最早开始时间为0
    int latest[MAX_VERTEX_NUM];          
    // 最晚开始时间数组
    for (int i = 0; i < graph->vexnum; i++) 
    {
        latest[i] = INF;  
        // 初始化最晚开始时间为无穷大
    }
    topologicalSort(graph, topoOrder);  
    // 对图进行拓扑排序
    // 计算最早开始时间
    for (int i = 0; i < graph->vexnum; i++) 
    {
        int u = topoOrder[i];
        ArcNode *p = graph->vertex[u].firstarc;
        while (p != NULL) 
        {
            int v = p->adjvex;
            if (earliest[u] + p->weight > earliest[v]) 
            {
                earliest[v] = earliest[u] + p->weight;  
                // 更新最早开始时间
            }
            p = p->nextarc;
        }
    }
    // 计算最晚开始时间
    for (int i = graph->vexnum - 1; i >= 0; i--) 
    {
        int u = topoOrder[i];
        ArcNode *p = graph->vertex[u].firstarc;
        while (p != NULL) 
        {
            int v = p->adjvex;
            if (latest[v] - p->weight < latest[u]) 
            {
                latest[u] = latest[v] - p->weight;  
                // 更新最晚开始时间
            }
            p = p->nextarc;
        }
    }
    // 输出最长路径长度
    printf("最长路径长度: %d\n", earliest[topoOrder[graph->vexnum - 1]]);
    printCriticalPath(graph, topoOrder, earliest, latest, earliest[topoOrder[graph->vexnum - 1]]);
}

// 输出关键路径
void printCriticalPath(AdjList *graph, int *topoOrder, int *earliest, int *latest, int ans) 
{
    printf("关键路径:\n");
    for (int i = 0; i < graph->vexnum; i++) 
    {
        int u = topoOrder[i];
        ArcNode *p = graph->vertex[u].firstarc;
        while (p != NULL) 
        {
            int v = p->adjvex;
            int e = earliest[u];     
            // 当前顶点的最早开始时间
            int l = latest[v] - p->weight - INF + ans; 
            // 邻接顶点的最晚开始时间减去边的权重
            if (e == l) 
                printf("%c->%c \n", graph->vertex[u].data, graph->vertex[v].data);  
                // 如果最早和最晚开始时间相等，则是关键路径
            p = p->nextarc;
        }
    }
    printf("\n");
}
int main() 
{
    AdjList graph;
    initGraph(&graph);
    buildGraph(&graph);
    findCriticalPath(&graph);
    return 0;
}