#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <string.h>

#define MAX_NUM 20
#define SIN_CAMBIOS 0
#define CON_CAMBIOS 1

int * getRandomVector(int size){
	srand(time(NULL));
	int * res = (int *) malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++){
		res[i] = rand() % MAX_NUM + 1;
	}
	return res;
}

int comp(const void * a, const void * b){
	return (*(int *)a - *(int *) b);
}

void partnerProcess(int * local_vector, int local_n, int my_rank, int * flag){
	MPI_Send(local_vector,local_n,MPI_INT,my_rank-1,0,MPI_COMM_WORLD);
	MPI_Recv(local_vector,local_n,MPI_INT,my_rank-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	MPI_Recv(flag,1,MPI_INT,my_rank-1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	if((*flag) == CON_CAMBIOS) qsort(local_vector,local_n,sizeof(int),comp);
}

void masterProcess(int * local_vector, int * partner_vector, int local_n, int my_rank, int * flag){
	int temp_num = 0;
	int * temp_vector = (int *) malloc(sizeof(int) * local_n);
	MPI_Recv(partner_vector,local_n,MPI_INT,my_rank+1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	if(local_vector[0] > partner_vector[0]){
		memcpy((void *) temp_vector, (void *) partner_vector, sizeof(int) * local_n);
		memcpy((void *) partner_vector, (void *) local_vector, sizeof(int) * local_n);
		memcpy((void *) local_vector, (void *) temp_vector, sizeof(int) * local_n);
	}

	*flag = SIN_CAMBIOS;
	for(int j = 0; j < local_n; j++){
		if(local_vector[local_n-j-1] > partner_vector[j]){
			*flag = CON_CAMBIOS;
			temp_num = partner_vector[j];
			partner_vector[j] = local_vector[local_n-j-1];
			local_vector[local_n-j-1] = temp_num; 
		}
		else break;
	}
	MPI_Send(partner_vector,local_n,MPI_INT,my_rank+1,0,MPI_COMM_WORLD);
	MPI_Send(flag,1,MPI_INT,my_rank+1,1,MPI_COMM_WORLD);
	if((*flag) == CON_CAMBIOS) qsort(local_vector,local_n,sizeof(int),comp);	
	free(temp_vector);
}



int main(int argc, char ** argv){
	if(argc != 2){
		printf("Argumentos Erroneos <vectorSize>\n");
		return 0;
	}
	int vectorSize = atoi(argv[1]);
	int * vector = getRandomVector(vectorSize);
	//int vector[16] = {19 ,5 ,20 ,8 ,10 ,3 ,5 ,14 ,8 ,6 ,12 ,1 ,13 ,19,10,10};

	int * local_vector = NULL;
	int * partner_vector = NULL;
	int flag = SIN_CAMBIOS;
	int my_rank;
	int comm_sz;
	int local_n = 0;
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

	if(my_rank == 0){
		printf("Vector Original->");
		for(int i = 0; i < vectorSize; i++){
			printf("%d ", vector[i]);
		}
		printf("\n");
	}

	local_n = vectorSize / comm_sz;
	local_vector = (int *) malloc(sizeof(int) * local_n);
	partner_vector = (int *) malloc(sizeof(int) * local_n);
	MPI_Scatter(vector,local_n,MPI_INT,local_vector,local_n,MPI_INT,0,MPI_COMM_WORLD);
	qsort(local_vector,local_n,sizeof(int),comp);
	for(int i = 0; i < comm_sz; i++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(i % 2 == 0){
			if(my_rank % 2 != 0){
				partnerProcess(local_vector, local_n, my_rank, &flag);
			}
			else if(my_rank != comm_sz - 1){
				masterProcess(local_vector, partner_vector, local_n, my_rank, &flag);
			}
		}
		else{
			if(my_rank % 2 == 0){
				if(my_rank != 0) partnerProcess(local_vector, local_n, my_rank, &flag);
			}
			else if(my_rank != comm_sz - 1){
				masterProcess(local_vector, partner_vector, local_n, my_rank, &flag);
			}
		}	
	}
	MPI_Gather(local_vector,local_n,MPI_INT,vector,local_n,MPI_INT,0,MPI_COMM_WORLD);
	
	if(my_rank == 0){
		printf("Vector Ordenado->");
		for(int i = 0; i < vectorSize; i++){
			printf("%d ", vector[i]);
		}
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}
