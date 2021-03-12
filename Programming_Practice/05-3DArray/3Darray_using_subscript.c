#include<stdio.h>
#include<stdlib.h>
#define SIZE_ROW	3
#define SIZE_COLUMN	2
#define SIZE_RANK	2
int main(void){
	int row,column,rank;
	int arr[SIZE_ROW][SIZE_COLUMN][SIZE_RANK];
	printf("Enter elements in array\n");
	for(row=0;row<SIZE_ROW;row++){
		for(column=0;column<SIZE_COLUMN;column++){
			for(rank=0;rank<SIZE_RANK;rank++){
				scanf("%d",&(arr[row][column][rank]));
			}
		}
	}

	for(row=0;row<SIZE_ROW;row++){
		for(column=0;column<SIZE_COLUMN;column++){
			for(rank=0;rank<SIZE_RANK;rank++){
				printf("%d\t",arr[row][column][rank]);
			}
		}
	}

	printf("\n");	
	exit(EXIT_SUCCESS);
}
