/***********************************************************************
 The task of the program is to read input text file and search the 
 input word and display lines which contain that search word. The program
 is implemented on producer-consumer paradigm, with 1 producer and N
 consumers. User can the program as below:
 
 I) ./pargrep [-t] <number_of_threads> <word_to_search> [<input_file_name>]

 II) cat <input_file_name> | ./pargrep <word_to_search>

***********************************************************************/
#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <stdlib.h>
#include<string.h>
  
sem_t mutex; 
sem_t producer;
sem_t consumer;
int alive = 1;
char **buff;
FILE *pipe_file; 

/************************************************************************
  The task of of consumer is to process the lines provided by producer
  throught buffer and check if search word is preent in input line, if 
  present then display the output otherwise process next line. Semaphore
  is used for synchronisation between producer-consumer and mutex is
  used to avoid race condition.
************************************************************************/

void *readfile(void *line)
{
       char *file_line= (char*)line;
       while(alive)
        {
	   sem_wait(&consumer);								//wait consumers
	   sem_wait(&mutex);
	   if(strstr(buff[0],file_line)!= NULL)						// Check search word in buffer
           {
                printf("%s",buff[0]);
           }
	   sem_post(&mutex);								
	   sem_post(&producer);								// Signal producer
	}
	exit(0);
}

/**************************************************************************
 The task of producer is to read input text file and send data to consumer
 so that consumer can process file datra and display result.
**************************************************************************/

void *prod(void *line)
{
        FILE *input_file;
        char * line1 = NULL;
        ssize_t read_line;
        size_t len = 0;
        int i = 0;
        buff = malloc(10 * sizeof(char*));
	if(line!=NULL)
		input_file = fopen(line,"r");
	else
	{
		input_file = pipe_file;
	}
	while ((read_line = getline(&line1, &len, input_file)) != -1)
        {
                sem_wait(&producer);							//wait producer
                i = 0 ;
                buff[i] = malloc(strlen(line1) + 1);
                strcpy(buff[i],line1);
                i++;
		sem_post(&consumer);							//signal consumer
         }
	 alive = 0;
	 pthread_exit(NULL);
}

/******************************************************************************
 The task of main thread is to initialise semaphores, mutex and create producer
 consumer threads. Secondly, it reads command line arguments and pass required
 data to producer and consumer.  
******************************************************************************/

int main(int nargs, char *args[])
{
    sem_init(&mutex, 0, 1);
    sem_init(&producer, 0, 1);
    sem_init(&consumer, 0, 0);
    int n;
    char *word = NULL;
    char *file_name = NULL;
    if(nargs<2)
    {
           printf("No sufficient arguments\n");
     	   exit(0);
    }
    if((strcmp(args[1],"-t") == 0))
    {
        if(nargs < 5)
	{
		printf("Please check and pass 3 argument(1. Number of threads 2. word to find 3.Input file name)\n");
		exit(0);
	}
	else
	{
        	n = atoi(args[2]);
		word = (char *)args[3];
                file_name = (char *)args[4];
	}
    }
    else
    {
    	if(nargs == 3)
   	 {
		n = 1;
		word = (char *)args[1];
        	file_name = (char *)args[2];
    	}
    	else
    	{
        	if(nargs == 2)
        	{
            		n = 1;
            		word = (char *)args[1];
			pipe_file = stdin;
        	}
		else
		{
	   		 printf("No sufficient arguments\n");
		}

    	}
    }
    pthread_t prod1;
    int i = 0;
    pthread_t cons1;
    pthread_t con_thread[n];
    pthread_create(&prod1,NULL,prod,file_name);							// Producer thread created
    for(i = 1; i<=n; i++)
    {
           pthread_create(&con_thread[i],NULL,*readfile,word);					// Consumer threads created
    }
    for(i = 1; i<=n; i++)
    {
          pthread_join(con_thread[i],NULL);
    }
    pthread_join(prod1,NULL);
    sem_destroy(&mutex);
    sem_destroy(&producer);
    sem_destroy(&consumer);
    return 0;
} 
