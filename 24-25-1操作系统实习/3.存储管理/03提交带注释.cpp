#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<iostream>
#define MAXSIZE 400
namespace fzy//创建命名空间fzy，用于写一些数据结构
{
    template<typename T>
    struct queuenode //创建队列结点
    {
        T v = T();//提供队列结点的默认构造方法
        queuenode<T>* next = nullptr;//指向下一个位置，并提供默认构造方法
    };
    template <typename T>//创建队列模板类
    class queue 
    {
    private:
        queuenode<T>* head;
        queuenode<T>* last;
        int size_;//队列的大小
    public:
        queue() //队列模板的默认构造方法
        {
            head = new queuenode<T>;
            last = head;
            size_ = 0;
        }
        queue(const queue<T>& other) //提供带参的构造函数
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
        queue<T>& operator=(const queue<T>& other)//重载队列拷贝函数，深拷贝
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
        ~queue() //析构函数
        {
            clear();
            delete head;
        }
        void clear() //清空队列
        {
            while (!empty()) pop();
        }
        void push(T v) //将元素压入队列
        {
            queuenode<T>* newnode = new queuenode<T>;//创建一个新的结点
            newnode->v = v;
            last->next = newnode;
            last = newnode;
            ++size_;
        }
        void pop() //弹出
        {
            if (!empty()) 
            {
                queuenode<T>* temp = head->next;
                head->next = temp->next;
                if (head->next == nullptr) last = head;
                delete temp;
                --size_;
            }
            else 
            ;//throw std::runtime_error("queue_empty");
        }
        T front() const //返回队列队首
        {
            if (!empty()) return head->next->v;
            //throw std::runtime_error("Queue is empty");
        }
        T back() const //返回队列队尾
        {
            if (!empty()) return last->v;
            //throw std::runtime_error("Queue is empty");
        }
        bool empty() const //队列判空
        {
            return size_ == 0;
        }
        int size() const //返回队列的长度
        {
            return size_;
        }
        /*
        void print() const //输出队列，用于调试
        {
            queuenode<T>* current = head->next;
            while (current != nullptr) 
            {
                std::cout << current->v << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
        */
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
        : pagenum(pagenum), count(count), clock_visit(clock_visit), clock_revise(clock_revise), LRU_time(LRU_time) {}//提供构造方法
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
        while (!fifo.empty()) fifo.pop();//初始化每一个数据结构
        while (!clock.empty()) clock.pop();//初始化每一个数据结构
        while (!lru_queue.empty()) lru_queue.pop();//初始化每一个数据结构
    }
    void print(const char way[],int Page_num,int RAM,double success)
    {
        printf("This is %s:\nPage_num=%d,RAM=%dPages,success_rate=%.1lf %%\n\n\n",way,Page_num,RAM,success/4);
        //输出结果
    }
public:
    storage(int n) : n(n) {}//创建默认构造函数
    void FIFO() 
    {
        init();//初始化
        for (int i = 0; i < MAXSIZE; ++i) {
            int t = 0;
            int page=rand()%400/10;// 检查 FIFO 队列是否命中
            fzy::queue<p_str>temp=fifo;
            while (!temp.empty()) 
            {
                if (temp.front().pagenum == page) //在页表中
                {
                    ++t;
                    ++effective_times;//有效
                    break;
                }
                temp.pop();
            }
            // 未命中则进行置换
            if (!t) 
            {
                if ((int)fifo.size() < n) fifo.push(p_str(page, 1));//队列没有满
                else 
                {
                    fifo.pop();            // 移除队首页面
                    fifo.push(p_str(page, 1)); // 加入新页面
                }
            }
        }
        print("FIFO",MAXSIZE,n,effective_times);//输出
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
                if ((int)lru_queue.size() < n) //队列没有满
                lru_queue.push(p_str(page, 1, -1, -1, 0));
                else //队列满了，找到最久没有使用的页面
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
        print("LRU",MAXSIZE,n,effective_times);//输出结果
    }
    void Improved_Clock() 
    {
        init();
        for (int i = 0; i < MAXSIZE; ++i) 
        {
            int t = 0;
            int page = rand() % 400 / 10;
            int m = rand()>>1;
            // 检查 Clock 队列是否命中
            fzy::queue<p_str>temp=clock;
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
            if(!t)
            {
                if ((int)clock.size() < n) clock.push(p_str(page, 1, 1, m));//队列没有满
                else //队列满了，则只能置换
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
        print("Improved_Clock",MAXSIZE,n,effective_times);//输出
    }
};
int main() 
{
    int n;
    srand((unsigned)time(NULL));
    scanf("%d", &n);
    printf("\n");
    storage storage(n);
    storage.FIFO();//先进先出
    storage.LRU();//最近最久未使用
    storage.Improved_Clock();//改进的时钟
    return 0;
}