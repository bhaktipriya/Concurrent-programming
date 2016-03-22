#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <syscall.h>
typedef int buffer_item;

#define TRUE 1
#define MAX 20
int inn,inn_size;
int rl,fl,ry,fy,type;

/* The mutex lock */
pthread_mutex_t mutex;
sem_t yorks;
sem_t lancs;
int l,y;
pthread_t tid[MAX];
pthread_attr_t attr;
void initializeData() {

	/* Create the mutex lock */
	pthread_mutex_init(&mutex, NULL);
	pthread_attr_init(&attr);
	
	int i;
	
	/* init inn size */
	inn_size = 1;
	inn=0;
	rl=fl=ry=fy=0;
	type=rand()%2;
	if(type==0){
	sem_init(&lancs, 0,inn_size);
	sem_init(&yorks, 0,0);
	}
	else{
	sem_init(&lancs, 0,0);
	sem_init(&yorks, 0,inn_size);
	}
	l=y=0;


}


void *Yorks(void *param) {

	int id=param;
	int i;
	pthread_mutex_lock(&mutex);
	y++;
	//printf("%d:York created,type%d\n",id,type);
	pthread_mutex_unlock(&mutex);
	
	sem_wait(&yorks);
	pthread_mutex_lock(&mutex);
	inn++;
	printf("%d:York entered\n",id);
	pthread_mutex_unlock(&mutex);
	
	sleep(5);

	pthread_mutex_lock(&mutex);
	y--;
	printf("%d:York exited\n",id);
	if(y==0){
		type^=1;
		for(i=0;i<inn_size;++i)
			sem_post(&lancs);
	}
	else
	sem_post(&yorks);
	pthread_mutex_unlock(&mutex);
	sem_post(&yorks);


}
void *Lancs(void *param) {
	

	int id=param;
	int i;
	pthread_mutex_lock(&mutex);
	l++;
	//printf("%d:Lacannaster created,type\n",id,type);
	pthread_mutex_unlock(&mutex);
	
	sem_wait(&lancs);
	pthread_mutex_lock(&mutex);
	inn++;
	printf("%d:Lacannaster entered\n",id);
	pthread_mutex_unlock(&mutex);
	
	sleep(5);
	
	pthread_mutex_lock(&mutex);
	l--;
	printf("%d:Lacannaster exited\n",id);
	if(l==0){
		type^=1;
		for(i=0;i<inn_size;++i)
		sem_post(&yorks);
	}
	else
		sem_post(&lancs);
	pthread_mutex_unlock(&mutex);
		sem_post(&lancs);

}

int main() {
	/* Loop counter */
	int i;
	int *arg = malloc(sizeof(*arg));

	/* Initialize the app */
	initializeData();

	int n=10;
	/* Create the soldier threads */
	
	int clan;
	for(i = 0; i < n; i++) {
		/* Create the thread */
		arg=i;
		clan=rand()%2;
		if(clan)
			pthread_create(&tid[i],&attr,Lancs,arg);
		else
			pthread_create(&tid[i],&attr,Yorks,arg);
	}

	for(i = 0; i < n; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}
