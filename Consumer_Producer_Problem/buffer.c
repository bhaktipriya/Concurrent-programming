#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
//#include "buffer.h"
#include <syscall.h>

typedef int buffer_item;
#define BUFFER_SIZE 10
#define DIV 1000000

#define MXCONSM 20
/* The mutex lock */
pthread_mutex_t mutex;

/* the semaphores */
sem_t full[50];
sem_t empty[50];
/* the buffer */
buffer_item buffer[BUFFER_SIZE];
/* buffer counter */
int fill;

int numProd = 1; /* Number of producer threads */
int numCons=5; /* Number of consumer threads */


pthread_t tid[MXCONSM];       //Thread ID
pthread_t pid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */

void initializeData() {

	/* Create the mutex lock */
	pthread_mutex_init(&mutex, NULL);

	int i;
	/* Create the full semaphore and initialize to 0 */
	for(i=0;i<=50;++i)
	{
		sem_init(&empty[i], 0, BUFFER_SIZE);
		sem_init(&full[i], 0, 0);
	}

	/* Create the empty semaphore and initialize to BUFFER_SIZE */
	//sem_init(&empty, 0, BUFFER_SIZE);

	/* Get the default attributes */
	pthread_attr_init(&attr);

	fill=0;
}

void put(int value){
	buffer[fill]=value;
	fill=(fill+1)%BUFFER_SIZE;
}
int  get(int *use){
	int temp=buffer[*use];
	*use=(*use+1)%BUFFER_SIZE;
	return temp;
}

void *producer(void *param) {

	int i,j;
	buffer_item item;
	int tmp;
	for(i=0;i<15;++i){
		item=rand()/DIV;
		for(j=0;j<numCons;++j)
			sem_wait(&empty[j]);
		pthread_mutex_lock(&mutex);
		tmp=fill;
		put(item);
		printf("producer produced %d @ %d \n",item,tmp);
		pthread_mutex_unlock(&mutex);
		for(j=0;j<numCons;++j)
			sem_post(&full[j]);
	}

}

void *consumer(void *param) {

	buffer_item item;
	int  tmp=0,a[20];
	int tid=(int)param;
	int use=0;
	int i=0,j;
	for(i=0;i<15;++i){
	sem_wait(&full[tid]);
	pthread_mutex_lock(&mutex);
	tmp=use;
	item=get(&use);
	a[i]=item;
	printf("consumer[%d] counsumed %d @ %d \n",tid,item,tmp);
	if(i==14)
	{	for(j=0;j<15;++j)
		printf("%d ",a[j]);
	printf("\n");
	}

	pthread_mutex_unlock(&mutex);
	sem_post(&empty[tid]);
	
	}
	
}
int main() {
	/* Loop counter */
	int i;
	int *arg = malloc(sizeof(*arg));

	/* Initialize the app */
	initializeData();

	/* Create the producer threads */
	for(i = 0; i < numProd; i++) {
		/* Create the thread */
		pthread_create(&pid,&attr,producer,NULL);
	}

	/* Create the consumer threads */
	for(i = 0; i < numCons; i++) {
		/* Create the thread */
		arg=i;
		pthread_create(&tid[i],&attr,consumer,arg);
	}

	pthread_join(pid, NULL);
	for(i = 0; i < numCons; i++) {

		pthread_join(tid[i], NULL);
	}

	return 0;
}
