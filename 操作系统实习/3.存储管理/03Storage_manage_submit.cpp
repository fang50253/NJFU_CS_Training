#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define MAXSIZE 400
struct p_str
{
    int pagenum; // 页号
    int count; // 页面访问次数
    int clock_visit; // 改进Clock置换的访问
    int clock_revise; // 改进Clock的修改
    int LRU_time; // 未使用的时间(LRU)
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
    void print(const char way[],int Page_num,int RAM,double success)
    {
        printf("This is %s:\nPage_num=%d,RAM=%dPages,success_rate=%.1lf %%\n\n\n",way,Page_num,RAM,success/4);
    }
    void init()
    {
        effective_times = 0;
        len = 0;
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
        this->n = n;
    }
    storage(int n)
    {
        set(n);
    }
    void LRU()
    {
        init();
        for (int i=0;i<MAXSIZE;++i)
        {
            int t = 0;
            int page = rand() % 400 / 10;
            p_str* p = begin;
            for (int j=0;j<len;++j) // 遍历
            {
                p = p->next;
                if (p->pagenum == page)
                {
                    ++p->count;
                    p->LRU_time = -1;
                    ++t;
                    ++effective_times;
                    break;
                }
            }
            if (!t) // 访问失败
            {
                if (len < n) // 未满
                {
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1;
                    q->pagenum = page;
                    q->next = NULL;
                    q->LRU_time = -1;
                    end->next = q;
                    end = q;
                    ++len;
                }
                else // 置换
                {
                    int max, maxtime = -1;
                    p = begin;
                    for (int j=0;j<len;++j)
                    {
                        p = p->next;
                        if (p->LRU_time > maxtime)
                        {
                            maxtime = p->LRU_time;
                            max = j;
                        }
                    }
                    p = begin;
                    for (int j=0;j<max-1;++j)
                        p = p->next;
                    p->next = p->next->next;
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1;
                    q->pagenum = page;
                    q->next = NULL;
                    q->LRU_time = -1;
                    end->next = q;
                    end = q;
                }
            }
            p = begin;
            for (int j=0;j<len;++j) // 更新未访问时间
            {
                p = p->next;
                ++p->LRU_time;
            }
        }
        print("LRU",MAXSIZE,n,effective_times);
    }
    void Improve_Clock()
    {
        init();
        for (int i=0;i<MAXSIZE;++i)
        {
            int t = 0;
            int page = rand() % 400 / 10;
            int m = rand() % 2;
            p_str* p = begin;
            for (int j=0;j<len;++j) // 遍历，寻找访问
            {
                p = p->next;
                if (p == NULL) p = end;
                if (p->pagenum == page)
                {
                    p->clock_visit = 1;
                    ++t;
                    ++effective_times;
                    break;
                }
            }
            if (!t)
            {
                if (len < n) // 未满
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
                    ++len;
                }
                else // 寻找并置换
                {
                    int f = 0;
                    p_str* q = (p_str*)malloc(sizeof(p_str));
                    q->count = 1;
                    q->pagenum = page;
                    q->next = NULL;
                    q->LRU_time = -1;
                    q->clock_visit = 1;
                    q->clock_revise = m;
                    p_str* p = begin;
                    for (int j=0;j<len-1;++j)
                    {
                        if (p->next->clock_visit == 0 && p->next->clock_revise == 0)
                        {
                            p->next = p->next->next;
                            end->next = q;
                            end = q;
                            f = 1;
                            break;
                        }
                        else p = p->next;
                    }
                    if (f) continue;
                    else
                    {
                        p = begin;
                        for (int j=0;j<len-1;++j)
                        {
                            if (p->next->clock_visit == 0 && p->next->clock_revise == 1)
                            {
                                p->next = p->next->next;
                                end->next = q;
                                end = q;
                                f = 1;
                                break;
                            }
                            else p = p->next;
                        }
                    }
                    if (f) continue;
                    else
                    {
                        p = begin;
                        for (int j=0;j<len;++j)
                        {
                            p = p->next;
                            p->clock_visit = 0;
                        }
                    }
                    p = begin;
                    for (int j=0;j<len-1;++j)
                    {
                        if (p->next->clock_visit == 0 && p->next->clock_revise == 0)
                        {
                            p->next = p->next->next;
                            end->next = q;
                            end = q;
                            f = 1;
                            break;
                        }
                        else p = p->next;
                    }
                    if (f) continue;
                    else
                    {
                        p = begin;
                        for (int j=0;j<len-1;++j)
                        {
                            if (p->next->clock_visit == 0 && p->next->clock_revise == 1)
                            {
                                p->next = p->next->next;
                                end->next = q;
                                end = q;
                                f = 1;
                                break;
                            }
                            else p = p->next;
                        }
                    }
                }
            }
        }
        print("Improved_clock",MAXSIZE,n,effective_times);
    }
};
int main()
{
    int n;
    srand((unsigned)time(NULL));
    scanf("%d", &n);
    printf("\n\n\n");
    storage *a=new storage(n);
    a->set(n);
    a->LRU();
    a->set(n);
    a->Improve_Clock();
    printf("\n\n\n");
    delete a;
    return 0;
}