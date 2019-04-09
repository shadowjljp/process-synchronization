#include "buffer.h"
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <semaphore.h>

buffer_item buffer[BUFFER_SIZE];
int count_write= 0;
int count_reader= 0;
int count= 0;
pthread_mutex_t mutex;//宣告一個互斥鎖
sem_t full;//宣告semaphore
sem_t empty;


int insert_item(buffer_item item){
  if (count>= 5)
    return -1;
  buffer[count_write% BUFFER_SIZE]= item;
  count_write++;
  count++;
  return 0;
}

int remove_item(buffer_item *item){
  if (count<= 0)
    return -1;
  *item= buffer[count_reader % BUFFER_SIZE];
  count_reader++;
  count--;
  return 0;
}

void *producer(void *param){
  buffer_item item;
  while(true){
    sleep(rand()% 10 +1);
    item= rand();
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    if(insert_item(item))
      std::cout << "Error, not enough space to insert." << std::endl;
    else
      std::cout << "producer produced " << item << std::endl;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}
void *consumer(void *param){
  buffer_item item;
  while(true){
    sleep(rand()% 10 +1);
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    if(remove_item(&item))
      std::cout << "Error, no full space to remove." << std::endl;
    else
      std::cout << "consumer consumed " << item << std::endl;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}

int main(int argc, char *argv[])
{
  pthread_mutex_init(&mutex, NULL);//初始化互斥鎖，第二個參數為設定屬性在此則不設定
  sem_init(&full, 0, 0);//初始化訊號，第二個參數=0代表這個訊號只能在這個行程使用，第三個參數設定訊號初始值
  sem_init(&empty, 0, 5);
  int sleep_time= atoi(argv[1]);
  int writer_num= atoi(argv[2]);
  int reader_num= atoi(argv[3]);

  pthread_t writer[writer_num];
  pthread_t reader[reader_num];
  for (int i= 0; i< writer_num; i++){
    pthread_create(&writer[i], NULL, producer, NULL);//詳見4-16
  }
  for (int i= 0; i< reader_num; i++){
    pthread_create(&reader[i], NULL, consumer, NULL);
  }

  sleep(sleep_time);

  for (int i= 0; i< writer_num; i++){
    pthread_cancel(writer[i]);//取消執行續
  }
  for (int i= 0; i< reader_num; i++){
    pthread_cancel(reader[i]);
  }

  return 0;
}
