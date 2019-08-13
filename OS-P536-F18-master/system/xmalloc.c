/******************************************************************
The program is to implement memory allocation functionality in Xinu.
Using buffer pool of diffrent size we are allocating memory for user
request and freeing the memory after use. 
******************************************************************/

#include<xinu.h>
#include<xmalloc.h>
#include<stdlib.h>
#define SIZE 2000

/* created 8 bufferpools of size 8,16,32,64 etc */

struct pool_data pool_node[11];

/*****************************************************************
 Function to create buffers of different sizes(8,16,32,64 etc) and
 to initialise all attributes corresponding to buffer pool.
****************************************************************/

void xmalloc_init()
{
    int32 i = 8;
    int j = 0;
    int addr_cnt = 0;
    while(i<=8192)
    {
	addr_cnt = 0;
	pool_node[j].poolid = mkbufpool(i,SIZE);
	pool_node[j].pool_size = i;
	while(addr_cnt<SIZE)
	{
		pool_node[j].pool_nodes[addr_cnt].nodes = NULL;
		pool_node[j].pool_nodes[addr_cnt].node_size = 0;	
		addr_cnt++;
	}
  	pool_node[j].num_allocated = 0;
	pool_node[j].allocated_bytes = 0;
	pool_node[j].fragmented_bytes = 0;
        i= i*2;
	j = j+1;
    }
}

/******************************************************************
 Function is to get memory block from bufer pool and return memory
 address to user request.
******************************************************************/

void* xmalloc(int size)
{
	int num = 8;
	int count = 0;
	char *buff;
	if(size>8192)
	{
		printf("Requested memory is greater than available memory\n");
		return NULL;
	}
	while(num<=size)
	{
		num = num * 2;
		count = count+1;
	}

	//get meory block from buffer pool

	buff = getbuf(pool_node[count].poolid);
	if(buff == (char*)SYSERR)
	{
		printf("Buffer pool for %d is full",num);
		return NULL;
	}
	//maintain values of attributes for pools
	pool_node[count].pool_nodes[pool_node[count].num_allocated].nodes = (int*)buff;
	pool_node[count].pool_nodes[pool_node[count].num_allocated].node_size = size; 
	pool_node[count].num_allocated =  pool_node[count].num_allocated + 1;
	pool_node[count].allocated_bytes = pool_node[count].allocated_bytes + size;
	pool_node[count].fragmented_bytes = pool_node[count].fragmented_bytes + (num-size);
	return (void*)buff;
}

/*******************************************************************
 Function is to free memory block and reassign the block in buffer
 pool for reuse.
*******************************************************************/

void xfree(void *ptr)
{
	int count = 0;
	int addr_cnt = 0;
	while(count<=11)
	{
		addr_cnt = 0;
		while(addr_cnt<SIZE)
		{
			if((int*)pool_node[count].pool_nodes[addr_cnt].nodes == (int*)ptr)
			{
				freebuf((char*)pool_node[count].pool_nodes[addr_cnt].nodes);
				pool_node[count].pool_nodes[addr_cnt].nodes = NULL;
				pool_node[count].num_allocated = pool_node[count].num_allocated - 1;
				pool_node[count].allocated_bytes = pool_node[count].allocated_bytes - pool_node[count].pool_nodes[addr_cnt].node_size;
				pool_node[count].fragmented_bytes = pool_node[count].fragmented_bytes - (pool_node[count].pool_size - pool_node[count].pool_nodes[addr_cnt].node_size);
				pool_node[count].pool_nodes[addr_cnt].node_size = 0;
				count = 8;
				break;
			}
			addr_cnt++;
		}
		count++;
	}

}

/********************************************************************
 Function is to show the current status of heap
********************************************************************/

void xheap_snapshot()
{
  int count = 0;
  while(count<=10)
  {
	if(pool_node[count].num_allocated == 0)
	{
		count++;
		continue;
	}
  	printf("pool_id=%d, buffer_size=%d, total_buffers=%d, allocated_bytes=%d, allocated_buffers=%d, fragmented_bytes=%d\n", pool_node[count].poolid,pool_node[count].pool_size,20,pool_node[count].allocated_bytes,pool_node[count].num_allocated,pool_node[count].fragmented_bytes);
	count++;
  }
}
