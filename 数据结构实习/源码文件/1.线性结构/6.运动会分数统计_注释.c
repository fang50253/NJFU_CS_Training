#include <stdio.h>
#include <string.h>
// 定义一个结构体来表示学校的成绩信息
struct School 
{
    int no;           
    // 学校编号
    int rank[100];    
    // 各个比赛的排名
    int num[100];     
    // 各个比赛的队伍数量
    int score[100];   
    // 各个比赛的得分, score[0]存储团体总分, score[98]男子, score[99]女子
} school[100];        
// 定义一个包含100个学校信息的数组
int n, m, w;          
// n表示学校数量, m表示男子比赛项目数, w表示女子比赛项目数
// 根据排名和参赛队伍数量计算得分
int get_score(int rank, int num) 
{
    const int s3[4] = {0, 5, 3, 2};  
    // 三支队伍时的得分表
    const int s5[6] = {0, 7, 5, 3, 2, 1};  
    // 五支队伍时的得分表
    if (num == 3) 
    {
        if (rank <= 3) return s3[rank];  
        // 排名前三的得分
        else return 0;  
        // 排名超过三名则无得分
    } 
    else if (num == 5) 
    {
        if (rank <= 5) return s5[rank];  
        // 排名前五的得分
        else return 0;  
        // 排名超过五名则无得分
    } 
    else return -1;  
    // 错误的队伍数量
}
// 交换两个学校结构体
void swap(struct School *a, struct School *b) 
{
    struct School tmp = *a;
    *a = *b;
    *b = tmp;
}
// 输出所有学校的总分
void output_school_total() 
{
    for (int i = 1; i <= n; ++i)
        printf("学校%d的总分为%d\n", school[i].no, school[i].score[0]);
}
// 按照总分升序输出所有学校
void output_total() 
{
    struct School p[100];
    memcpy(p, school, sizeof(school));  
    // 复制一份学校数据以免影响原始数据
    // 使用冒泡排序对学校总分进行排序
    for (int i = 1; i <= n; ++i) 
    {
        for (int j = 1; j <= n - 1; ++j) 
        {
            if (p[j].score[0] > p[j + 1].score[0])
                swap(&p[j], &p[j + 1]);
        }
    }
    // 输出排序后的学校总分
    for (int i = 1; i <= n; ++i)
        printf("学校%d的总分为%d\n", p[i].no, p[i].score[0]);
}
// 按照男子总分升序输出所有学校
void output_male() 
{
    struct School p[100];
    memcpy(p, school, sizeof(school));  
    // 复制一份学校数据以免影响原始数据
    // 使用冒泡排序对男子总分进行排序
    for (int i = 1; i <= n; ++i) 
    {
        for (int j = 1; j <= n - 1; ++j) 
        {
            if (p[j].score[98] > p[j + 1].score[98])
                swap(&p[j], &p[j + 1]);
        }
    }
    // 输出排序后的男子总分
    for (int i = 1; i <= n; ++i)
        printf("学校%d的男子总分为%d\n", p[i].no, p[i].score[98]);
}
// 读取文件中的比赛数据并计算每个学校的得分
void read() 
{
    // n学校数量，m男子比赛项目数，w女子比赛项目数
    FILE *fp = fopen("game.fzy", "r");
    if (fp == NULL) 
    {
        printf("文件打开失败\n");
        return;
    }
    // 读取学校数量、男子比赛项目数和女子比赛项目数
    fscanf(fp, "%d%d%d", &n, &m, &w);
    // 读取每个学校在各个比赛中的排名和参赛队伍数量，并计算得分
    for (int i = 1; i <= n; ++i) 
    {
        school[i].no = i;  // 设定学校编号
        for (int j = 1; j <= m + w; ++j) 
        {
            fscanf(fp, "%d%d", &school[i].rank[j], &school[i].num[j]);  
            // 读取排名和参赛队伍数量
            school[i].score[j] = get_score(school[i].rank[j], school[i].num[j]);  
            // 计算得分
            school[i].score[0] += school[i].score[j];  
            // 更新总分
            if (j <= m) school[i].score[98] += school[i].score[j];  
            // 更新男子总分
            else school[i].score[99] += school[i].score[j];  
            // 更新女子总分
        }
    }
    fclose(fp);
}
// 按照女子总分升序输出所有学校
void output_female() 
{
    struct School p[100];
    memcpy(p, school, sizeof(school));  
    // 复制一份学校数据以免影响原始数据
    // 使用冒泡排序对女子总分进行排序
    for (int i = 1; i <= n; ++i) 
    {
        for (int j = 1; j <= n - 1; ++j) 
        {
            if (p[j].score[99] > p[j + 1].score[99])
                swap(&p[j], &p[j + 1]);
        }
    }
    // 输出排序后的女子总分
    for (int i = 1; i <= n; ++i)
        printf("学校%d的女子总分为%d\n", p[i].no, p[i].score[99]);
}
// 将当前比赛数据保存到文件中
void save() 
{
    FILE *fp = fopen("game.fzy", "w+");
    if (fp == NULL) 
    {
        printf("文件打开失败\n");
        return;
    }
    // 写入学校数量、男子比赛项目数和女子比赛项目数
    fprintf(fp, "%d %d %d\n", n, m, w);
    // 写入每个学校在各个比赛中的排名和参赛队伍数量
    for (int i = 1; i <= n; ++i) 
    {
        for (int j = 1; j <= m + w; ++j) 
        {
            fprintf(fp, "%d %d\n", school[i].rank[j], school[i].num[j]);
        }
    }
    fclose(fp);
}
// 新增一个男子项目
void new_male() 
{
    // 为每个学校新增一个男子项目
    for (int i = 1; i <= n; ++i) 
    {
        for (int j = m + 1; j <= m + w; ++j) 
        {
            school[i].score[j + 1] = school[i].score[j];
            school[i].num[j + 1] = school[i].num[j];
            school[i].rank[j + 1] = school[i].rank[j];
        }
    }
    // 询问用户获奖队伍数量
    printf("请输入获奖的队伍数量(3/5)");
    int num;
    do scanf("%d", &num); while (!(num == 3 || num == 5));
    // 询问每个学校在新项目中的排名
    printf("请按照学校顺序依次输入1-%d学校的成绩", n);
    for (int i = 1; i <= n; ++i) 
    {
        int rank;
        scanf("%d", &rank);
        school[i].rank[m + 1] = rank;
        school[i].num[m + 1] = num;
    }
    ++m;  // 更新男子比赛项目数
    save();  // 保存更新后的数据
    read();  // 重新读取数据
}
// 新增一个女子项目
void new_female() 
{
    // 询问用户获奖队伍数量
    printf("请输入获奖的队伍数量(3/5)");
    int num;
    do scanf("%d", &num); while (!(num == 3 || num == 5));
    // 询问每个学校在新项目中的排名
    printf("请按照学校顺序依次输入1-%d学校的成绩", n);
    for (int i = 1; i <= n; ++i) 
    {
        int rank;
        scanf("%d", &rank);
        school[i].rank[m + w + 1] = rank;
        school[i].num[m + w + 1] = num;
    }
    ++w;  // 更新女子比赛项目数
    save();  // 保存更新后的数据
    read();  // 重新读取数据
}
// 新增一个学校
void new_school() 
{
    ++n;  // 学校数量增加
    printf("请输入学校在各个比赛中的排名\n");
    // 输入新学校在每个比赛中的排名
    for(int i = 1; i <= m + w; ++i) 
    {
        int rank;
        scanf("%d", &rank);
        school[n].rank[i] = rank;               // 记录排名
        school[n].num[i] = school[n-1].num[i];  // 使用前一个学校的获奖队伍数量
    }
    save();  // 保存数据到文件
    read();  // 重新读取数据，以确保信息同步
}
// 主函数
int main() {
    read();  // 读取初始数据
    while(1) 
    {
        int op;  // 用户操作选项
        printf("1.按照学校编号输出\n");
        printf("2.按照学校总成绩升序输出\n");
        printf("3.按照男生成绩升序输出\n");
        printf("4.按照女生成绩升序输出\n");
        printf("5.新增一个男生项目\n");
        printf("6.新增一个女生项目\n");
        printf("7.新增一个学校\n");
        printf("0.退出\n");
        scanf("%d", &op);  // 获取用户输入的操作选项
        // 根据用户选择执行相应的操作
        if(op == 0) break;                      
        // 退出程序
        else if(op == 1) output_school_total(); 
        // 按学校编号输出成绩
        else if(op == 2) output_total();        
        // 按学校总成绩升序输出
        else if(op == 3) output_male();         
        // 按男生成绩升序输出
        else if(op == 4) output_female();       
        // 按女生成绩升序输出
        else if(op == 5) new_male();            
        // 新增一个男生项目
        else if(op == 6) new_female();          
        // 新增一个女生项目
        else if(op == 7) new_school();          
        // 新增一个学校
    }
    return 0;  // 程序结束
}