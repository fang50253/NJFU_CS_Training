#include<stdio.h>
#include<string.h>
const int N = 100010;
int e[N], ne[N], idx;
int h[N];
int q[N], hh = 0, tt = -1;
int n, m;
int d[N];
void add(int a, int b)
{
    e[idx] = b, ne[idx] = h[a], h[a] = idx++;
}
void topsort()
{
    for(int i = 1; i <= n; i++) if(d[i] == 0) q[++tt] = i;
    while(tt >= hh)
    {
        int a = q[hh++];
        for(int i = h[a]; i != -1; i = ne[i])
        {
            int b = e[i];
            d[b]--;
            if(d[b] == 0) q[++tt] = b;
        }
    }
    if(tt == n - 1)
    {
        for(int i = 0; i < n; i++) printf("%d ",q[i]);
    }
    else printf("-1");
}
int main(){
    scanf("%d%d",&n,&m);
    memset(h, -1, sizeof h);
    while (m -- )
    {
        int a, b;
        scanf("%d%d",&a,&b);
        d[b]++;
        add(a, b);
    }
    topsort();
    return 0;
}