#include<stdio.h>

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
struct buffer_info
{
   int *nodes;
   int node_size;	
};

struct pool_data
{
   int poolid;
   int pool_size;
   struct buffer_info pool_nodes[2000];
   int num_allocated;
   int fragmented_bytes;
   int allocated_bytes;
};

void xmalloc_init() ;
void xfree(void*) ; 
void* xmalloc(int);
void heap_snapshot();

 
