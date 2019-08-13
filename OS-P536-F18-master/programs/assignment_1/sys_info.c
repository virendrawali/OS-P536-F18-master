#include<stdio.h>
#include<unistd.h>
#include<string.h>

void main(int nargs, char *args[])
{
	int pid;
	int pipe_handlers[2];
	if(nargs != 2)
	{
		printf("Please check and pass only one argument\n");
		return;
	}
	char *input_string = args[1];
	int value = pipe(pipe_handlers);							//Pipe created for interprocess communication
	int pipe_read_handler = pipe_handlers[0];
	int pipe_write_handler = pipe_handlers[1];
	if(value == -1){
		printf("pipe function failed");
	}
	char read_pipe[20];
	pid = getpid();							
	printf("Parent PID = %d\n",pid);							//Writing in pipe
	write(pipe_write_handler,input_string,20);			
	int cid = fork();									//New process creation
	if(cid == 0)
	{
		printf("Child PID =  %d\n",getpid());
		read(pipe_read_handler,read_pipe,20);						//Reading from pipe
		if(strcmp(read_pipe,"/bin/echo") == 0)						//comparing string
		{
			int exec_flag = execl(read_pipe,read_pipe,"Hello World",NULL);		//Command execution
		        if(exec_flag < 0)
			{
				printf("Wrong command\n");
			}
			printf("\n");
		}
		else
		{
			int exec_flag = execl(read_pipe,read_pipe,NULL);
			if(exec_flag < 0)
                        {
                                printf("Wrong command\n");
				printf("Exec flag:%d", exec_flag);
				return;
                        }
			printf("\n");
		}
	}
	else 
	{
		wait(NULL);
	}
	close(pipe_handlers[0]);
	close(pipe_handlers[1]);
	return;
}
