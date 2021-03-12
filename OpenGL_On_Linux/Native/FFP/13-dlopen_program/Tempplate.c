#include<stdio.h>
#include<stdlib.h>
#include<dlfcn.h>

int main(int argc,char **argv){
	void *handle;
	int (*output)(int,int);
	char *error;

	handle=dlopen("./libprogram.so",RTLD_LAZY);
	if(!handle){
		fprintf(stderr,"%s\n",dlerror());
		exit(EXIT_FAILURE);
	}
	

	output=(int (*)(int,int))dlsym(handle,"Addition");

	if((error=dlerror())!=NULL){
		fprintf(stderr,"%s\n",error);
		exit(EXIT_FAILURE);
	}
	printf("Addition is %d\n",(*output)(5,5));
	
	output=(int (*)(int,int))dlsym(handle,"Subtraction");
	if((error=dlerror())!=NULL){
		fprintf(stderr,"%s\n",error);
		exit(EXIT_FAILURE);
	}
	printf("Subtraction is %d\n",(*output)(8,2));
	dlclose(handle);
	handle=NULL;
	exit(EXIT_SUCCESS);
}
