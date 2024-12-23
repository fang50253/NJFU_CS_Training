#include<stdio.h>
#include<iostream>
//#include<stdexcept>
#include<string.h>
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
            if(size_==0) ;//throw std::runtime_error("stack is empty");//栈为空则抛出异常
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
            if(size_==0) ;//throw std::runtime_error("stack is empty");//如果栈为空则抛出异常
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
        /*
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
        */
    };
    template<typename T>//使用模板类定义优先队列
    class priority_queue{
    public:
        priority_queue() //优先队列的无参数构造函数
        :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(&_compare)
        {
            pt = new T[capacity];
            if(nullptr==pt) ;//throw std::runtime_error("malloc failed");
        }
        priority_queue(int val) //带有默认大小的优先队列
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(&_compare){
            while( capacity < val) capacity <<=1;
            pt = new T[capacity];//申请空间
            if( nullptr == pt) ;//throw std::runtime_error("malloc failed");
            return;
        }
        priority_queue(bool (*cmp)(T&,T&)) //带有比较器函数的优先队列构造函数
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(cmp)
            {
            pt = new T[capacity];
            if( nullptr == pt );//throw std::runtime_error("malloc failed");
            return;
        }
        priority_queue( int val, bool (*cmp)(T&,T&) ) //带有默认大小并且有比较器的优先队列
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(cmp){
            while( capacity < val) capacity <<= 1;//申请一个大于该空间大小的空间
            pt = new T[capacity];
            if( nullptr == pt ) ;//throw std::runtime_error("malloc failed");
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
            if(size_of_priority_queue<0) ;//throw std::runtime_error("queue empty");//优先队列为空则抛出异常
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
            if(fp==NULL) ;//throw std::runtime_error("open file failed");
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
            if(fp==NULL) ;//throw std::runtime_error("open file failed");
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