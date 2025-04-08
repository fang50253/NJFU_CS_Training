#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CLS "clear" // 在Windows系统中改成"cls" 调试端为macOS
const int MAXN = 1e3 + 10; // 数据量上限

int data_open; // 日志模式开关
int idx = 0; // 数据量顶

// 数据库结构体
struct DataBase {
    char num[30]; // 学号
    char name[30]; // 姓名
    int score; // 总成绩，使用int类型以防偏移
} info[MAXN];

// 函数声明
int read(); // 读取数据
int write(); // 写入数据
int query_score_by_num(char num[]); // 通过学号查询成绩
void query_score_output(); // 查询成绩的前端处理函数
void main_tab(); // 主菜单
int add_record(char num[], char name[], int score); // 添加记录函数

// 读取数据函数
int read() {
    FILE *fp = fopen("data.fzy", "r"); // 以只读模式打开文件
    if (fp == NULL) return 0; // 如果文件读取失败，则代表程序为新程序
    fscanf(fp, "%d", &data_open); // 读取日志模式状态

    char num[100], name[100]; // 文件读取缓冲区
    int score; // 文件读取缓冲区
    memset(info, 0, sizeof info); // 清空原有数据
    idx = 0; // 清空数据顶

    while (fscanf(fp, "%s %s %d", num, name, &score) == 3) {
        ++idx;
        if (idx >= MAXN - 10) printf("Error, 数据量过大\n");
        // 数据最大规模可以在代码顶部的全局变量区进行调整
        strcpy(info[idx].num, num); // 将文件读入内存
        strcpy(info[idx].name, name); // 将文件读入内存
        info[idx].score = score; // 将文件读入内存
    }
    fclose(fp); // 关闭文件，停止占用
    return 0;
}

// 写入数据函数
int write() {
    FILE *fp = fopen("data.fzy", "w+"); // 打开文件以写模式
    fprintf(fp, "%d\n", data_open); // 写入日志模式配置文件
    for (int i = 1; i <= idx; ++i) {
        fprintf(fp, "%s %s %d\n", info[i].num, info[i].name, info[i].score);
    }
    fclose(fp); // 关闭文件，停止占用
    printf("数据保存成功\n"); // 保存成功提示
    return 0;
}

// 通过学号查询总成绩
int query_score_by_num(char num[]) {
    for (int i = 1; i <= idx; ++i) {
        if (strcmp(info[i].num, num) == 0) {
            return i; // 返回成绩
        }
    }
    return -1; // 学号不存在
}
// 添加记录函数
int add_record(char num[], char name[], int score) {
    // 检查学号是否已经存在
    for (int i = 1; i <= idx; ++i) {
        if (strcmp(info[i].num, num) == 0) return -1; // 学号重复
    }

    // 增加记录
    ++idx;
    if (idx >= MAXN) return -2; // 数据量超限
    if (strlen(num) > 25) return -3; // 学号超长
    if (strlen(name) > 25) return -4; // 姓名超长

    strcpy(info[idx].num, num);
    strcpy(info[idx].name, name);
    info[idx].score = score;
    write(); // 写入文件保存信息
    return 0; // 添加成功
}

// 查询成绩的前端处理函数
void query_score_output() {
    char num[100];
    int choice;
    printf("请选择查询方式:\n");
    printf("1.通过学号查询\n2.通过学生姓名查询\n");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("请输入学号，回车键结束\n");
        scanf("%s", num);
        int id=query_score_by_num(num);
        int score = info[id].score; // 查询成绩
        if (id == -1) {
            if (data_open) system(CLS);
            printf("学号不存在\n"); // 处理特殊情况
            main_tab(); // 返回主菜单
            return;
        }
        if (data_open) system(CLS);
        printf("学号[%s],姓名[%s]的总成绩为[%d]\n", num, info[id].name ,score); // 输出成绩
    } 
    else if(choice==2)
    {
        printf("请输入姓名，回车键结束\n");
        scanf("%s", num);
        int score=-1;
        for (int i = 1; i <= idx; ++i) {
        if (strcmp(info[i].name, num) == 0) {
            printf("学号[%s],姓名[%s]的总成绩为[%d]\n", info[i].num, info[i].name ,info[i].score);
            score=0;
        }
    }
        if (score == -1) {
            if (data_open) system(CLS);
            printf("姓名不存在\n"); // 处理特殊情况
            main_tab(); // 返回主菜单
            return;
        }
        if (data_open) system(CLS);
    }
    else {
        printf("无效的选择\n");
    }

    main_tab(); // 返回主菜单
}

// 主菜单函数
void main_tab() {
    // 主界面
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
    while (1) {
        int choose;
        scanf("%d", &choose); // 不断重复询问选择
        switch (choose) {
            case 1: read(); break; // 读文件
            case 2: write(); break; // 写文件
            case 3: query_score_output(); break; // 查询成绩
            case 4: {
                char num[30], name[30];
                int score;
                printf("请输入学号、姓名和总成绩，使用空格分开\n");
                scanf("%s %s %d", num, name, &score);
                int result = add_record(num, name, score); // 添加记录
                if (result == -1) printf("学号已存在\n");
                else if (result == -2) printf("数据量超限\n");
                else if (result == -3) printf("学号超长\n");
                else if (result == -4) printf("姓名超长\n");
                else printf("记录添加成功\n");
                break;
            }
            case 5: return; // 退出程序
            default: printf("无效的选择\n"); break;
        }
    }
}

// 主函数
int main() {
    read(); // 读取数据
    if (data_open) system(CLS);
    main_tab(); // 进入主菜单
    printf("已退出系统\n"); // 退出提示
    return 0;
}