#include<stdio.h>
#include<iostream>
#include<stdexcept>
#include<string.h>
#include<queue>
#define MAX_PROCESS_NUMBER 100
namespace fzy
{
    template<class T>
    class Less
    {
    public:
        bool operator()(const T& x, const T& y)
        {
            return x < y;
        }
    };
    template<class T>
    class Greater
    {
    public:
        bool operator()(const T& x, const T& y)
        {
            return x > y;
        }
    };
    template<typename T>
    struct queuenode
    {
        T v=T();
        queuenode<T>* next;
    };
    template<typename T>
    class queue
    {
    private:
        queuenode<T>* head;
        queuenode<T>* last;
        int size_;
    public:
        queue()
        {
            head=new queuenode<T>;
            head->next=nullptr;
            last=head;
            size_=0;
        }
        ~queue()
        {
            clear();
            delete head;
        }
        void clear()
        {
            while(!empty()) pop();
        }
        void push(T v)
        {
            queuenode<T>* newnode = new queuenode<T>;
            newnode->v = v;
            newnode->next = nullptr;
            last->next = newnode;
            last = newnode;
            ++size_;
        }
        void pop()
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
        T front()
        {
            if(size_>0) return head->next->v;
            throw std::runtime_error("Queue is empty");
        }
        T back()
        {
            if(size_>0) return last->v;
            throw std::runtime_error("Queue is empty");
        }
        bool empty()
        {
            return size_==0;
        }
        int size()
        {
            return size_;
        }
        void print()
        //定义为按照队列的入(出)队顺序进行打印
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
    struct stacknode
    {
        T v;
        stacknode *next;
    };
    template<typename T>
    class stack
    {
        private:
        stacknode<T> *head;
        int size_;
        public:
        stack()
        {
            head=new stacknode<T>;
            head->next = nullptr;
            size_=0;
        }
        ~stack()
        {
            clear();
        }
        void clear()
        {
            while(size()) pop();
        }
        void pop()
        {
            if(size_==0) throw std::runtime_error("stack is empty");
            stacknode<T>* tmp=head->next;
            head->next=head->next->next;
            --size_;
            delete tmp;
        }
        int size()
        {
            return size_;
        }
        bool empty()
        {
            return size_==0;
        }
        T top()
        {
            if(size_==0) throw std::runtime_error("stack is empty");
            return head->next->v;
        }
        void push(T v)
        {
            stacknode<T> *newnode=new stacknode<T>;
            newnode->next=head->next;
            head->next=newnode;
            newnode->v=v;
            ++size_;
        }
        void print()
        //定义为按照栈的出栈顺序进行打印
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
    template<typename T>
    class priority_queue{
    public:
        priority_queue()\
        :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(&_compare)
        {
            pt = new T[capacity];
            if(nullptr==pt) throw std::runtime_error("malloc failed");
        }
        priority_queue(int val)\
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(&_compare){
            while( capacity < val) capacity *= 2;
            pt = new T[capacity];//申请空间
            if( nullptr == pt) throw std::runtime_error("malloc failed");
            return;
        }
        priority_queue(bool (*cmp)(T&,T&))\
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(cmp)
            {
            pt = new T[capacity];
            if( nullptr == pt )throw std::runtime_error("malloc failed");
            return;
        }
        priority_queue( int val, bool (*cmp)(T&,T&) )\
            :size_of_priority_queue(0), capacity(MAX_PROCESS_NUMBER), compare(cmp){
            while( capacity < val) capacity *= 2;
            pt = new T[capacity];
            if( nullptr == pt ) throw std::runtime_error("malloc failed");
            return;
        }
        ~priority_queue()
        {
            if( nullptr != pt){
                delete[] pt;
                pt = nullptr;
            }
        }
        bool empty()
        {
            return size_of_priority_queue==0;
        }
        bool push(const T& t)
        {
            T *ptt = pt;
            if( size_of_priority_queue == capacity )
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
            heap_up();
            return true;
        }

        bool pop(){
            if(size_of_priority_queue==0) return false;
            if(size_of_priority_queue==1)
            {
                size_of_priority_queue = 0;
                return true;
            }
            pt[0] = pt[size_of_priority_queue-1];
            size_of_priority_queue--;
            heap_down();
            return true;
        }
        T top()
        {
            if(size_of_priority_queue<0) throw std::runtime_error("queue empty");
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
        void heap_up();
        void heap_down();
        void obj_cpy(T* dest, const T* sour, int n)
        {
            for(int i=0;i<n;i++) dest[i]=sour[i];
        }
        bool static _compare(T &t1, T &t2)
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
    void priority_queue<T>::heap_up()
    {
        T temp;
        int itr = size_of_priority_queue-1;
        while( itr > 0 ){
            if( (compare(pt[itr/2], pt[itr])) )
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
    void priority_queue<T>::heap_down(){
        //当对头出栈韩，需要将队尾数据移动到队头，向下重新调整堆
        T temp;
        int pitr = 0, citr;
        while( pitr <= size_of_priority_queue/2 -1 )
        {
            citr = pitr * 2 + 1;
            if( citr + 1 < size_of_priority_queue && compare(pt[citr], pt[citr+1]))
                citr ++;
            if( (compare(pt[pitr], pt[citr])) )
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
namespace os
{
    class Process
    {
        public:
        char process_name;//进程名称
        int time_arrive;//到达时间
        int time_serve;//服务时间
        bool operator < (const Process &W) const 
        {
            return time_arrive>W.time_arrive;
        }
        bool operator <= (const Process &W) const 
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
        Process(char process_name,int time_arrive,int time_serve)
        {
            this->process_name=process_name;
            this->time_arrive=time_arrive;
            this->time_serve=time_serve;
        }
        Process()
        {
            this->process_name=0;
            this->time_arrive=0;
            this->time_serve=0;
        }
    };
    class Process_finish:public Process
    {
        public:
        Process_finish(Process a,int time_end,int time_turnaround,double time_turnaround_rights)
        {
            this->process_name=a.process_name;
            this->time_arrive=a.time_arrive;
            this->time_serve=a.time_serve;
            this->time_end=time_end;
            this->time_turnaround=time_turnaround;
            this->time_turnaround_rights=time_turnaround_rights;
        }
        Process_finish()
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
        bool operator < (const Process &W) const 
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
    }finished[MAX_PROCESS_NUMBER];
    int finished_index;
    class Process_remain:public Process
    {
        public:
        int remain;
        Process_remain(class Process p)
        {
            this->process_name=p.process_name;
            this->time_arrive=p.time_arrive;
            this->time_serve=p.time_serve;
            this->remain=p.time_serve;
        }
        Process_remain()
        {
            this->remain=0;
        }
        bool operator < (const Process &W) const 
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
    class RR
    {
        private:
        fzy::priority_queue<Process>process;
        fzy::queue<Process_remain>doing_process;
        int process_cnt;
        int time;
        public:
        void read(char filename[])
        {
            FILE *fp=fopen(filename,"r+");
            if(fp==NULL) throw std::runtime_error("open file failed");
            fscanf(fp,"%d",&process_cnt);
            for(int i=1;i<=process_cnt;++i)
            {
                char process_name;
                int time_arrive,time_serve;
                fscanf(fp," %c%d%d",&process_name,&time_arrive,&time_serve);
                process.push(Process(process_name,time_arrive,time_serve));
            }
            fclose(fp);
        }
        RR()
        {
            process_cnt=0;
            finished_index=0;
            char filename[]="filename.dat";
            read(filename);
            memset(finished,0,sizeof finished);
            finished_index=0;
        }
        void conduct()
        {
            time=0;
            Process_remain doing;
            while(finished_index!=process_cnt)
            {
                Process top_process;
                if(!process.empty())
                {
                    top_process=process.top();
                    if(top_process.time_arrive<=time)
                    {
                        process.pop();
                        doing_process.push(Process_remain(top_process));
                        //printf("push:%c\n",top_process.process_name);
                    }
                }
                doing=doing_process.front();
                doing_process.pop();
                --doing.remain;
                //printf("%c",doing.process_name);
                printf("第%d-%d秒，执行程序%c,剩余%d/%d\n",time,time+1,doing.process_name,doing.remain,doing.time_serve);
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
                if(doing.remain==0)
                {
                    ++finished_index;
                    //printf("第%d-%d秒，执行程序%c,剩余%d/%d\n",time,time+1,doing.process_name,doing.remain,doing.time_serve);
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
        void display()
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
    class SJF
    {
        private:
        public:
        void read(char filename[])
        {
            FILE *fp=fopen(filename,"r+");
            if(fp==NULL) throw std::runtime_error("open file failed");
            fscanf(fp,"%d",&process_cnt);
            for(int i=1;i<=process_cnt;++i)
            {
                char process_name;
                int time_arrive,time_serve;
                fscanf(fp," %c%d%d",&process_name,&time_arrive,&time_serve);
                process.push(Process(process_name,time_arrive,time_serve));
            }
            fclose(fp);
        }
        int process_cnt;
        fzy::priority_queue<Process>process;
        fzy::priority_queue<Process_remain>doing_process;
        public:
        SJF()
        {
            process_cnt=0;
            finished_index=0;
            char filename[]="filename.dat";
            read(filename);
            memset(finished,0,sizeof finished);
            finished_index=0;
        }
        void conduct()
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
                printf("第%d-%d秒，执行程序%c,剩余%d/%d\n",time,time+1,doing.process_name,doing.remain,doing.time_serve);
                --doing.remain;
                ++time;
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
        void display()
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
    class SPF
    {
        private:
        fzy::priority_queue<Process>process;
        int process_cnt;
        public:
        void read(char filename[])
        {
            FILE *fp=fopen(filename,"r+");
            if(fp==NULL) throw std::runtime_error("open file failed");
            fscanf(fp,"%d",&process_cnt);
            //printf("%d",process_cnt);
            for(int i=1;i<=process_cnt;++i)
            {
                char process_name;
                int time_arrive,time_serve;
                fscanf(fp," %c%d%d",&process_name,&time_arrive,&time_serve);
                
                Process *tmp;
                tmp=new Process(process_name,time_arrive,time_serve);
                process.push(*tmp);
                //printf("i=%d;Read process: %c %d %d\n", i,process_name, time_arrive, time_serve);
                delete tmp;
                //printf("i=%d;Read process: %c %d %d\n", i,process_name, time_arrive, time_serve);
            }
            //printf("out");
            fclose(fp);
        }
        SPF()
        {
            process_cnt=0;
            char filename[]="filename.dat";
            read(filename);
            memset(finished,0,sizeof finished);
            finished_index=0;
        }
        void conduct()
        {
            int time=0;
            while(!process.empty())
            {
                //printf("time:%d",time);
                std::priority_queue<Process_finish>temp;
                while(!process.empty())
                {
                    auto tmp=process.top();
                    if(tmp.time_arrive<=time)
                    {
                        Process_finish *insert=new Process_finish(tmp,0,0,0.0);
                        temp.push(*insert);
                        process.pop();
                        delete(insert);
                    }
                    else break;
                }
                if(!temp.empty())//说明有程序可以被执行
                {
                    auto tmp=temp.top();
                    if(tmp.time_arrive<=time)
                    {
                        time+=tmp.time_serve;
                        tmp.time_end=time;
                        tmp.time_turnaround=time-tmp.time_arrive;
                        tmp.time_turnaround_rights=1.0*tmp.time_turnaround/tmp.time_serve;
                        ++finished_index;
                        finished[finished_index]=tmp;
                        temp.pop();
                    }
                }
                else 
                {
                    ++time;
                    printf("?");
                }
                while(!temp.empty())
                {
                    auto out=temp.top();
                    temp.pop();
                    Process *a=new Process(out.process_name,out.time_arrive,out.time_serve);
                    process.push(*a);
                    delete(a);
                }
            }
        }
        void display()
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
    printf("\n\nThis is SPF:\n\n\n");
    os::SPF *spf=new os::SPF();
    spf->conduct();
    spf->display();
    spf->display_avergae();
    delete spf;
    printf("\n\n\n");
    printf("\n\nThis is RR:\n\n\n");//使用新来进程优先的算法
    os::RR *rr=new os::RR();
    rr->conduct();
    rr->display();
    rr->display_avergae();
    delete(rr);
    printf("\n\n\n");
    printf("\n\nThis is SJF:\n\n\n");//使用新来进程优先的算法
    os::SJF *sjf=new os::SJF();
    sjf->conduct();
    sjf->display();
    sjf->display_avergae();
    delete(sjf);
    printf("\n\n\n");
    return 0;
}