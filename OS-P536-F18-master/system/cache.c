#include<xinu.h>
#include<xmalloc.h>
#include<stdlib.h>
#include<kv.h>

#define SIZE 100
#define MEM_SIZE 100

struct hash_node hash_array_new[MEM_SIZE];
struct kind_info cache_in[6];
struct LFU lfu_list[SIZE];

/******************************************************************** 
 Function to initialise the cache table, cache information table and 
 to create buffer pools of different sizes(8,16,32,64...etc) which 
 will be used to allocate memory for key and value
********************************************************************/ 

int kv_init()
{
	int i = 0;
	int j = 0;
	xmalloc_init();
	cache_in[0].kind_name = "total_hits";
	cache_in[0].kind_count = 0;
	cache_in[1].kind_name = "total_accesses";
        cache_in[1].kind_count = 0;
	cache_in[2].kind_name = "total_set_success";
        cache_in[2].kind_count = 0;
	cache_in[3].kind_name = "cache_size";
        cache_in[3].kind_count = 0;
	cache_in[4].kind_name = "num_keys";
        cache_in[4].kind_count = 0;
	cache_in[5].kind_name = "total_evictions";
        cache_in[5].kind_count = 0;
	for(i=0;i<MEM_SIZE;i++)
        {
                hash_array_new[i].key = NULL;
                hash_array_new[i].value = NULL;
        }
	for(j=0;j<SIZE;j++)
	{
		lfu_list[j].lfu_key = NULL;
		lfu_list[j].frequency = 0;
	}
	return 1;
}

/*****************************************************************
 Function  to sort the LFU list in ascending order of frequency
 to fetch least recently used page from cache. 
*****************************************************************/

struct LFU* sort()
{
        int i = 0;
        int j = 0;
        struct LFU tmp;
        for (i = 0; i < SIZE; i++)
        {
	 	for (j = 0; j < SIZE; j++)
                {
                        if (lfu_list[j].frequency > lfu_list[i].frequency)
                        {
                                tmp = lfu_list[i];
                                lfu_list[i] = lfu_list[j];
                                lfu_list[j] = tmp;
                        }
                }
        }
        return lfu_list;


}

/******************************************************************
 Function to store key-value pair using buffer pools for memory 
 allocation.
*******************************************************************/

int kv_set(char* input_key,char* input_data) {
   if(strlen(input_data)>1024 || strlen(input_key)>64)
   {
	printf("Key or data greater the required size\n");
	return 1;
   }
   char *data = (char*) xmalloc((int)(strlen(input_data)+1));
   if(data == (char*)SYSERR)
   {
        printf("Buffer pool is full");
        return 1;
   }

   strncpy(data,input_data,strlen(input_data)+1);
   char *key = (char*) xmalloc((int)(strlen(input_key)+1));
    if(key == (char*)SYSERR)
   {
        printf("Buffer pool is full");
        return 1;
   }
   strncpy(key,input_key,strlen(input_key)+1);
   cache_in[3].kind_count = cache_in[3].kind_count + strlen(input_key) + strlen(input_data) + 2;
   int index =0;
   int j = 0;
   int k = 0;
   int flag = 0;
   while(hash_array_new[index].key != NULL)
   {
        if(strncmp(hash_array_new[index].key,key,strlen(input_key)+1)== 0)
        {
                hash_array_new[index].value = data;
		flag = 1;
                break;
        }
        index++;
   }
   if(index == MEM_SIZE)
   {
        struct LFU* temp_lfu = sort();
        bool del = kv_delete(temp_lfu[0].lfu_key);
        if(del)
        {
            for(k=0;k<SIZE;k++)
            {
                    if(hash_array_new[k].key == NULL)
                    {
                        hash_array_new[k].key = key;
                        hash_array_new[k].value = data;
                        break;
                    }
            }
        }
        cache_in[5].kind_count = cache_in[5].kind_count + 1;
        lfu_list[0].lfu_key = key;
        lfu_list[0].frequency = lfu_list[0].frequency + 1;
        flag = 1;
   }

   if(flag == 0)
   {
         hash_array_new[index].key = key;
         hash_array_new[index].value = data;
         for(j=0;j<SIZE;j++)
         {
                if(lfu_list[j].lfu_key == NULL)
                {
                        lfu_list[j].lfu_key = key;
			break;
                }
         }
   }
   cache_in[2].kind_count = cache_in[2].kind_count + 1;
   if(cache_in[4].kind_count < SIZE)
   {
  	 cache_in[4].kind_count = cache_in[4].kind_count + 1;
   }
   return 0;
}

/******************************************************************
 Function to read value from cachefor requested key and return to 
 the used .
*******************************************************************/

char* kv_get(char *key)
{
        int index = 0;
	int j = 0;
        cache_in[1].kind_count = cache_in[1].kind_count + 1;
	while(hash_array_new[index].key!=NULL)
        {
	        if(strncmp(hash_array_new[index].key,key,strlen(key)+1)== 0)
		{
			cache_in[0].kind_count = cache_in[0].kind_count + 1;
			for(j=0;j<SIZE;j++)
               		 {
                        	 if(strncmp(lfu_list[j].lfu_key,key,strlen(key)+1) == 0)
                        	 {
                                	 lfu_list[j].frequency = lfu_list[j].frequency + 1;
                                	 break;
                        	 }
               		 }

			return hash_array_new[index].value;
		}
		index++;
	}
	return NULL;
}

/******************************************************************
 Function to remove key-value entry from cache which has selected by 
 cache replacement policies and free the memory used by key and value
 to buffer pools for reuse.
*******************************************************************/

bool kv_delete(char* key)
{
   int index = 0;
   while(hash_array_new[index].key != NULL) {

      if(strncmp(hash_array_new[index].key,key,strlen(key)+1) == 0)
      {
	 cache_in[3].kind_count = cache_in[3].kind_count - strlen(hash_array_new[index].key) - strlen(hash_array_new[index].value) - 2;
	 xfree(hash_array_new[index].key);
	 xfree(hash_array_new[index].value);
         hash_array_new[index].key = NULL;
         hash_array_new[index].value = NULL;
	 cache_in[4].kind_count = cache_in[4].kind_count - 1;
	 lfu_list[0].lfu_key = NULL;
         lfu_list[0].frequency = 0;
	 return 1;
      }
      index++;
   }
   return 0;
}

void display()
{
   int i = 0;
   for(i = 0; i<MEM_SIZE; i++) {

      if(hash_array_new[i].key != NULL)
         printf(" (%s,%s)",hash_array_new[i].key,hash_array_new[i].value);
      else
        printf(" ~~ ");
   }

   printf("\n");
   for(i = 0; i<SIZE; i++) {

      if(lfu_list[i].lfu_key != NULL)
         printf("Key and frequency (%s,%d)\n",lfu_list[i].lfu_key,lfu_list[i].frequency);
   }
   printf("\n");
}

/*********************************************************************
  Function to reset all the tables like cache table, cache information 
  table and LFU table.
**********************************************************************/

void kv_reset()
{
        int i = 0;
        for(i = 0; i<MEM_SIZE; i++) {

         if(hash_array_new[i].key != NULL)
	 {
		xfree(hash_array_new[i].key);
                xfree(hash_array_new[i].value);
		hash_array_new[i].key = NULL;
                hash_array_new[i].value = NULL;
	
	}
   }
         for(i=0;i<SIZE;i++)
        {
                lfu_list[i].lfu_key = NULL;
                lfu_list[i].frequency = 0;
        }
	for(i = 0; i<6; i++)
  	{
		cache_in[i].kind_count = 0;		
        }
  
}

/******************************************************************
 Function to get cache information like total_hits, total_accesses,
 total_set_success, cache_size, num_keys, total_evictions.
*******************************************************************/

int get_cache_info(char* kind)
{
   int i = 0;
   for(i = 0; i<6; i++) 
   {
	if(cache_in[i].kind_name == kind)
	{
		printf("Get cache info name is:%s\n",cache_in[i].kind_name);
		return cache_in[i].kind_count;
	}
   }
   printf("Kind name wrong\n");
   return 0;
}

/*********************************************************************
  Function to retrieve most frequently accessed pages from cache.
**********************************************************************/

char **most_popular_keys(int k)
{
	int i = 0;
	int j = 0;
	struct LFU tmp;
	char **most_frequent = (char**)getmem(sizeof(char**)*k);
	for (i = 0; i < SIZE; i++)                     
	{
		for (j = 0; j < SIZE; j++)             
		{
			if (lfu_list[j].frequency < lfu_list[i].frequency)                
			{
				tmp = lfu_list[i];
                                lfu_list[i] = lfu_list[j];
				lfu_list[j] = tmp; 
			}  
		}
	}
	int k1 = 0;
	for (k1 = 0; k1 < k; k1++)
        {
		most_frequent[k1] = (char*)getmem(strlen( lfu_list[k1].lfu_key)+1);
		most_frequent[k1] = lfu_list[k1].lfu_key;
	}
	return most_frequent;
}
