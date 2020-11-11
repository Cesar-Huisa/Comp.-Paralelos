#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUM 20;

int * getRandomVector(int size){
	srand(time(NULL));
	int * res = (int *) malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++){
		res[i] = rand() % MAX_NUM + 1;
	}
	return res;
}

int main(){
	int size = 11;
	int * vector = getRandomVector(size);
	int temp = 0;
	for(int i = 0; i < size; i++){
		printf("%d ", vector[i]);
	}
	printf("\n");
	for(int i = 0; i < size; i++){
		if(i % 2 == 0){
			for(int j = 0; j < size - 1; j+=2){
				if(vector[j] > vector[j+1]){
					temp = vector[j];
					vector[j] = vector[j+1];
					vector[j+1] = temp;
				}
			}
		}
		else{
			for(int j = 1; j < size - 1; j+=2){
				if(vector[j] > vector[j+1]){
					temp = vector[j];
					vector[j] = vector[j+1];
					vector[j+1] = temp;
				}
			}
		}
	}
	for(int i = 0; i < size; i++){
		printf("%d ", vector[i]);
	}
	printf("\n");	
}