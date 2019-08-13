#include <xinu.h>
#include <stdio.h>

shellcmd xsh_hello(int nargs,char *args[]) {
	if(nargs>2){
		printf("Too many arguments\n");
	}
	else
	{
		if(nargs == 2)
		{
			printf("Hello %s, welcome to the world of Xinu!!\n",args[1]);
		}
		else
		{
			printf("No argument passed, Please enter argument\n");
		}
	}

	return 0;	
}
