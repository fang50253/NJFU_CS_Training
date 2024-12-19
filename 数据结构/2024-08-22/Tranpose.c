//构建矩阵的三元组顺序表压缩存储结构；
//实现三元组顺序表压缩存储结构的矩阵的创建
//矩阵的输出
//矩阵的简单转置和快速转置算法
//以及两个矩阵的相加。

#include<stdio.h>
#include<string.h>
#define ElemType int
#define MAXSIZE 100
typedef struct
{
    int row,col;
    ElemType e;
}Triple;
typedef struct
{
    Triple data[MAXSIZE+1];
    int m,n,len;
}TSMarix;
void init(TSMarix *table)
{
    table->len=0;
}
void build(TSMarix *table)
{
    printf("输入矩阵的行数和列数\n");
    scanf("%d%d",&(table->m),&(table->n));
    for(int i=1;i<=table->m;++i)
    {
        for(int j=1;j<=table->n;++j)
        {
            int e;
            scanf("%d",&e);
            if(e==0) continue;
            table->len++;
            table->data[table->len].row=i;
            table->data[table->len].col=j;
            table->data[table->len].e=e;
        }
    }
}
void output(TSMarix *table)
{
    int t=1;
    for(int i=1;i<=table->m;++i)
    {
        for(int j=1;j<=table->n;++j)
        {
            if(table->data[t].row==i&&table->data[t].col==j)
                printf("%d ",table->data[t++].e);
            else
                printf("0 ");
        }
        printf("\n");
    }
}
void add(TSMarix *table1,TSMarix *table2,TSMarix *ans)
{
    if((table1->m!=table2->m)||(table1->n!=table2->n))
    {
        printf("Error\n");
        return;
    }
    ans->m=table1->m;
    ans->n=table1->n;
    init(ans);
    int t1=1,t2=1,t=1;
    for(int i=1;i<=table1->m;++i)
    {
        for(int j=1;j<=table1->n;++j)
        {
            int v=0;
            if(table1->data[t1].row==i&&table1->data[t1].col==j)
                v+=table1->data[t1++].e;
            if(table2->data[t2].row==i&&table2->data[t2].col==j)
                v+=table2->data[t2++].e;
            if(v==0) continue;
            ans->len=t;
            ans->data[t].row=i;
            ans->data[t].col=j;
            ans->data[t++].e=v;
        }
    }
    output(ans);
}
void quick_tranpose(TSMarix *table,TSMarix *ans)
{
    int col,q;
    int num[MAXSIZE],position[MAXSIZE];
    ans->len=table->len;
    ans->n=table->m;
    ans->m=table->n;
    init(ans);
    memset(num,0,sizeof num);
    for(int i=1;i<=table->len;++i) ++num[table->data[i].col];//统计出现次数
    position[1]=1;
    for(int i=2;i<=table->n;++i) position[i]=position[i-1]+num[i-1];
    for(int i=1;i<=table->len;++i)
    {
        col=table->data[i].col;
        q=position[col];
        ans->data[q].row=table->data[i].col;
        ans->data[q].col=table->data[i].row;
        ans->data[q].e=table->data[i].e;
        ++position[col];
    }
}
void tranpose(TSMarix *table,TSMarix *ans)
{
    int t=0;
    ans->len=table->len;
    ans->m=table->n;
    ans->n=table->m;
    for(int i=1;i<=table->n;++i)
    {
        for(int j=1;j<=table->len;++j)
        {
            if(table->data[j].col==i)
            {
                ans->data[++t].col=table->data[j].row;
                ans->data[t].row=table->data[j].col;
                ans->data[t].e=table->data[j].e;
            }
        }
    }
}
void menu()
{
    printf("1.读入矩阵直接输出\n");
    printf("2.读入矩阵，转置输出\n");
    printf("3.读入矩阵，快速转置输\n");
    printf("4.读入2个矩阵，相加后输出\n");
    int op;
    scanf("%d",&op);
    if(op==1)
    {
        TSMarix table;
        init(&table);
        build(&table);
        output(&table);
    }
    else if(op==2)
    {
        TSMarix table,ans;
        init(&table);
        init(&ans);
        build(&table);
        tranpose(&table,&ans);
        output(&ans);
    }
    else if(op==3)
    {
        TSMarix table,ans;
        init(&table);
        init(&ans);
        build(&table);
        quick_tranpose(&table,&ans);
        output(&ans);
    }
    else if(op==4)
    {
        TSMarix table1,table2,ans;
        init(&table1);
        init(&table2);
        init(&ans);
        build(&table1);
        build(&table2);
        add(&table1,&table2,&ans);
    }
}
int main()
{
    while(1) menu();
    return 0;
}