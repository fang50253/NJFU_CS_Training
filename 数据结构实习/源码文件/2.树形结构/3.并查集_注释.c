#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100
// 定义集合中的节点结构
typedef struct Node
{
    int data;           
    // 节点数据，即元素值
    struct Node* parent; 
    // 指向父节点，用于路径压缩
} Node;
Node* nodes[MAX_SIZE];  
// 定义一个全局数组，用于存储集合中的所有节点
// 初始化集合中的单个元素，使其自成一个集合
void InitSet(int x) {
    nodes[x] = (Node*)malloc(sizeof(Node));  
    // 动态分配内存给节点
    nodes[x]->data = x;                      
    // 设置节点的数据为元素值
    nodes[x]->parent = nodes[x];             
    // 初始时，每个节点的父节点是它自己
    nodes[x]->rank = 0;                      
    // 初始时，节点的秩为0
}
// 查找元素所在集合的代表元（根节点），带路径压缩
Node* findSet(Node* node)
{
    if (node->parent != node) 
    {
        // 如果节点的父节点不是它自己，递归查找根节点，并进行路径压缩
        node->parent = findSet(node->parent);
    }
    return node->parent;  
    // 返回根节点
}

// 合并两个集合，按秩合并
void unionSets(int x, int y) {
    Node* rootX = findSet(nodes[x]);  
    // 查找x所在集合的根节点
    Node* rootY = findSet(nodes[y]);  
    // 查找y所在集合的根节点
    // 如果两个元素的根节点不同，进行合并操作
    if (rootX != rootY)
    {
        rootY->parent = rootX;  
        // 将y的根节点的父节点设置为x的根节点，表示两个集合合并
    }
}
// 输出集合的存储结构
void printSets(int n) 
{
    for (int i = 0; i < n; i++) 
    {
        Node* root = findSet(nodes[i]);  
        // 查找每个元素所在集合的根节点
        printf("Element %d -> Set Representative %d\n", i, root->data);  
        // 输出每个元素及其集合代表元
    }
}
int main() 
{
    int n, ch, x, y;
    printf("请输入集合中元素的数量: ");
    scanf("%d", &n);  
    // 输入集合中元素的数量
    while (1) 
    {
        // 显示操作菜单
        printf("\n1. 建立集合\n");
        printf("2. 查找(判定)元素所属集合\n");
        printf("3. 集合的并运算\n");
        printf("0. 退出\n");
        printf("请选择 0-3: ");
        scanf("%d", &ch);
        if (ch == 1) 
        {
            // 建立集合：对每个元素调用InitSet函数，初始化集合
            for (int i = 0; i < n; i++) InitSet(i);
        }
        else if (ch == 2) 
        {
            // 查找元素所属集合的代表元
            printf("请输入要查找的元素: ");
            scanf("%d", &x);
            printf("元素 %d 所属集合的代表元是: %d\n", x, findSet(nodes[x])->data);
        }
        else if (ch == 3) 
        {
            // 集合的并运算：将两个元素所在的集合合并
            printf("请输入要联合的两个元素: ");
            scanf("%d %d", &x, &y);
            unionSets(x, y);
        } 
        else if (ch == 0) 
        {
            // 退出程序
            return 0;
        }
    }
    return 0;
}