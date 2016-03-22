#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>
#define SHMSIZE 1000000

key_t key = IPC_PRIVATE;

void merge(int arr[],int l, int m, int r)
{
	int L[SHMSIZE];
	int R[SHMSIZE];
	int i, j, k;
	int n1 = m - l + 1;
	int n2 =  r - m;


	for(i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for(j = 0; j < n2; j++)
		R[j] = arr[m + 1+ j];

	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}

void selection_sort(int arr[],int l, int r){
	int c,position,d,swap;
	for ( c = l ; c <= ( r - 1 ) ; c++ )
	{
		position = c;

		for ( d = c + 1 ; d <= r; d++ )
		{
			if ( arr[position] > arr[d] )
				position = d;
		}
		if ( position != c )
		{
			swap = arr[c];
			arr[c] = arr[position];
			arr[position] = swap;
		}
	}

}

void mergeSort(int arr[], int l, int r)
{
	if((r-l)<5)
		selection_sort(arr,l,r);

	else 
		if (l < r)
	{
		int m = l+(r-l)/2; 
		int p1,p2,status;

		p1=fork();
		if(p1<0){
			perror("fork");
			exit(1);

		}
		if (p1==0){
			//	printf("hi");
			mergeSort(arr,l, m);
			exit(0);
		}
		else{

			p2=fork();
			if(p2<0){
				perror("fork");
				exit(1);
			}
			if(p2==0){
				mergeSort(arr,m+1, r);
				exit(0);
			}
		}
		waitpid(p1, &status, 0);
		waitpid(p2, &status, 0);
		merge(arr,l, m, r);
	}
}



int main()
{
	int shmid,i,j,k,status,n;
	int *arr;

	if((shmid=shmget(key, sizeof(int) *SHMSIZE, IPC_CREAT| 0666)) < 0)
	{
		perror("Shmget failed");
		exit(1);
	}

	if((arr = shmat(shmid, 0, 0)) == (int *) -1)
	{
		perror("Shmat failed");
		exit(1);
	}

	scanf("%d",&n);
	for(i=0;i<n;i++)
		scanf("%d",&arr[i]);
	mergeSort(arr,0,n-1);
	for(i=0;i<n;i++)
		printf("%d ",arr[i]);
	printf("\n");

	return 0;
}
