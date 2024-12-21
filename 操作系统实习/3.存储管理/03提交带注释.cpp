#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<iostream>
#define MAXSIZE 400
namespace fzy//创建命名空间fzy，用于写一些数据结构
{
    template<typename T>
    struct queuenode 
    {
        T v = T();
        queuenode<T>* next = nullptr;
    };
    template <typename T>
    class queue 
    {
    private:
        queuenode<T>* head;
        queuenode<T>* last;
        int size_;
    public:
        queue() 
        {
            head = new queuenode<T>;
            last = head;
            size_ = 0;
        }
        queue(const queue<T>& other) 
        {
            head = new queuenode<T>;
            last = head;
            size_ = 0;
            queuenode<T>* current = other.head->next;
            while (current != nullptr) 
            {
                push(current->v);
                current = current->next;
            }
        }
        queue<T>& operator=(const queue<T>& other)//重载队列拷贝函数
        {
            if (this != &other) 
            {
                clear();
                queuenode<T>* current = other.head->next;
                while (current != nullptr) 
                {
                    push(current->v);
                    current = current->next;
                }
            }
            return *this;
        }
        ~queue() 
        {
            clear();
            delete head;
        }
        void clear() 
        {
            while (!empty()) pop();
        }
        void push(T v) 
        {
            queuenode<T>* newnode = new queuenode<T>;
            newnode->v = v;
            last->next = newnode;
            last = newnode;
            ++size_;
        }
        void pop() 
        {
            if (!empty()) 
            {
                queuenode<T>* temp = head->next;
                head->next = temp->next;
                if (head->next == nullptr) last = head;
                delete temp;
                --size_;
            }
        }
        T front() const 
        {
            if (!empty()) return head->next->v;
            throw std::runtime_error("Queue is empty");
        }
        T back() const 
        {
            if (!empty()) return last->v;
            throw std::runtime_error("Queue is empty");
        }
        bool empty() const 
        {
            return size_ == 0;
        }
        int size() const 
        {
            return size_;
        }
        void print() const 
        {
            queuenode<T>* current = head->next;
            while (current != nullptr) 
            {
                std::cout << current->v << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    };
}
struct p_str 
{
    int pagenum;     // 页号
    int count;       // 页面访问次数
    int clock_visit; // 改进 Clock 的访问标记
    int clock_revise;// 改进 Clock 的修改标记
    int LRU_time;    // LRU 的未使用时间
    p_str(int pagenum = -1, int count = -1, int clock_visit = -1, int clock_revise = -1, int LRU_time = -1)
        : pagenum(pagenum), count(count), clock_visit(clock_visit), clock_revise(clock_revise), LRU_time(LRU_time) {}
};
class storage 
{
private:
    int n;                        // 内存大小
    double effective_times;       // 命中次数
    fzy::queue<p_str> fifo;       // FIFO 队列
    fzy::queue<p_str> clock;      // 改进 Clock 队列
    fzy::queue<p_str> lru_queue;  // LRU 模拟队列（用于访问）
    void init() 
    {
        effective_times = 0;
        while (!fifo.empty()) fifo.pop();
        while (!clock.empty()) clock.pop();
        while (!lru_queue.empty()) lru_queue.pop();
    }
    void print(const char way[],int Page_num,int RAM,double success)
    {
        printf("This is %s:\nPage_num=%d,RAM=%dPages,success_rate=%.1lf %%\n\n\n",way,Page_num,RAM,success/4);
    }
public:
    storage(int n) : n(n) {}//创建默认构造函数
    void FIFO() 
    {
        init();
        for (int i = 0; i < MAXSIZE; ++i) {
            int t = 0;
            int page=rand()%400/10;// 检查 FIFO 队列是否命中
            fzy::queue<p_str>temp=fifo;
            while (!temp.empty()) 
            {
                if (temp.front().pagenum == page) 
                {
                    ++t;
                    ++effective_times;
                    break;
                }
                temp.pop();
            }
            // 未命中则进行置换
            if (!t) 
            {
                if ((int)fifo.size() < n) fifo.push(p_str(page, 1));
                else 
                {
                    fifo.pop();            // 移除队首页面
                    fifo.push(p_str(page, 1)); // 加入新页面
                }
            }
        }
        print("FIFO",MAXSIZE,n,effective_times);
    }
    void LRU() 
    {
        init();
        for (int i = 0; i < MAXSIZE; ++i) 
        {
            int t = 0;
            int page = rand() % 400 / 10;
            // 检查 LRU 队列是否命中
            fzy::queue<p_str> temp = lru_queue;
            fzy::queue<p_str> new_queue; // 用于构建更新后的队列
            while (!temp.empty()) 
            {
                p_str current = temp.front();
                temp.pop();
                if (current.pagenum == page) 
                {
                    ++t;
                    ++effective_times;
                    current.LRU_time = 0; // 重置未访问时间
                } 
                else ++current.LRU_time; // 更新未访问时间
                new_queue.push(current);
            }
            lru_queue = new_queue;
            // 未命中则置换
            if (!t) 
            {
                if ((int)lru_queue.size() < n) lru_queue.push(p_str(page, 1, -1, -1, 0));
                else 
                {
                    // 找到最久未使用页面
                    fzy::queue<p_str> temp = lru_queue;
                    p_str max_page;
                    int max_time = -1;
                    while (!temp.empty()) 
                    {
                        p_str current = temp.front();
                        temp.pop();
                        if (current.LRU_time > max_time) 
                        {
                            max_time = current.LRU_time;
                            max_page = current;
                        }
                    }
                    fzy::queue<p_str> new_queue;
                    temp = lru_queue;
                    while (!temp.empty()) 
                    {
                        p_str current = temp.front();
                        temp.pop();
                        if (current.pagenum != max_page.pagenum) new_queue.push(current);
                    }
                    new_queue.push(p_str(page,1,-1,-1,0));
                    lru_queue = new_queue;
                }
            }
        }
        print("LRU",MAXSIZE,n,effective_times);
    }
    void Improved_Clock() 
    {
        init();
        for (int i = 0; i < MAXSIZE; ++i) 
        {
            int t = 0;
            int page = rand() % 400 / 10;
            int m = rand() % 2;
            // 检查 Clock 队列是否命中
            fzy::queue<p_str> temp = clock;
            while (!temp.empty()) 
            {
                if (temp.front().pagenum == page) 
                {
                    t = 1;
                    ++effective_times;
                    p_str cur = temp.front(); // 获取队首元素
                    cur.clock_visit = 1;      // 修改访问标记
                    temp.pop();               // 弹出旧的队首
                    temp.push(cur);           // 将修改后的节点重新加入队列
                    break;
                }
                temp.pop();
            }
            // 未命中则置换
            if (!t) 
            {
                if ((int)clock.size() < n) clock.push(p_str(page, 1, 1, m));
                else 
                {
                    // 查找替换目标
                    while (true) 
                    {
                        p_str cur = clock.front();
                        clock.pop();
                        if (cur.clock_visit == 0) 
                        {
                            clock.push(p_str(page, 1, 1, m));
                            break;
                        } 
                        else 
                        {
                            cur.clock_visit = 0;
                            clock.push(cur); // 重置访问标记，放回队列末尾
                        }
                    }
                }
            }
        }
        print("Improved_Clock",MAXSIZE,n,effective_times);
    }
};
int main() 
{
    int n;
    srand((unsigned)time(NULL));
    scanf("%d", &n);
    printf("\n\n\n");
    storage storage(n);
    storage.FIFO();
    storage.LRU();
    storage.Improved_Clock();
    printf("\n\n\n");
    return 0;
}