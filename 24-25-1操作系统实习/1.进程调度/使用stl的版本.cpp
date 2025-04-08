#include<stdio.h>
#include<iostream>
#include<stdexcept>
#include<string.h>
#include<queue>
#define MAX_PROCESS_NUMBER 100
namespace fzy
{
    template<typename T>
    struct queuenode
    {
        T v;
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
    struct priority_queue_node
    {
        T v;
        priority_queue_node<T> *parent;
        priority_queue_node<T> *lchild;
        priority_queue_node<T> *rchild;
    };
    template<typename T>
    class priority_queue{
    private:
    priority_queue_node<T>*root;
    int node_cnt;
    void swap(T &a,T &b)
    {
        T c=a;
        a=b;
        b=c;
    }
    void up(priority_queue_node<T>*node)
    {
        while (node && node->parent && node->v > node->parent->v) 
        {
            swap(node->v, node->parent->v);
            node = node->parent;
        }
    }
    void down(priority_queue_node<T>*node)
    {
        while (node && node->lchild) 
        {
            priority_queue_node<T>*largest = node->lchild;

            if (node->rchild && node->rchild->v > largest->v)
                largest = node->rchild;

            if (node->v >= largest->v) break;

            swap(node->v, largest->v);
            node = largest;
        }
    }
    priority_queue_node<T>*find_last() 
    {
        if (!root) return nullptr;

        int path = node_cnt - 1;  // 使用 0-based 编号
        priority_queue_node<T>*cur = root;

        while (path > 0) 
        {
            if (path & 1) cur = cur->rchild;
            else cur = cur->lchild;
            path >>= 1;
        }
        return cur;
    }
    public:
    priority_queue()
    {
        root=NULL;
        node_cnt=0;
    }
    ~priority_queue()
    {
        while(!empty()) pop();
    }
    void push(T value)
    {
        ++node_cnt;
        priority_queue_node<T>*new_node = new priority_queue_node<T>{value, nullptr, nullptr, nullptr};

        if (!root) 
        {
            root = new_node;
            return;
        }

        // 找到插入节点的父节点
        priority_queue_node<T>*last_parent = find_last();
        new_node->parent = last_parent;

        if (!last_parent->lchild)
            last_parent->lchild = new_node;
        else
            last_parent->rchild = new_node;

        // 调整堆
        up(new_node);
    }
    void pop()
    {
        if(empty())throw std::runtime_error("Priority queue is empty.");
        if(node_cnt==1)
        {
            delete root;
            root=nullptr;
            --node_cnt;
            return;
        }
        priority_queue_node<T>*last=find_last();
        swap(root->v,last->v);
        if(last->parent->rchild==last)last->parent->rchild=nullptr;
        else last->parent->lchild=nullptr;
        delete last;
        --node_cnt;
        down(root);
    }
    T top()
    {
        if(empty())throw std::runtime_error("Priority queue is empty.");
        return root->v;
    }
    bool empty()
    {
        return node_cnt==0;
    }
    int size()
    {
        return node_cnt;
    }
    };
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
    class RR
    {
        private:

        public:

    };
    class SJF
    {
        private:
        std::priority_queue<Process>process;
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
        SJF()
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
                    if(tmp.time_arrive<time)
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
                    if(tmp.time_arrive<time)
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
                else ++time;
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
            for(int i=1;i<=finished_index;++i)
            {
                printf("%c\t",finished[i].process_name);//输出进程名称
                printf("%d\t",finished[i].time_end);//输出进程完成时间
                printf("%d\t",finished[i].time_turnaround);//周转时间
                printf("%.2lf\t\n",finished[i].time_turnaround_rights);//带权周转时间
            }
        }
    };
}
int main()
{
    os::SJF *sjf=new os::SJF();
    sjf->conduct();
    sjf->display();
    delete(sjf);
    return 0;
}