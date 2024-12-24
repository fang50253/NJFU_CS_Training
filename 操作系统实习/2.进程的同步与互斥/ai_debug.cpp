#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_BUFFER 5
int buffer[MAX_BUFFER]; // 缓冲区
int in = 0, out = 0;    // 生产者和消费者的指针

sem_t s1;   // 控制缓冲区未满
sem_t s2;   // 控制缓冲区非空
sem_t mutex; // 控制对缓冲区的访问

void *producer(void *arg) {
    while (1) {
        int item = rand() % 100 + 1; // 随机生成数据
        sem_wait(&s1);              // 等待缓冲区未满
        sem_wait(&mutex);           // 加锁

        // 生产数据
        buffer[in] = item;
        printf("Producer produced in %d: %d\n", in, item);
        in = (in + 1) % MAX_BUFFER;

        sem_post(&mutex);           // 解锁
        sem_post(&s2);              // 增加缓冲区非空信号量

        sleep(rand() % 2); // 随机休眠
    }
    return NULL;
}

void *consumer(void *arg) {
    while (1) {
        sem_wait(&s2);              // 等待缓冲区非空
        sem_wait(&mutex);           // 加锁

        // 消费数据
        int item = buffer[out];
        printf("Consumer consumed in %d: %d\n", out, item);
        out = (out + 1) % MAX_BUFFER;

        sem_post(&mutex);           // 解锁
        sem_post(&s1);              // 增加缓冲区未满信号量

        sleep(rand() % 2); // 随机休眠
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    sem_init(&s1, 0, MAX_BUFFER); // 初始化缓冲区未满信号量
    sem_init(&s2, 0, 0);          // 初始化缓冲区非空信号量
    sem_init(&mutex, 0, 1);       // 初始化互斥信号量

    pthread_create(&prod, NULL, producer, NULL); // 创建生产者线程
    pthread_create(&cons, NULL, consumer, NULL); // 创建消费者线程

    pthread_join(prod, NULL); // 等待生产者线程结束
    pthread_join(cons, NULL); // 等待消费者线程结束

    sem_destroy(&s1); // 销毁缓冲区未满信号量
    sem_destroy(&s2); // 销毁缓冲区非空信号量
    sem_destroy(&mutex); // 销毁互斥信号量

    return 0;
}