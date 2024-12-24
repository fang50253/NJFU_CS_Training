#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#define MAX_BUFFER 5
int buffer[MAX_BUFFER];//缓冲
int in=0,out=0;
sem_t empty;//控制缓冲区未满
sem_t full;//控制缓冲区非空
sem_t mutex;//控制对临界区的访问
void *producer(void *arg)//生产者进程
{
    int item;
    while(1) 
    {
        item=rand()%100+1;
        sem_wait(&empty); 
        sem_wait(&mutex);
        if((in+1)%MAX_BUFFER!=out)//判断队列非满
        {
            printf("Producer produced in %d:%d\n",in,item);
            buffer[in]=item;
            in=(in+1)%MAX_BUFFER;    
        }
        sem_post(&mutex);
        //if(in==out) 
        sem_post(&full);
        sleep(rand()%2);
    }
    return NULL;
}
void *consumer(void *arg)//消费者进程
{
    int item;
    while(1) 
    {
        sem_wait(&full);
        sem_wait(&mutex);
        if(in!=out)//判断队列非空
        {
            item=buffer[out];
            printf("Consumer consumed in %d:%d\n",out,item);
            out=(out+1)%MAX_BUFFER;
        }
        sem_post(&mutex);
        sem_post(&empty);
        sleep(rand()%3);
    }
    return NULL;
}
int main() 
{
    pthread_t prod,cons;
    sem_init(&empty,0,MAX_BUFFER);//初始化缓冲区未满信号量
    sem_init(&full,0,0);//初始化缓冲区非空信号量
    sem_init(&mutex,0,1);//初始化临界区访问信号量
    //for(int i=1;i<=3;++i)//只运行3次
    pthread_create(&prod,NULL,producer,NULL);//创建生产者进程
    pthread_create(&cons,NULL,consumer,NULL);//创建消费者进程
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    sem_destroy(&empty);//删除信号量
    sem_destroy(&full);
    sem_destroy(&mutex);
    return 0;
}