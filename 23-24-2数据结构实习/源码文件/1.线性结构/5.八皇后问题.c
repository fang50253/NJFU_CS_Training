#include<stdio.h>
#include<string.h>
int q[9],st[9],ans_cnt;
//q数组用于存放答案，st代表这个数字是否被使用过，ans_cnt统计答案的个数
int check()
{
    //先统计序号和q中值之和
    int cnt[20];
    memset(cnt,0,sizeof cnt);
    for(int i=1;i<=8;++i) ++cnt[i+q[i]];
    //统计每一个皇后所在位置的横坐标和纵坐标之和
    for(int i=2;i<=16;++i)
        if(cnt[i]>1) return 0;
    //如果存在两个数字的横坐标和纵坐标之和相等
    //那就说明存在两个或者更多的皇后出现在同一个副对角线上
    //先统计序号和q中值之差(带偏移值)
    memset(cnt,0,sizeof cnt);
    for(int i=1;i<=8;++i) ++cnt[i-q[i]+10];
    //统计每一个皇后坐在位置横坐标和纵坐标位置之差(算偏移值)
    for(int i=3;i<=17;++i)
        if(cnt[i]>1) return 0;
    //如果存在两个数字其横坐标和纵坐标点差相等
    //那就说明存在两个或更多的皇后出现在同一个主对角线上
    return 1;
}
void print()
{
    ans_cnt++;//print答案说明答案正确
    for(int i=1;i<=8;++i) printf("%d ",q[i]);
    //将答案数组q里的元素全部输出
    printf("\n");
    //输出结束
}
void dfs(int n)
{
    if(n==9)
    //如果已经搜索到了第九层，说明全排列已经生成完毕
    {
        if(check()) print();
        //如果满足要求，那么输出结果
        return;//递归退层
    }
    for(int i=1;i<=8;++i)
    //生成全排列
    {
        if(st[i]==0)
        {
            st[i]=1;
            q[n]=i;
            //如果这个数字没有被使用过，那么使用
            dfs(n+1);
            st[i]=0;
            //归位
        }
    }
}
int main()
{
    dfs(1);
    //从第一位开始生成
    printf("%d",ans_cnt);
    //输出结果的总数
    return 0;
}