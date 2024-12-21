# 一.进程调度 
## 1.1实验内容
* 实验目的：加深对进程调度的理解，熟悉进程调度的不同算法，比较其优劣性。
* 实验内容：

（假如一个系统中有5个进程，它们的到达时间内如表1所示，忽略I/O以及其他开销时间。若分别按抢占的短作业优先（SJF）、时间片轮转（RR，时间片=1）进行CPU调度，请按照上述2个算法，编程计算出各进程的完成时间内、周转时间、带权周转周期、平均周转周期和平均带权周转时间。

| 进程 | 到达时间 | 服务时间 |
|------|----------|----------|
| A    | 0        | 3        |
| B    | 2        | 6        |
| C    | 4        | 4        |
| D    | 6        | 5        |
| E    | 8        | 2        |

## 1.2算法描述
* 算法1:抢占的短作业优先(SJF)
SJF算法是以作业的长短来计算优先级，作业越短，其优先级越高。作业的长短是以作业所要求的运行时间来衡量的。SJF算法可以分别用于作业调度和进程调度。在把段作业优先调度算法用于作业调度时，它将从外存的作业后备队列中选择若干个估计运行时间最短的作业，优先将他们调入内存中运行。
SJF可以有多种写法，一种是采取新进入的进程优先的写法，另一种是上一次运行的进程优先。我认为新进入的进程优先的写法更符合程序运行的规则，所以本次实习中我采用新进入的进程优先的办法来写。
* 算法2:时间便流转(RR，时间片=1)
在轮转(RR)法中，系统根据FCFS策略，将所有的就绪进程排列成一个就绪队列，并可设置每隔一定时间间隔(比如30ms)完成一次中断，激活系统中的进程调度程序，完成一次调度，将CPU分配给队首进程，令其执行。当该新进程的时间片耗尽或运行完毕时，系统再次将CPU分配给心得队首进程(或者新到达的紧迫进程)。由此，可保证就绪队列中的所有进程在一个确定的时间段内，都能获得一次CPU执行。
## 1.3实验结果
实验需要在运行程序相同目录下放置文件：filename.dat，文件内容如下：
```
5
A 0 3
B 2 6
C 4 4
D 6 5
E 8 2
```
第一种SJF抢占的短作业优先调度算法，新进入的进程的优先级别更高，存在如下执行日志：
```
第0-1秒，执行程序A,剩余2/3
第1-2秒，执行程序A,剩余1/3
第2-3秒，执行程序A,剩余0/3
第3-4秒，执行程序B,剩余5/6
第4-5秒，执行程序C,剩余3/4
第5-6秒，执行程序C,剩余2/4
第6-7秒，执行程序C,剩余1/4
第7-8秒，执行程序C,剩余0/4
第8-9秒，执行程序E,剩余1/2
第9-10秒，执行程序E,剩余0/2
第10-11秒，执行程序D,剩余4/5
第11-12秒，执行程序D,剩余3/5
第12-13秒，执行程序D,剩余2/5
第13-14秒，执行程序D,剩余1/5
第14-15秒，执行程序D,剩余0/5
第15-16秒，执行程序B,剩余4/6
第16-17秒，执行程序B,剩余3/6
第17-18秒，执行程序B,剩余2/6
第18-19秒，执行程序B,剩余1/6
第19-20秒，执行程序B,剩余0/6
```
对于第二种RR时间片轮转，采用新进入进程优先的模式，存在以下运行日志：
```
第0-1秒，执行程序A,剩余2/3
第1-2秒，执行程序A,剩余1/3
第2-3秒，执行程序B,剩余5/6
第3-4秒，执行程序A,剩余0/3
第4-5秒，执行程序B,剩余4/6
第5-6秒，执行程序C,剩余3/4
第6-7秒，执行程序B,剩余3/6
第7-8秒，执行程序D,剩余4/5
第8-9秒，执行程序C,剩余2/4
第9-10秒，执行程序B,剩余2/6
第10-11秒，执行程序E,剩余1/2
第11-12秒，执行程序D,剩余3/5
第12-13秒，执行程序C,剩余1/4
第13-14秒，执行程序B,剩余1/6
第14-15秒，执行程序E,剩余0/2
第15-16秒，执行程序D,剩余2/5
第16-17秒，执行程序C,剩余0/4
第17-18秒，执行程序B,剩余0/6
第18-19秒，执行程序D,剩余1/5
第19-20秒，执行程序D,剩余0/5
```
此程序在macOS14.6.1系统，采用如下编译指令：
```
g++ -std=c++14 01Process_submit.cpp -o 01Process_submit 
```
运行结果的截图如下：
![图片 1.png](https://cdn.acwing.com/media/article/image/2024/12/21/383173_eece6606bf-图片-1.png) 
## 1.4实现小结
* 在本次实习的过程中，我原本写的答案是非抢占式的，也就是SPF(相关代码我将会打包放在文件夹下，在本实习报告中不再展现)，后来发现这样的结果和其他同学的有些许偏差，于是我重新检查自己的思路，发现代码中存在的问题，重新编程，写了SJF和RR算法。
* 由于实习过程中不可以使用C\+\+的标准STL库，因此在本次实习中我通过C\+\+模板类创建了queue、stack和priority_queue三种模板类，queue用于RR，priority_queue用于到时间以后将进程加入就绪态，以及SJF算法的实现。
## 1.5实验代码
```
#include<stdio.h>
#include<iostream>
#include<stdexcept>
#include<string.h>
#include<queue>
#define MAX_PROCESS_NUMBER 100
namespace fzy//创建命名空间fzy，用于数据结构的实现
{
    template<class T>//创建模板类
    class Less//创建比较函数
    {
    public:
        bool operator()(const T& x, const T& y)
        {
            return x < y;
        }
    };
    template<class T>
    class Greater//创建比较函数
    {
    public:
        bool operator()(const T& x, const T& y)
        {
            return x > y;
        }
    };
    template<typename T>
    struct queuenode//创建队列的节点
    {
        T v=T();
        queuenode<T>* next;
    };
    template<typename T>//创建队列数据结构
    class queue
    {
    private:
        queuenode<T>* head;
        queuenode<T>* last;
        int size_;
    public:
        queue()//队列的构造函数
        {
            head=new queuenode<T>;
            head->next=nullptr;
            last=head;
            size_=0;
        }
        ~queue()//队列的析构函数
        {
            clear();
            delete head;
        }
        void clear()//清空队列
        {
            while(!empty()) pop();
        }
        void push(T v)//压入队列
        {
            queuenode<T>* newnode = new queuenode<T>;
            newnode->v = v;
            newnode->next = nullptr;
            last->next = newnode;
            last = newnode;
            ++size_;
        }
        void pop()//弹出队列
        {
            if (size_>0)
            {
                queuenode<T>* temp=head->next;
                head->next=temp->next;
                if (head->next==nullptr) last=head;
                delete temp;
                --size_;
            }
        }
        T front()//取队首
        {
            if(size_>0) return head->next->v;
            throw std::runtime_error("Queue is empty");//如果队列为空则抛出异常
        }
        T back()//取队尾
        {
            if(size_>0) return last->v;
            throw std::runtime_error("Queue is empty");//如果队列为空则抛出异常
        }
        bool empty()//队列的判空操作
        {
            return size_==0;
        }
        int size()//返回队列的长度
        {
            return size_;
        }
        void print()//定义为按照队列的入(出)队顺序进行打印
        {
            queuenode<T> *i;
            int j;
            for(i=head->next,j=1;j<=size_;++j,i=i->next)
            {
                std::cout<<i->v<<" ";
            }
            std::cout<<std::endl;
        }
    };
    template<typename T>
    struct stacknode//定义为栈的结点
    {
        T v;//使用模板类使其支持任意类型
        stacknode *next;
    };
    template<typename T>//模板类
    class stack
    {
        private:
        stacknode<T> *head;//链栈的头结点
        int size_;
        public:
        stack()//链栈的构造方法
        {
            head=new stacknode<T>;//创建头结点
            head->next = nullptr;
            size_=0;
        }
        ~stack()//栈的析构函数
        {
            clear();
        }
        void clear()//清空链栈
        {
            while(size()) pop();
        }
        void pop()//将栈顶元素弹出
        {
            if(size_==0) throw std::runtime_error("stack is empty");//栈为空则抛出异常
            stacknode<T>* tmp=head->next;//弹出操作
            head->next=head->next->next;
            --size_;
            delete tmp;//释放内存
        }
        int size()//返回栈的长度
        {
            return size_;
        }
        bool empty()//栈的判空函数
        {
            return size_==0;
        }
        T top()//返回栈顶元素
        {
            if(size_==0) throw std::runtime_error("stack is empty");//如果栈为空则抛出异常
            return head->next->v;
        }
        void push(T v)//压入栈
        {
            stacknode<T> *newnode=new stacknode<T>;
            newnode->next=head->next;
            head->next=newnode;
            newnode->v=v;
            ++size_;
        }
        void print()//定义为按照栈的出栈顺序进行打印
        {
            int j=1;
            stacknode<T> *i;
            for(i=head->next;j<=size_;++j,i=i->next)
            {
                std::cout<<i->v<<" ";
            }
            std::cout<<std::endl;
        }
    };
    template<typename T>//使用模板类定义优先队列
    class priority_queue{
    public:
        priority_queue() //优先队列的无参数构造函数
        :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(&_compare)
        {
            pt = new T[capacity];
            if(nullptr==pt) throw std::runtime_error("malloc failed");
        }
        priority_queue(int val) //带有默认大小的优先队列
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(&_compare){
            while( capacity < val) capacity <<=1;
            pt = new T[capacity];//申请空间
            if( nullptr == pt) throw std::runtime_error("malloc failed");
            return;
        }
        priority_queue(bool (*cmp)(T&,T&)) //带有比较器函数的优先队列构造函数
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(cmp)
            {
            pt = new T[capacity];
            if( nullptr == pt )throw std::runtime_error("malloc failed");
            return;
        }
        priority_queue( int val, bool (*cmp)(T&,T&) ) //带有默认大小并且有比较器的优先队列
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(cmp){
            while( capacity < val) capacity <<= 1;//申请一个大于该空间大小的空间
            pt = new T[capacity];
            if( nullptr == pt ) throw std::runtime_error("malloc failed");
            return;
        }
        ~priority_queue()//优先队列的析构函数
        {
            if( nullptr != pt){
                delete[] pt;
                pt = nullptr;
            }
        }
        bool empty()//优先队列的判空
        {
            return size_of_priority_queue==0;
        }
        bool push(const T& t)//压入优先队列
        {
            T *ptt = pt;
            if( size_of_priority_queue == capacity)
            {
                capacity *= 2;
                pt = new T[capacity];
                if( nullptr == pt )
                {
                    pt = ptt;
                    capacity /= 2;
                    return false;
                }
                obj_cpy(pt, ptt, size_of_priority_queue);
                delete[] ptt;
            }
            pt[size_of_priority_queue++] = t;
            heap_up();//堆堆上传操作
            return true;
        }
        bool pop()
        {
            if(size_of_priority_queue==0) return 0;
            if(size_of_priority_queue==1)
            {
                size_of_priority_queue = 0;
                return 1;
            }
            pt[0] = pt[size_of_priority_queue-1];
            size_of_priority_queue--;
            heap_down();//堆堆下传操作
            return 1;
        }
        T top()
        {
            if(size_of_priority_queue<0) throw std::runtime_error("queue empty");//优先队列为空则抛出异常
            return pt[0];//返回队头元素
        }
        bool is_empty_pl()const
        {
            return 0==size_of_priority_queue;//返回队是否为空
        }
        int get_size()const
        {
            return size_of_priority_queue;//返回队元素个数
        }
        int get_capacity()const
        {
            return capacity;//返回队当前容量应该为2的n次方
        }
    private:
        void heap_up();//定义上传操作
        void heap_down();//定义下传操作
        void obj_cpy(T* dest, const T* sour, int n)//拷贝函数
        {
            for(int i=0;i<n;i++) dest[i]=sour[i];
        }
        bool static _compare(T &t1, T &t2)//定义比较器
        {
            return t1 < t2;
        }
    private:
        T     *pt;//数据
        int    size_of_priority_queue;// 元素个数
        int    capacity;//队容量
        bool (*compare)(T&,T&);//比较函数

    };
    template<typename T>
    void priority_queue<T>::heap_up()//上传操作，基于堆
    {
        T temp;
        int itr = size_of_priority_queue-1;
        while( itr > 0 )
        {
            if( (compare(pt[itr/2], pt[itr])))
            {
                temp = pt[itr];
                pt[itr] = pt[itr/2];
                pt[itr/2] = temp;
                itr = itr/2;
                continue;
            }
            break;
        }
        return;
    }
    template<typename T>
    void priority_queue<T>::heap_down()//下传操作，基于堆
    {
        T temp;
        int pitr = 0, citr;
        while(pitr<=size_of_priority_queue/2-1)
        {
            citr = pitr * 2 + 1;
            if(citr+1<size_of_priority_queue&&compare(pt[citr],pt[citr+1])) ++citr;
            if((compare(pt[pitr],pt[citr])))
            {
                temp = pt[citr];
                pt[citr] = pt[pitr];
                pt[pitr] = temp;
                pitr = citr;//继续将pitr指向孩子节点，进行下一次的比较
                continue;
            }
            break;//如果处在对的位置，直接结束，不需要继续比较下去了
        }
        return;
    }
}
namespace os//定义命名空间os，用于进程调度的书写
{
    class Process//进程，重载运算符实现时间升序排序
    {
        public:
        char process_name;//进程名称
        int time_arrive;//到达时间
        int time_serve;//服务时间
        bool operator < (const Process &W) const //最重要的一个重载，如果使用stl库，那么只需要重载这一个运算符即可
        {
            return time_arrive>W.time_arrive;
        }
        bool operator <= (const Process &W) const //因为算法中使用到了<=号
        {
            return time_arrive>=W.time_arrive;
        }
        bool operator > (const Process &W) const 
        {
            return time_arrive<W.time_arrive;
        }
        bool operator >= (const Process &W) const 
        {
            return time_arrive<=W.time_arrive;
        }
        Process(char process_name,int time_arrive,int time_serve)//带有三个参数的构造函数，用于接受子类的拷贝
        {
            this->process_name=process_name;
            this->time_arrive=time_arrive;
            this->time_serve=time_serve;
        }
        Process()//默认构造函数
        {
            this->process_name=0;
            this->time_arrive=0;
            this->time_serve=0;
        }
    };
    class Process_finish:public Process//继承Process，用于SJF SPF算法中的中间计算过程
    {
        public:
        Process_finish(Process a,int time_end,int time_turnaround,double time_turnaround_rights)//用于接收Process的拷贝
        {
            this->process_name=a.process_name;
            this->time_arrive=a.time_arrive;
            this->time_serve=a.time_serve;
            this->time_end=time_end;
            this->time_turnaround=time_turnaround;
            this->time_turnaround_rights=time_turnaround_rights;
        }
        Process_finish()//默认构造函数，生成类数组
        {
            this->process_name=0;
            this->time_arrive=0;
            this->time_end=0;
            this->time_serve=0;
            this->time_turnaround=0;
            this->time_turnaround_rights=0;
        }
        int time_end;//完成时间
        int time_turnaround;//周转时间
        double time_turnaround_rights;//带权周转时间
        bool operator < (const Process &W) const //重载<
        {
            return time_serve>W.time_serve;
        }
        bool operator <= (const Process &W) const 
        {
            return time_serve>=W.time_serve;
        }
        bool operator > (const Process &W) const 
        {
            return time_serve<W.time_serve;
        }
        bool operator >= (const Process &W) const 
        {
            return time_serve<=W.time_serve;
        }
    }finished[MAX_PROCESS_NUMBER];//使用默认构造函数构造
    int finished_index;
    class Process_remain:public Process
    {
        public:
        int remain;
        Process_remain(class Process p)//构造函数
        {
            this->process_name=p.process_name;
            this->time_arrive=p.time_arrive;
            this->time_serve=p.time_serve;
            this->remain=p.time_serve;
        }
        Process_remain()//构造函数
        {
            this->remain=0;
        }
        bool operator < (const Process &W) const //重载<实现以服务时间升序排序
        {
            return time_serve>W.time_serve;
        }
        bool operator <= (const Process &W) const 
        {
            return time_serve>=W.time_serve;
        }
        bool operator > (const Process &W) const 
        {
            return time_serve<W.time_serve;
        }
        bool operator >= (const Process &W) const 
        {
            return time_serve<=W.time_serve;
        }
    };
    class RR//时间片轮转法
    {
        private:
        fzy::priority_queue<Process>process;//还没有进入就绪状态的进程，采用优先队列对这些进程进行排序
        fzy::queue<Process_remain>doing_process;//正在被执行的进程，在用一个队列去转
        int process_cnt;
        int time;
        public:
        void read(const char filename[])//读取文件
        {
            FILE *fp=fopen(filename,"r+");
            if(fp==NULL) throw std::runtime_error("open file failed");
            fscanf(fp,"%d",&process_cnt);
            for(int i=1;i<=process_cnt;++i)
            {
                char process_name;
                int time_arrive,time_serve;
                fscanf(fp," %c%d%d",&process_name,&time_arrive,&time_serve);
                process.push(Process(process_name,time_arrive,time_serve));//使用临时的类去赋值
            }
            fclose(fp);//文件读取完成
        }
        RR()
        {
            process_cnt=0;
            finished_index=0;
            char filename[]="filename.dat";//构造方法，读取文件
            read(filename);
            memset(finished,0,sizeof finished);
            finished_index=0;
        }
        void conduct()//执行RR时间片轮转
        {
            time=0;
            Process_remain doing;
            while(finished_index!=process_cnt)//在所有进程完成之前
            {
                Process top_process;
                if(!process.empty())//如果进程不空就一直执行下去
                {
                    top_process=process.top();
                    while(top_process.time_arrive<=time)
                    {
                        process.pop();
                        doing_process.push(Process_remain(top_process));
                        //printf("push:%c\n",top_process.process_name);
                        top_process=process.top();//不断读取
                    }
                }
                doing=doing_process.front();
                doing_process.pop();//执行这个进程
                --doing.remain;
                //printf("%c",doing.process_name);
                ++time;
                if(!process.empty())
                {
                    top_process=process.top();
                    while(top_process.time_arrive<=time)
                    {
                        process.pop();
                        doing_process.push(Process_remain(top_process));
                        top_process=process.top();
                        if(process.empty()) break;
                    }
                }
                if(doing.remain==0)//这个进程已经执行完毕
                {
                    ++finished_index;
                    finished[finished_index].process_name=doing.process_name;
                    finished[finished_index].time_arrive=doing.time_arrive;
                    finished[finished_index].time_end=time;
                    finished[finished_index].time_serve=doing.time_serve;
                    finished[finished_index].time_turnaround=time-doing.time_arrive;
                    finished[finished_index].time_turnaround_rights=1.0*finished[finished_index].time_turnaround/doing.time_serve;
                }
                else doing_process.push(doing);//将进程继续送回正在执行的队列
                
            }

        }
        void display()//显示函数
        {
            printf("Process name\t");
            printf("Finished time\t");
            printf("Turnaround time\t");
            printf("Weighted turnaround time\t\n");
            for(int i=1;i<=finished_index;++i)
            {
                printf("|\t%c\t|\t",finished[i].process_name);//输出进程名称
                printf("%d\t|\t",finished[i].time_end);//输出进程完成时间
                printf("%d\t|\t",finished[i].time_turnaround);//周转时间
                printf("%.2lf\t|\n",finished[i].time_turnaround_rights);//带权周转时间
            }
        }
        void display_avergae()
        {
            double average_time_turnaround=0;//计算平均周转时间
            double average_time_turnaround_rights=0;//计算平均带权周转时间
            for(int i=1;i<=finished_index;++i)
            {
                average_time_turnaround+=finished[i].time_turnaround;
                average_time_turnaround_rights+=finished[i].time_turnaround_rights;
            }
            average_time_turnaround/=finished_index;
            average_time_turnaround_rights/=finished_index;
            printf("avergae turnaround time=%.2lf\n",average_time_turnaround);//输出平均周转时间
            printf("average weighted turnaround time=%.2lf",average_time_turnaround_rights);//输出平均带权周转时间
        }
    };
    class SJF//抢占式短进程优先算法
    {
        private:
        void read(const char filename[])//读取文件
        {
            FILE *fp=fopen(filename,"r+");
            if(fp==NULL) throw std::runtime_error("open file failed");
            fscanf(fp,"%d",&process_cnt);
            for(int i=1;i<=process_cnt;++i)
            {
                char process_name;
                int time_arrive,time_serve;
                fscanf(fp," %c%d%d",&process_name,&time_arrive,&time_serve);
                process.push(Process(process_name,time_arrive,time_serve));//创建临时类去压入优先队列中
            }
            fclose(fp);//关闭文件
        }
        int process_cnt;
        fzy::priority_queue<Process>process;
        fzy::priority_queue<Process_remain>doing_process;
        public:
        SJF()//默认构造方法
        {
            process_cnt=0;
            finished_index=0;
            char filename[]="filename.dat";
            read(filename);//读取文件
            memset(finished,0,sizeof finished);
            finished_index=0;
        }
        void conduct()//抢占式短进程优先
        {
            int time=0;
            Process_remain doing;
            while(!(process.empty()&&doing_process.empty()))
            {
                Process top_process;
                if(!process.empty())
                {
                    top_process=process.top();
                    while(top_process.time_arrive<=time)
                    {
                        process.pop();
                        doing_process.push(Process_remain(top_process));
                        top_process=process.top();
                        if(process.empty()) break;
                    }
                }
                
                doing=doing_process.top();
                doing_process.pop();
                --doing.remain;++time;
                if(doing.remain==0)
                {
                    ++finished_index;
                    finished[finished_index].process_name=doing.process_name;
                    finished[finished_index].time_arrive=doing.time_arrive;
                    finished[finished_index].time_end=time;
                    finished[finished_index].time_serve=doing.time_serve;
                    finished[finished_index].time_turnaround=time-doing.time_arrive;
                    finished[finished_index].time_turnaround_rights=1.0*finished[finished_index].time_turnaround/doing.time_serve;
                }
                else doing_process.push(doing);
            }
        }
        void display()//显示
        {
            printf("Process name\t");
            printf("Finished time\t");
            printf("Turnaround time\t");
            printf("Weighted turnaround time\t\n");
            for(int i=1;i<=finished_index;++i)
            {
                printf("|\t%c\t|\t",finished[i].process_name);//输出进程名称
                printf("%d\t|\t",finished[i].time_end);//输出进程完成时间
                printf("%d\t|\t",finished[i].time_turnaround);//周转时间
                printf("%.2lf\t|\n",finished[i].time_turnaround_rights);//带权周转时间
            }
        }
        void display_avergae()//显示平均周转时间和平均带权周转时间
        {
            double average_time_turnaround=0;
            double average_time_turnaround_rights=0;
            for(int i=1;i<=finished_index;++i)
            {
                average_time_turnaround+=finished[i].time_turnaround;
                average_time_turnaround_rights+=finished[i].time_turnaround_rights;
            }
            average_time_turnaround/=finished_index;
            average_time_turnaround_rights/=finished_index;
            printf("avergae turnaround time=%.2lf\n",average_time_turnaround);
            printf("average weighted turnaround time=%.2lf",average_time_turnaround_rights);
        }
    };
}
int main()
{
    printf("\n\nThis is SJF:\n\n\n");//使用新来进程优先的算法
    os::SJF *sjf=new os::SJF();
    sjf->conduct();
    sjf->display();
    sjf->display_avergae();
    delete(sjf);
    printf("\n\n\n");
    printf("\n\nThis is RR:\n\n\n");//使用新来进程优先的算法
    os::RR *rr=new os::RR();
    rr->conduct();
    rr->display();
    rr->display_avergae();
    delete(rr);
    printf("\n\n\n");
    return 0;
}
```
# 三.存储管理
## 3.1实验内容
* 实现目的：通过请求页面式存储管理中页面置换算法设计，了解存储技术的特点，掌握请求页式存储管理的页面置换算法。
* 存储管理：
* 
用程序实现生产者——消费者问题，将指令序列转换为用户虚存中的请求调用页面流。

具体要求：
```
	页面大小为1K
	用户内存容量为4页到40页
	用户外存的容量为40k
```
在用户外存中，按每K存放10条指令，400条指令在外存中的存放方式为：
```
	0-9条指令为第0页
	0-19条指令为第1页
......
	90-399条指令为第39页
```
按以上方式，用户指令可组成40页，通过随机数产生一个指令序列，共400个指令（0-399）。模拟请求页式存储管理中页面置换算法，执行一条指令，首先在外存中查找所对应的页面和页面号，然后将此页面调入内存中，模拟并计算下列三种算法在不同内存容量下的命中率(页面有效次数/页面流的个数): 
```
1.	最久未使用算法(LRU)
2.	改进的Clock置换算法
```
提示
```
•	随机指令的产生 ：rand() 或srand()
•	用户内存中页面控制结构采用链表
struct p_str{ 
int pagenum; /* 页号 */
int count; /* 访问页面的次数 */
struct p_str next; /* 下一指针 */
}p_str;
```
## 3.2算法描述
* 最近最久未使用置换算法(LRU)：LRU算法的基本思想是：当内存空间不足，需要换出一个页面时，选择最近最久没有被访问的页面。即，替换掉在最近一段时间内最少被使用的页面。它假设如果某个页面最近被访问过，那么它在未来一段时间内也可能会被再次访问，因此优先保留最近访问过的页面。这种算法可以改进FIFO性能较差的问题
* 改进的Clock置换算法：将一个页面换出时，如果该页已经被修改过，便需要将该页重新写回磁盘上；但是如果该页未被修改过，则不必将其拷贝回磁盘。换而言之，对于修改过的页面，在换出时所付出的代价比未修改过的页面大。在改进型Clock置换算法中，除了需要考虑页面大使用情况外，还需要考虑增加一个因素——置换代价。这样在页面换出时候，既要是未使用过的页面，也要是未被修改过的页面。把同事满足这两个条件的页面作为首要淘汰的页面，由访问位A和修改未M可以组成以下四种情况：
```
1:(A=0,M=0)表示该页最近既未被访问，又未被修改，是最佳淘汰页
2:(A=0,M=1)表示该页最近未被访问，但已经被修改，并不是很好的淘汰页
3:(A=1,M=0)表示最近已被访问，但未被修改，有可能再次被访问
4:(A=1,M=1)表示最近已被访问且被修改，可能再被访问
```
## 3.3实验结果
* 当页面大小增大时，不同内存容量下的命中率有上升的趋势(但不一定绝对的单调递增)。
* LRU和改进型Clock置换算法的命中率相差不大，但在内存空间较大时，改进型Clock置换算法的效率回略高于LRU。
![图片 1.png](https://cdn.acwing.com/media/article/image/2024/12/21/383173_a82d6d5ebf-图片-1.png) 
## 3.4实现小结
* 在本次实习过程中，我直接使用了之前已经写好的queue类创建实例，但是在拷贝queue的对象的时候发生了错误，经过检查发现是因为在拷贝函数中我使用了浅拷贝，导致只拷贝了原来对象的指针，而原对象执行析构函数该指针成为了野指针，导致了内存的越界访问。通过本次实习我了解了深拷贝和浅拷贝的区别。
![图片 1.png](https://cdn.acwing.com/media/article/image/2024/12/21/383173_c91852c5bf-图片-1.png) 
* LRU算法通过追踪页面的使用顺序，将最久未使用的页面淘汰出去，这种方法简单且直观，但在实现时需要频繁更新每个页面的访问时间，可能导致效率低下。改进型Clock算法则在LRU的基础上进行优化，通过模拟时钟指针来追踪页面的使用状态，每次替换时只需扫描一次页面，减少了频繁更新的开销，提升了性能。
* LRU虽然直观，但效率不高，而改进型Clock算法则通过优化时钟指针的方式提高了性能，展现了算法设计中不断迭代和改进的重要性。
## 3.5实验代码
```
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
            throw std::runtime_error("queue_empty");
        }
        T front() const //返回队列队首
        {
            if (!empty()) return head->next->v;
            throw std::runtime_error("Queue is empty");
        }
        T back() const //返回队列队尾
        {
            if (!empty()) return last->v;
            throw std::runtime_error("Queue is empty");
        }
        bool empty() const //队列判空
        {
            return size_ == 0;
        }
        int size() const //返回队列的长度
        {
            return size_;
        }
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
```