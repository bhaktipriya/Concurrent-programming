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
pthread_cond_t lc[MAX]; 
pthread_cond_t yc[MAX]; 
int lqueue[MAX];
int yqueue[MAX];

pthread_t tid[MAX];
pthread_attr_t attr;
void initializeData() {

	/* Create the mutex lock */
	pthread_mutex_init(&mutex, NULL);
	pthread_attr_init(&attr);
	
	int i;
	
	/* init inn size */
	inn_size = 2;
	inn=0;
	rl=fl=ry=fy=0;
	type=rand()%2;


}

void _enterq(int clan,int id){

	//assuming max persons at a time in a queue cannot exceed MAX
	if(clan==0){
		lqueue[rl]=id;
		rl=(rl+1)%MAX;
	}
	else{
		yqueue[ry]=id;
		ry=(ry+1)%MAX;
	}
}

int  _exitq(int clan){
	int tmp;
	
	if(clan==0){
		tmp=lqueue[fl];
		fl=(fl+1)%MAX;
		
	}
	else{
		tmp=yqueue[fy];
		fy=(fy+1)%MAX;
	}
	return tmp;
}

int  _inviteq(int clan){
	int tmp;
	
	if(clan==0){
		tmp=lqueue[fl];
	}
	else{
		tmp=yqueue[fy];
	}
	return tmp;
}

void *pub(void *param) {

	int clan=rand()%2;
	int id=param,inv;
	
	pthread_mutex_lock(&mutex);

	_enterq(clan,id);
	
	if(inn==inn_size||type!=clan){
	//printf("!innsize=%d\n",inn);
	if(clan==0){
	
		printf("%d:Lacannaster waiting\n",id);
		pthread_cond_wait(&lc[id], &mutex);
	}
	else{
		printf("%d:York waiting\n",id);
		pthread_cond_wait(&yc[id], &mutex);
	}
	
	}
	
	inv=_exitq(clan);
	inn++;
	if(inn==0) 
	{
		type=clan;
		
	}
	
	//printf("innsize=%d,type=%d \n",inn,type);
	if(clan==0)
		printf("%d:Lacannaster entered\n",id);
	else
		printf("%d:York entered\n",id);
	
	//printf("!innsize=%d\n",inn);
	pthread_mutex_unlock(&mutex);


	sleep(rand()%5);

	pthread_mutex_lock(&mutex);
	inn--;	
	//printf("innsize=%d,type=%d \n",inn,type);
	if(clan==0){
		printf("%d:Lacannaster exited\n",id);
		if(rl-fl>0&&inn!=1){
			inv=_inviteq(clan);
			//printf("%d %d\n",inv,id);
			pthread_cond_signal(&lc[inv]);
			}
		else if(ry-fy>0&&!inn){
			inv=_inviteq(clan^1);
			//printf("%d %d\n",inv,id);
			type^=1;
			pthread_cond_signal(&yc[inv]);

		}

	}
	else{
		printf("%d:York exited\n",id);
		if(ry-fy>0&&inn!=1){
			inv=_inviteq(clan);
			//printf("%d %d\n",inv,id);
			pthread_cond_signal(&yc[inv]);
		}
		
		else if(rl-fl>0&&!inn)
		{
			inv=_inviteq(clan^1);
			//printf("%d %d\n",inv,id);
			type^=1;
			pthread_cond_signal(&lc[inv]);
		}
	}
	//printf("!innsize=%d\n",inn);
	pthread_mutex_unlock(&mutex);
}


int main() {
	/* Loop counter */
	int i;
	int *arg = malloc(sizeof(*arg));

	/* Initialize the app */
	initializeData();

	int n=10;
	/* Create the soldier threads */
	for(i = 0; i < n; i++) {
		/* Create the thread */
		arg=i;
		pthread_create(&tid[i],&attr,pub,arg);
	}

	for(i = 0; i < n; i++) {

		pthread_join(tid[i], NULL);
	}

	return 0;
}
