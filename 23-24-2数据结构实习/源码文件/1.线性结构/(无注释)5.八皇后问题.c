#include<stdio.h>
#include<string.h>
int q[9],st[9],ans_cnt;
int check()
{
    //先统计序号和q中值之和
    int cnt[20];
    memset(cnt,0,sizeof cnt);
    for(int i=1;i<=8;++i) ++cnt[i+q[i]];
    for(int i=2;i<=16;++i)
        if(cnt[i]>1) return 0;
    //先统计序号和q中值之差(带偏移值)
    memset(cnt,0,sizeof cnt);
    for(int i=1;i<=8;++i) ++cnt[i-q[i]+10];
    for(int i=3;i<=17;++i)
        if(cnt[i]>1) return 0;
    return 1;
}
void print()
{
    ans_cnt++;
    for(int i=1;i<=8;++i) printf("%d ",q[i]);
    printf("\n");
}
void dfs(int n)
{
    if(n==9)
    {
        if(check()) print();
        return;
    }
    for(int i=1;i<=8;++i)
    {
        if(st[i]==0)
        {
            st[i]=1;
            q[n]=i;
            dfs(n+1);
            st[i]=0;
        }
    }
}
int main()
{
    dfs(1);
    printf("%d",ans_cnt);
    return 0;
}