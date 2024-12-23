#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#define MAX_BUFFER 5
int buffer[MAX_BUFFER];//缓冲
int in=0,out=0;
sem_t s1;//控制缓冲区未满
sem_t s2;//控制缓冲区非空
sem_t mutex;//控制对临界区的访问
void *producer(void *arg)//生产者进程
{
    int item;
    while(1) 
    {
        item=rand()%100+1;
        printf("Producer produced in %d:%d\n",in,item);
        sem_wait(&s1); 
        sem_wait(&mutex);
        buffer[in]=item;
        in=(in+1)%MAX_BUFFER;
        sem_post(&mutex);
        if(in==out) sem_post(&s2);
        sleep(rand()%2);
    }
    return NULL;
}
void *consumer(void *arg)//消费者进程
{
    int item;
    while(1) 
    {
        sem_wait(&s2);
        sem_wait(&mutex);
        item=buffer[out];
        out=(out+1)%MAX_BUFFER;
        printf("Consumer consumed in %d:%d\n",out,item);
        sem_post(&mutex);
        if((in+1)%MAX_BUFFER==out) sem_post(&s1);
        sleep(rand()%2);
    }
    return NULL;
}
int main() 
{
    pthread_t prod,cons;
    sem_init(&s1,0,MAX_BUFFER);//初始化缓冲区未满信号量
    sem_init(&s2,0,0);//初始化缓冲区非空信号量
    sem_init(&mutex,0,1);//初始化临界区访问信号量
    //for(int i=1;i<=3;++i)//只运行3次
    pthread_create(&prod,NULL,producer,NULL);//创建生产者进程
    sleep(2);
    pthread_create(&cons,NULL,consumer,NULL);//创建消费者进程
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    sem_destroy(&s1);//删除信号量
    sem_destroy(&s2);
    sem_destroy(&mutex);
    return 0;
}