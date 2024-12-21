#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define MAXSIZE 400
struct p_str
{
	int pagenum;//页号
	int count;//页面访问次数
	int clock_visit;//改进Clock置换的访问
	int clock_revise;//改进Clock的修改
	int LRU_time;//未使用的时间(LRU)
	struct p_str* next;
};
class storage
{
    private:
    int n;
    double effective_times;
    int len;
    p_str* begin;
    p_str* end;
    void init()
    {
        effective_times = 0;
        len =0;
        begin = (p_str*)malloc(sizeof(p_str));
        end = begin;
        begin->count = -1;
        begin->pagenum = -1;
        begin->clock_visit = -1; 
        begin->clock_revise = -1;
        begin->next = NULL;
        begin->LRU_time = -1;
    }
    public:
    void set(int n)
    {
        this->n=n;
    }
    storage(int n)
    {
        set(n);
    }
    void FIFO()
    {
        init();
        for (int i = 0; i < MAXSIZE; i++)
        {
            int t = 0;
            int page = rand() % 400 / 10;
            p_str* pp = begin;
            for (int j=0;j<len;j++)//看队列里面有没有这一页
            {
                pp=pp->next;
                if(pp->pagenum==page) 
                {
                    pp->count++;
                    t = 1;
                    effective_times++;
                    break;
                }
            }
            if (t == 0) 
            {
                if(len<n)//还有剩余空间
                {
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1; q->pagenum = page; q->next = NULL;
                    end->next = q;
                    end = q;
                    len++;
                }
                else //置换
                {
                    begin->next = begin->next->next;
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1; q->pagenum = page; q->next = NULL;
                    end->next = q;
                    end = q;
                }
            }
        }
        printf("Page_number=%d,RAM=%dPages,FIFO_success_rate=%.1lf %%\n", MAXSIZE, n, (effective_times / 400) * 100);
    }
    void LRU() 
    {
        init();
        for (int i = 0; i < MAXSIZE; i++) 
        {
            int t = 0;
            int page = rand() % 400 / 10;
            p_str* pp = begin;
            for (int j = 0; j < len; j++) //遍历
            {
                pp = pp->next;
                if (pp->pagenum == page) 
                {
                    pp->count++; 
                    pp->LRU_time = -1; 
                    t++; 
                    effective_times++; 
                    break;
                }
            }
            if (!t) //访问失败
            {
                if (len < n) //未满
                {	
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1; q->pagenum = page; q->next = NULL; q->LRU_time = -1;
                    end->next = q;
                    end = q;
                    len++;
                }
                else //置换
                {
                    int max, maxtime = -1;
                    pp = begin;
                    for (int j = 0; j < len; j++) 
                    {
                        pp = pp->next;
                        if (pp->LRU_time > maxtime) 
                        {
                            maxtime = pp->LRU_time;
                            max = j;
                        }
                    }
                    pp = begin;
                    for (int j = 0; j < max - 1; j++) pp = pp->next;
                    pp->next = pp->next->next;
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1; q->pagenum = page; q->next = NULL; q->LRU_time = -1;
                    end->next = q;
                    end = q;
                }
            }
            pp = begin;
            for (int j = 0; j < len; j++) //更新未访问时间
            {
                pp = pp->next;
                pp->LRU_time++;
            }
        }
        printf("Page_num=%d,RAM=%dPages,LRU_success_rate=%.1lf %%\n", MAXSIZE, n, (effective_times / 400) * 100);
    }
    void  Improve_Clock() 
    {
        init();
        for (int i = 0; i < MAXSIZE; i++) 
        {
            int t = 0;
            int page = rand() % 400 / 10;
            int  m = rand() % 2;
            p_str* pp = begin;
            for (int j = 0; j < len; j++) //遍历，寻找访问
            {
                pp = pp->next;
                if (pp == NULL) pp = end;
                if (pp->pagenum == page) 
                {
                    pp->clock_visit=1; 
                    ++t; 
                    ++effective_times; 
                    break;
                }
            }
            if (!t) 
            {
                if (len < n) //未满
                {
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1; 
                    q->pagenum = page; 
                    q->next = NULL; 
                    q->LRU_time = -1; 
                    q->clock_visit = 1; 
                    q->clock_revise = m;
                    end->next = q;
                    end = q;
                    len++;
                }
                else //寻找并置换
                {
                    int f = 0;
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1; 
                    q->pagenum = page;
                    q->next = NULL; 
                    q->LRU_time = -1; 
                    q->clock_visit = 1; 
                    q->clock_revise = m;
                    p_str* pp = begin;
                    for (int j = 0; j < len-1; j++) 
                    {
                        if (pp->next->clock_visit == 0 && pp->next->clock_revise == 0) 
                        {
                            pp->next = pp->next->next;
                            end->next = q;
                            end = q;
                            f = 1; 
                            break;
                        }
                        else pp = pp->next;
                    }
                    if(f) continue;
                    else 
                    {
                        pp = begin;
                        for (int j = 0; j < len-1; j++) 
                        {
                            if (pp->next->clock_visit == 0 && pp->next->clock_revise == 1) 
                            {
                                pp->next = pp->next->next;
                                end->next = q;
                                end = q;
                                f = 1; break;
                            }
                            else pp = pp->next;
                        }
                    }
                    if (f)continue;
                    else 
                    {
                        pp = begin;
                        for (int j = 0; j < len; j++) 
                        {
                            pp = pp->next;
                            pp->clock_visit = 0;
                        }
                    }
                    pp = begin;
                    for (int j = 0; j < len-1; j++) 
                    {
                        if (pp->next->clock_visit == 0 && pp->next->clock_revise == 0) 
                        {
                            pp->next = pp->next->next;
                            end->next = q;
                            end = q;
                            f = 1; break;
                        }
                        else pp = pp->next;
                    }
                    if (f)continue;
                    else 
                    {
                        pp = begin;
                        for (int j = 0; j < len-1; j++) 
                        {
                            if (pp->next->clock_visit == 0 && pp->next->clock_revise == 1) 
                            {
                                pp->next = pp->next->next;
                                end->next = q;
                                end = q;
                                f = 1; break;
                            }
                            else pp = pp->next;
                        }
                    }
                }
            }
        }
        printf("Page_num=%d,RAM=%dPages,Improved_Clock_success_rate=%.1lf%%\n", MAXSIZE, n, (effective_times / 400) * 100);
    }
};
int main() 
{
	int n;
	srand((unsigned)time(NULL));
	scanf("%d",&n);
    printf("\n\n\n");
    storage a(n);
    a.set(n);
    a.LRU();
    a.set(n);
	a.Improve_Clock();
    printf("\n\n\n");
    return 0;
}
 
