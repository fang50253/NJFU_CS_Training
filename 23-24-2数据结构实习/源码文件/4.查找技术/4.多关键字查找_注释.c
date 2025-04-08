#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CLS "clear"
// 在Windows系统中改成"cls"，在其他系统中使用"clear"
const int MAXN = 1e3 + 10;
// 数据库最大记录数
int data_open; 
// 日志模式开关，指示是否启用调试模式
int idx = 0; 
// 当前记录数
// 数据库结构体定义
struct DataBase
{
    char num[30]; // 学号
    char name[30]; // 姓名
    int score; // 总成绩
} info[MAXN]; // 数据库数组
// 函数声明
int read(); 
// 读取数据函数
int write(); 
// 写入数据函数
int query_score_by_num(char num[]); 
// 根据学号查询成绩
void query_score_output(); 
// 查询成绩的前端处理函数
void main_tab(); 
// 主菜单函数
int add_record(char num[], char name[], int score); 
// 添加记录函数
// 读取数据函数
int read() 
{
    FILE *fp = fopen("data.fzy", "r"); 
    // 打开数据文件以只读模式
    if (fp == NULL) return 0; 
    // 如果文件打开失败，可能是首次运行
    fscanf(fp, "%d", &data_open); 
    // 读取日志模式开关状态
    char num[100], name[100]; 
    // 临时缓冲区
    int score; // 临时缓冲区
    memset(info, 0, sizeof(info)); 
    // 清空原有数据
    idx = 0; // 重置记录数
    // 从文件中读取数据并存入info数组
    while (fscanf(fp, "%s %s %d", num, name, &score) == 3) 
    {
        ++idx;
        if (idx >= MAXN - 10) printf("Error, 数据量过大\n"); 
        // 数据量过多时给出提示
        strcpy(info[idx].num, num); 
        // 复制学号
        strcpy(info[idx].name, name); 
        // 复制姓名
        info[idx].score = score; 
        // 复制成绩
    }
    fclose(fp); 
    // 关闭文件
    return 0;
}

// 写入数据函数
int write() {
    FILE *fp = fopen("data.fzy", "w+"); 
    // 打开数据文件以写模式
    fprintf(fp, "%d\n", data_open); 
    // 写入日志模式开关状态
    // 将所有记录写入文件
    for (int i = 1; i <= idx; ++i) 
    {
        fprintf(fp, "%s %s %d\n", info[i].num, info[i].name, info[i].score);
    }
    fclose(fp); // 关闭文件
    printf("数据保存成功\n"); 
    // 提示保存成功
    return 0;
}

// 根据学号查询成绩
int query_score_by_num(char num[]) 
{
    for (int i = 1; i <= idx; ++i) 
    {
        if (strcmp(info[i].num, num) == 0) 
        {
            return i; 
            // 返回找到记录的索引
        }
    }
    return -1; 
    // 学号不存在
}

// 添加记录函数
int add_record(char num[], char name[], int score) 
{
    // 检查学号是否已经存在
    for (int i = 1; i <= idx; ++i) 
    {
        if (strcmp(info[i].num, num) == 0) return -1; 
        // 学号重复
    }

    // 增加新记录
    ++idx;
    if (idx >= MAXN) return -2; 
    // 数据量超限
    if (strlen(num) > 25) return -3; 
    // 学号长度超长
    if (strlen(name) > 25) return -4; 
    // 姓名长度超长
    strcpy(info[idx].num, num); 
    // 复制学号
    strcpy(info[idx].name, name); 
    // 复制姓名
    info[idx].score = score; 
    // 复制成绩
    write(); // 写入文件保存数据
    return 0; // 添加成功
}

// 查询成绩的前端处理函数
void query_score_output() 
{
    char num[100]; 
    // 学号或姓名的输入缓冲区
    int choice; 
    // 用户选择的查询方式
    printf("请选择查询方式:\n");
    printf("1. 通过学号查询\n2. 通过学生姓名查询\n");
    scanf("%d", &choice); 
    // 读取用户选择
    if (choice == 1) 
    {
        printf("请输入学号，回车键结束\n");
        scanf("%s", num); // 读取学号
        int id = query_score_by_num(num); 
        // 查询学号对应的记录
        if (id == -1) 
        {
            if (data_open) system(CLS); 
            // 清屏
            printf("学号不存在\n"); 
            // 提示学号不存在
            main_tab(); 
            // 返回主菜单
            return;
        }
        if (data_open) system(CLS); 
        // 清屏
        // 输出查询结果
        printf("学号[%s], 姓名[%s]的总成绩为[%d]\n", num, info[id].name, info[id].score);
    } 
    else if (choice == 2) 
    {
        printf("请输入姓名，回车键结束\n");
        scanf("%s", num); 
        // 读取姓名
        int score = -1;
        // 遍历记录查找姓名
        for (int i = 1; i <= idx; ++i) 
        {
            if (strcmp(info[i].name, num) == 0) 
            {
                printf("学号[%s], 姓名[%s]的总成绩为[%d]\n", info[i].num, info[i].name, info[i].score);
                score = 0; 
                // 找到记录
            }
        }
        if (score == -1) 
        {
            if (data_open) system(CLS); 
            // 清屏
            printf("姓名不存在\n"); 
            // 提示姓名不存在
            main_tab(); 
            // 返回主菜单
            return;
        }
        if (data_open) system(CLS); 
        // 清屏
    } 
    else 
    {
        printf("无效的选择\n"); 
        // 提示无效选择
    }
    main_tab(); 
    // 返回主菜单
}
// 主菜单函数
void main_tab() {
    // 打印主界面
    for (int i = 1; i <= 50; ++i) printf("#");
    for (int i = 1; i <= 2; ++i) printf("\n");
    printf("   学生成绩管理程序\n");
    printf("   请输入以下数字，选择您所要进行的操作：\n");
    printf("   1. 从磁盘中读取记录\n");
    printf("   2. 向磁盘中写入记录\n");
    printf("   3. 查询成绩\n");
    printf("   4. 添加记录\n");
    printf("   5. 退出系统\n");
    for (int i = 1; i <= 1; ++i) printf("\n");
    for (int i = 1; i <= 50; ++i) printf("#");
    printf("\n请输入您的选择(回车键确认)：");

    // 主菜单循环
    while (1) 
    {
        int choose;
        scanf("%d", &choose); 
        // 读取用户选择
        switch (choose) 
        {
            case 1: read(); break; 
            // 读数据文件
            case 2: write(); break; 
            // 写数据文件
            case 3: query_score_output(); break; 
            // 查询成绩
            case 4: 
            {
                char num[30], name[30];
                int score;
                printf("请输入学号、姓名和总成绩，使用空格分开\n");
                scanf("%s %s %d", num, name, &score); 
                // 读取用户输入
                int result = add_record(num, name, score); 
                // 添加记录
                // 输出操作结果
                if (result == -1) printf("学号已存在\n");
                else if (result == -2) printf("数据量超限\n");
                else if (result == -3) printf("学号超长\n");
                else if (result == -4) printf("姓名超长\n");
                else printf("记录添加成功\n");
                break;
            }
            case 5: return; 
            // 退出程序
            default: printf("无效的选择\n"); break; 
            // 提示无效选择
        }
    }
}

// 主函数
int main() 
{
    read(); 
    // 读取初始数据
    if (data_open) system(CLS); 
    // 清屏（调试模式下）
    main_tab(); 
    // 进入主菜单
    printf("已退出系统\n"); 
    // 程序退出提示
    return 0;
}