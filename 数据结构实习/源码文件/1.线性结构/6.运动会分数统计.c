#include<stdio.h>
#include<string.h>
struct School
{
    int no;
    int rank[100];
    int num[100];
    int score[100];
    //score[0]存储团体总分，score[98]男子，score[99]女子
}school[100];
int n,m,w;
int get_score(int rank,int num)
{
    const int s3[4]={0,5,3,2};
    const int s5[6]={0,7,5,3,2,1};
    if(num==3)
    {
        if(rank<=3) return s3[rank];
        else return 0;
    }
    else if(num==5)
    {
        if(rank<=5) return s5[rank];
        else return 0;
    }
    else return -1;
}
void swap(struct School *a,struct School *b)
{
    struct School tmp=*a;
    *a=*b;
    *b=tmp;
}
void output_school_total()
{
    for(int i=1;i<=n;++i) printf("学校%d的总分为%d\n",school[i].no,school[i].score[0]);
}
void output_total()
{
    struct School p[100];
    memcpy(p,school,sizeof school);
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=n-1;++j)
        {
            if(p[j].score[0]>p[j+1].score[0])
                swap(&p[j],&p[j+1]);
        }
    }
    for(int i=1;i<=n;++i) printf("学校%d的总分为%d\n",p[i].no,p[i].score[0]);
}
void output_male()
{
    struct School p[100];
    memcpy(p,school,sizeof school);
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=n-1;++j)
        {
            if(p[j].score[98]>p[j+1].score[98])
                swap(&p[j],&p[j+1]);
        }
    }
    for(int i=1;i<=n;++i) printf("学校%d的总分为%d\n",p[i].no,p[i].score[0]);
}
void read()
{
    //n学校数量，m男子，w女子
    FILE *fp=fopen("game.fzy","r");
    if(fp==NULL)
    {
        printf("文件打开失败\n");
        return;
    }
    fscanf(fp,"%d%d%d",&n,&m,&w);
    for(int i=1;i<=n;++i)
    {
        school[i].no=i;
        for(int j=1;j<=m+w;++j)
        {
            fscanf(fp,"%d%d",&school[i].rank[j],&school[i].num[j]);
            school[i].score[j]=get_score(school[i].rank[j],school[i].num[j]);
            school[i].score[0]+=school[i].score[j];
            if(j<=m) school[i].score[98]+=school[i].score[j];
            else school[i].score[99]+=school[i].score[j];
        }
    }
    fclose(fp);
}
void output_female()
{
    struct School p[100];
    memcpy(p,school,sizeof school);
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=n-1;++j)
        {
            if(p[j].score[99]>p[j+1].score[99])
                swap(&p[j],&p[j+1]);
        }
    }
    for(int i=1;i<=n;++i) printf("学校%d的总分为%d\n",p[i].no,p[i].score[0]);
}
void save()
{
    FILE *fp=fopen("game.fzy","w+");
    if(fp==NULL)
    {
        printf("文件打开失败\n");
        return;
    }
    fprintf(fp,"%d %d %d\n",n,m,w);
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=m+w;++j)
        {
            fprintf(fp,"%d %d\n",school[i].rank[j],school[i].num[j]);
        }
    }
    fclose(fp);
}
void new_male()
{
    for(int i=1;i<=n;++i)
    {
        for(int j=m+1;j<=m+w;++j)
        {
            school[i].score[j+1]=school[i].score[j];
            school[i].num[j+1]=school[i].num[j];
            school[i].rank[j+1]=school[i].rank[j];
        }
    }
    printf("请输入获奖的队伍数量(3/5)");
    int num;
    do scanf("%d",&num);while(!(num==3||num==5));
    printf("请按照学校顺序依次输入1-%d学校的成绩",n);
    for(int i=1;i<=n;++i)
    {
        int rank;
        scanf("%d",&rank);
        school[i].rank[m+1]=rank;
        school[i].num[m+1]=num;
    }
    ++m;
    save();
    read();
}
void new_female()
{
    printf("请输入获奖的队伍数量(3/5)");
    int num;
    do scanf("%d",&num);while(!(num==3||num==5));
    printf("请按照学校顺序依次输入1-%d学校的成绩",n);
    for(int i=1;i<=n;++i)
    {
        int rank;
        scanf("%d",&rank);
        school[i].rank[m+w+1]=rank;
        school[i].num[m+w+1]=num;
    }
    ++w;
    save();
    read();
}
void new_school()
{
    ++n;
    printf("请输入学校在各个比赛中的排名\n");
    for(int i=1;i<=m+w;++i)
    {
        int rank;
        scanf("%d",&rank);
        school[n].rank[i]=rank;
        school[n].num[i]=school[n-1].num[i];
    }
    save();
    read();
}
int main()
{
    read();
    while(1)
    {
        int op;
        printf("1.按照学校编号输出\n");
        printf("2.按照学校总成绩升序输出\n");
        printf("3.按照男生成绩升序输出\n");
        printf("4.按照女生成绩升序输出\n");
        printf("5.新增一个男生项目\n");
        printf("6.新增一个女生项目\n");
        printf("7.新增一个学校\n");
        printf("0.退出\n");
        scanf("%d",&op);
        if(op==0) break;
        else if(op==1) output_school_total();
        else if(op==2) output_total();
        else if(op==3) output_male();
        else if(op==4) output_female();
        else if(op==5) new_male();
        else if(op==6) new_female();
        else if(op==7) new_school();
    }
    return 0;
}