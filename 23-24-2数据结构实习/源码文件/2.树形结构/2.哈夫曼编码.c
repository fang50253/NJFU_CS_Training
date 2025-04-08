#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 定义二叉树节点结构
typedef struct BiTree
{
    char data; 
    // 节点数据，字符
    int weight; 
    // 节点权重
    struct BiTree *lchild, *rchild; 
    // 左右子树指针
} BiTree;
// 定义哈夫曼编码表的结构
typedef struct 
{
    char data;
    char code[100];
} HuffmanCode;
// 创建新的树节点
BiTree* createNode(char data, int weight) 
{
    BiTree *node = (BiTree *)malloc(sizeof(BiTree));
    node->data = data;
    node->weight = weight;
    node->lchild = node->rchild = NULL;
    return node;
}
// 合并两个节点为一个新的父节点
BiTree* merge(BiTree *left, BiTree *right) 
{
    BiTree *node = createNode('\0', left->weight + right->weight);
    node->lchild = left;
    node->rchild = right;
    return node;
}
// 比较函数，用于按权重排序
int compare(const void *a, const void *b) 
{
    return (*(BiTree **)a)->weight - (*(BiTree **)b)->weight;
}
// 生成哈夫曼树
BiTree* create(BiTree *nodes[], int n) 
{
    while (n > 1) 
    {
        qsort(nodes, n, sizeof(BiTree *), compare); // 按权重排序
        BiTree *left = nodes[0];
        BiTree *right = nodes[1];
        BiTree *newNode = merge(left, right);
        nodes[1] = newNode;
        nodes++; // 缩小范围
        n--;
    }
    return nodes[0]; // 返回哈夫曼树的根节点
}
// 递归生成哈夫曼编码
void build(BiTree *root, char *code, int length, HuffmanCode huffCodes[], int *idx) 
{
    if (root->lchild == NULL && root->rchild == NULL) 
    {
        code[length] = '\0';
        huffCodes[*idx].data = root->data;
        strcpy(huffCodes[*idx].code, code);
        (*idx)++;
        return;
    }
    if (root->lchild) 
    {
        code[length] = '0';
        build(root->lchild, code, length + 1, huffCodes, idx);
    }
    if (root->rchild) 
    {
        code[length] = '1';
        build(root->rchild, code, length + 1, huffCodes, idx);
    }
}
// 根据输入字符串生成哈夫曼编码
void encode(char *str, HuffmanCode huffCodes[], int n) 
{
    for (int i = 0; str[i] != '\0'; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            if (str[i] == huffCodes[j].data) 
            {
                printf("%s", huffCodes[j].code);
                break;
            }
        }
    }
    printf("\n");
}
// 根据哈夫曼编码解码为字符串
void decode(char *encodedStr, BiTree *root) 
{
    BiTree *current = root;
    for (int i = 0; encodedStr[i] != '\0'; i++) 
    {
        if (encodedStr[i] == '0')
            current = current->lchild;
        else if (encodedStr[i] == '1')
            current = current->rchild;

        if (current->lchild == NULL && current->rchild == NULL) 
        {
            printf("%c", current->data);
            current = root;
        }
    }
    printf("\n");
}
void menu() 
{
    printf("输入结点的个数：");
    int len;
    scanf("%d", &len);
    // 动态分配节点数组
    BiTree *nodes[len];
    printf("输入字符串内容+频度：\n");
    for (int i = 0; i < len; ++i) 
    {
        char str;
        int fre;
        scanf(" %c %d", &str, &fre);
        nodes[i] = createNode(str, fre); // 创建叶子节点
    }
    // 构建哈夫曼树
    BiTree *root = create(nodes, len);
    // 生成哈夫曼编码
    HuffmanCode huffCodes[len];
    char code[100];
    int idx = 0;
    build(root, code, 0, huffCodes, &idx);
    printf("生成的哈夫曼编码:\n");
    for (int i = 0; i < len; i++) 
    {
        printf("%c: %s\n", huffCodes[i].data, huffCodes[i].code);
    }
    // 菜单选项
    int choice;
    do 
    {
        printf("\n请选择操作:\n");
        printf("1. 编码字符串\n");
        printf("2. 解码字符串\n");
        printf("3. 退出\n");
        printf("输入您的选择: ");
        scanf("%d", &choice);
        if (choice == 1) 
        {
            char str[100];
            printf("输入要编码的字符串: ");
            scanf("%s", str);
            printf("编码结果: ");
            encode(str, huffCodes, len);

        } 
        else if (choice == 2) 
        {
            char encodedStr[100];
            printf("输入要解码的哈夫曼编码: ");
            scanf("%s", encodedStr);
            printf("解码结果: ");
            decode(encodedStr, root);

        } 
        else if (choice != 3) 
        {
            printf("无效的选择，请重新输入。\n");
        }
    }while (choice != 3);
    printf("已退出程序。\n");
}
int main()
{
    menu();
    return 0;
}