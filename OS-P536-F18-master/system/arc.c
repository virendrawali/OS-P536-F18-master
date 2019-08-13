#include<xinu.h>
#include<xmalloc.h>
#include<stdlib.h>
#include<arc.h>

#define SIZE 3
#define MEM_SIZE 3

struct hash_node hash_array_new[MEM_SIZE];
struct kind_info cache_in[6];
struct arc_node *t1_head = NULL;
struct arc_node *t1_last = NULL;
struct arc_node *t2_head = NULL;
struct arc_node *t2_last = NULL;
struct arc_node *b1_head = NULL;
struct arc_node *b1_last = NULL;
struct arc_node *b2_head = NULL;
struct arc_node *b2_last = NULL;
int p = 0;

/*********************************************************
 Linked list functions for ARC
*********************************************************/

bool kv_isempty(struct arc_node *head2) {
   return head2 == NULL;
}

void displayForward(struct arc_node *head2) {

   struct arc_node *ptr = head2;
	
   printf("\n[ ");
	
   while(ptr != NULL) {        
      printf("(%s,%s) ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
	
   printf(" ]\n");
}

int kv_search(struct arc_node *head2, char *key) {

   struct arc_node *ptr = head2;

   while(ptr != NULL) {
      if(strncmp(ptr->key,key,strlen(key)+1)== 0)
      {
		return 1;
      }
      ptr = ptr->next;
   }
   return 0;
}


struct arc_node* kv_ispresent_t1(char *key)
{
   struct arc_node* current = t1_head;
   struct arc_node* previous = NULL;	
   if(t1_head == NULL) {
      return NULL;
   }
    while(strncmp(current->key,key,strlen(key)+1)!= 0) {
    if(current->next == NULL) {
         return NULL;
      } else {
           previous = current;
	   current = current->next;             
      }
   }
     if(current == t1_head) {
        t1_head = t1_head->next;
   } else {
        current->prev->next = current->next;
   }    
   if(current == t1_last) {
        t1_last = current->prev;
   } else {
      current->next->prev = current->prev;
   }	
   return current;

}

struct arc_node* kv_ispresent_t2(char *key)
{
   struct arc_node* current = t2_head;
   struct arc_node* previous = NULL;
   if(t2_head == NULL) {
      return NULL;
   }
    while(strncmp(current->key,key,strlen(key)+1)!= 0) {
    if(current->next == NULL) {
         return NULL;
      } else {
           previous = current;
           current = current->next;
      }
   }
     if(current == t2_head) {
        t2_head = t2_head->next;
   } else {
        current->prev->next = current->next;
   }
   if(current == t2_last) {
        t2_last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
   return current;

}

struct arc_node* kv_ispresent_b1(char *key)
{
   struct arc_node* current = b1_head;
   struct arc_node* previous = NULL;
   if(b1_head == NULL) {
      return NULL;
   }
    while(strncmp(current->key,key,strlen(key)+1)!= 0) {
    if(current->next == NULL) {
         return NULL;
      } else {
           previous = current;
           current = current->next;
      }
   }
     if(current == b1_head) {
        b1_head = b1_head->next;
   } else {
        current->prev->next = current->next;
   }
   if(current == b1_last) {
        b1_last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
   return current;

}

struct arc_node* kv_ispresent_b2(char *key)
{
   struct arc_node* current = b2_head;
   struct arc_node* previous = NULL;
   if(b2_head == NULL) {
      return NULL;
   }
    while(strncmp(current->key,key,strlen(key)+1)!= 0) {
    if(current->next == NULL) {
         return NULL;
      } else {
           previous = current;
           current = current->next;
      }
   }
     if(current == b2_head) {
        b2_head = b2_head->next;
   } else {
        current->prev->next = current->next;
   }
   if(current == b2_last) {
        b2_last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
   return current;

}

void insertFirst_t1(char *key, char *data) {
   struct arc_node *t1_link = (struct arc_node*)getmem(sizeof(struct arc_node));
   t1_link->key = key;
   t1_link->data = data;	
   if(kv_isempty(t1_head)) {
       t1_last = t1_link;
   } else {
       t1_head->prev = t1_link;
   }
    t1_link->next = t1_head;	
    t1_head = t1_link;
}
void insertFirst_t2(char *key,char *data) {
   struct arc_node *t2_link = (struct arc_node*) getmem(sizeof(struct arc_node));
   t2_link->key = key;
   t2_link->data = data;	
   if(kv_isempty(t2_head)) {
       t2_last = t2_link;
   } else {
       t2_head->prev = t2_link;
   }
    t2_link->next = t2_head;	
    t2_head = t2_link;
}

void insertFirst_b1(char *key,char *data) {
   struct arc_node *b1_link = (struct arc_node*) getmem(sizeof(struct arc_node));
   b1_link->key = key;
   b1_link->data = data;
   b1_link->prev = NULL;
   b1_link->next = NULL;
   if(kv_isempty(b1_head)) {
       b1_last = b1_link;
   } else {
       b1_head->prev = b1_link;
   }
    b1_link->next = b1_head;
    b1_head = b1_link;
}

void insertFirst_b2(char* key,char *data) {
   struct arc_node *b2_link = (struct arc_node*) getmem(sizeof(struct arc_node));
   b2_link->key = key;
   b2_link->data = data;
   if(kv_isempty(b2_head)) {
       b2_last = b2_link;
   } else {
       b2_head->prev = b2_link;
   }
    b2_link->next = b2_head;
    b2_head = b2_link;
}


int length(struct arc_node *head2) 
{
   int length = 0;
   struct arc_node *current = head2;

   for(current = head2; current != NULL; current = current->next){
      length++;
   }
	
   return length;
}

struct arc_node* deleteLast_t1() {
   struct arc_node *tempLink = t1_last;	
   if(t1_head->next == NULL) {
   t1_head = NULL;
   } else {
      t1_last->prev->next = NULL;
   }	
   t1_last = t1_last->prev;	
   return tempLink;
}

struct arc_node* deleteLast_t2() {
      struct arc_node *tempLink = t2_last;	
      if(t2_head->next == NULL) {
      t2_head = NULL;
   } else {
      t2_last->prev->next = NULL;
   }	
   t2_last = t2_last->prev;	
   return tempLink;
}

struct arc_node* deleteLast_b1() {
      struct arc_node *tempLink = b1_last;
      if(b1_head->next == NULL) {
      b1_head = NULL;
   } else {
      b1_last->prev->next = NULL;
   }
   b1_last = b1_last->prev;
   return tempLink;
}

struct arc_node* deleteLast_b2() {
      struct arc_node *tempLink = b2_last;
      if(b2_head->next == NULL) {
      b2_head = NULL;
   } else {
      b2_last->prev->next = NULL;
   }
   b2_last = b2_last->prev;
   return tempLink;
}

/***********************************************************
 Replace function for ARC
***********************************************************/

void replace(char* key,int p)
{
	int index = 0;
	int b2_val = kv_search(b2_head,key);
	if((kv_isempty(b1_head)) && ((length(t1_head)>p) || ( b2_val == 1 && length(t1_head) == p)))
	{
		struct arc_node *temp_node = deleteLast_t1();
                insertFirst_b1(temp_node->key,temp_node->data);
		while(hash_array_new[index].key != NULL) 
		{

	        	if(strncmp(hash_array_new[index].key,temp_node->key,strlen(key)+1) == 0)
	        	{
        			 hash_array_new[index].key = NULL;
        			 hash_array_new[index].value = NULL;
				 xfree(hash_array_new[index].key);
       				 xfree(hash_array_new[index].value);
     			 }
     			 index++;
  	    	 }
	}	
	else
	{
		index = 0;
		struct arc_node *temp_node = deleteLast_t2();
                insertFirst_b2(temp_node->key,temp_node->data);
		while(hash_array_new[index].key != NULL)
                {

                        if(strncmp(hash_array_new[index].key,temp_node->key,strlen(key)+1) == 0)
                        {
                                 hash_array_new[index].key = NULL;
                                 hash_array_new[index].value = NULL;
                                 xfree(hash_array_new[index].key);
                                 xfree(hash_array_new[index].value);
                         }
                         index++;
                 }
		
	}
}


/********************************************************************
Function to initialise the cache table, cache information table and
to create buffer pools of different sizes(8,16,32,64...etc) which
will be used to allocate memory for key and value
********************************************************************/
int kv_init_arc()
{
	int i = 0;
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
	return 1;
}

/******************************************************************
Function to store key-value pair using buffer pools for memory
allocation.
*******************************************************************/

int kv_set_arc(char* input_key,char* input_data) {
   char *kv_data = (char*) xmalloc((int)(strlen(input_data)+1));
   if(kv_data == (char*)SYSERR)
   {
        printf("Buffer pool is full");
        return 1;
   }
   strncpy(kv_data,input_data,strlen(input_data)+1);
   char *kv_key = (char*) xmalloc((int)(strlen(input_key)+1));
    if(kv_key == (char*)SYSERR)
   {
        printf("Buffer pool is full");
        return 1;
   }
   strncpy(kv_key,input_key,strlen(input_key)+1);
   cache_in[3].kind_count = cache_in[3].kind_count + strlen(input_key) + strlen(input_data) + 2;
   int index =0;
   int k = 0;
   int flag = 0;
   while(hash_array_new[index].key != NULL)
   {
        if(strncmp(hash_array_new[index].key,kv_key,strlen(input_key)+1)== 0)
        {
                hash_array_new[index].value = kv_data;
		struct arc_node *t1_node1 = kv_ispresent_t1(kv_key);
		insertFirst_t1(kv_key,kv_data);	
		flag = 1;
                break;
        }
        index++;
   }
   if(index == MEM_SIZE)
   {
        bool del = 0;
        
	if(length(t1_head) ==  SIZE)
         {
                struct arc_node *t1_node = deleteLast_t1(t1_head);
		if(length(b1_head)==SIZE)
		{
			struct arc_node *b1_node = deleteLast_b1();
			insertFirst_b1(t1_node->key,t1_node->data);
		}
		else
		{
			insertFirst_b1(t1_node->key,t1_node->data);
		}
                insertFirst_t1(kv_key,kv_data);
		del = kv_delete_arc(t1_node->key);
         }
	if(del)
        {
            for(k=0;k<SIZE;k++)
            {
                    if(hash_array_new[k].key == NULL)
                    {
                        hash_array_new[k].key = kv_key;
                        hash_array_new[k].value = kv_data;
                        break;
                    }
            }
        }
        cache_in[5].kind_count = cache_in[5].kind_count + 1;
        flag = 1;
   }

   if(flag == 0)
   {
         hash_array_new[index].key = kv_key;
         hash_array_new[index].value = kv_data;
	 insertFirst_t1(kv_key,kv_data); 
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


char* kv_get_arc(char *key)
{
        int index = 0;
        cache_in[1].kind_count = cache_in[1].kind_count + 1;
	while(hash_array_new[index].key!=NULL)
        {
	        if(strncmp(hash_array_new[index].key,key,strlen(key)+1)== 0)
		{
			struct arc_node *temp_node = kv_ispresent_t1(key);
			if((strncmp(temp_node->key,key,strlen(key)+1)== 0) && length(t2_head)>SIZE)
			{
				struct arc_node *temp_node1 = deleteLast_t2(t2_head);
		                insertFirst_b2(temp_node1->key,temp_node1->data);				
				insertFirst_t2(temp_node->key,temp_node->data);
			}
			else
			{
				insertFirst_t2(key,hash_array_new[index].value);
			}	
			struct arc_node *temp_node1 = kv_ispresent_t2(key);			
			if(temp_node1!=NULL)
			{
				insertFirst_t2(key,hash_array_new[index].value);
			}
			cache_in[0].kind_count = cache_in[0].kind_count + 1;
			return hash_array_new[index].value;
		}
		index++;
	}
	int b1_value = kv_search(b1_head,key);
	int b2_value = kv_search(b2_head,key);
	int b1_count = length(b1_head);
        int b2_count = length(b2_head);
	int delta= 0;
	int p = 0;
	if(b1_value == 1)
	{
		cache_in[0].kind_count = cache_in[0].kind_count + 1;
		if(b1_count>=b2_count)
                {
                        delta = 1;
                }
                else
                {
                        delta = b2_count/b1_count;
                }
                if((p+delta)>SIZE)
                {
                        p = p+delta;
                }
                else
                {
                        p = SIZE;
                }
                replace(key,p);
		struct arc_node *temp_node = kv_ispresent_b1(key);
		bool del = 0;
		if(length(t2_head)>SIZE)
		{
			struct arc_node *temp_node1 = deleteLast_t2(t2_head);
			del = kv_delete_arc(temp_node1->key);
                        if(length(b2_head)>SIZE)
			{
				struct arc_node *temp_node2 = deleteLast_b2(t2_head);
				insertFirst_b2(temp_node1->key,temp_node1->data);
			}
			else
			{
				insertFirst_b2(temp_node1->key,temp_node1->data);
			}
			insertFirst_t2(temp_node->key,temp_node->data);	
		}
		else
		{
			insertFirst_t2(temp_node->key,temp_node->data);
		}
		int k = 0;
		if(del)
		{
                    for(k=0;k<SIZE;k++)
                    {
                           if(hash_array_new[k].key == NULL)
                           {
                                   hash_array_new[k].key = temp_node->key;
                                   hash_array_new[k].value = temp_node->data;
                                   break;
                           }
                    }
                 }
                cache_in[5].kind_count = cache_in[5].kind_count + 1;
		return temp_node->data;
	}
	if(b2_value == 1)
	{
		cache_in[0].kind_count = cache_in[0].kind_count + 1;
		if(b1_count>=b2_count)
                {
                        delta = 1;
                }
                else
                {
                        delta = b2_count/b1_count;
                }
                if((p+delta)>SIZE)
                {
                        p = p+delta;
                }
                else
                {
                        p = SIZE;
                }
                replace(key,p);
		struct arc_node *temp_node = kv_ispresent_b2(key);
		bool del = 0;
		if(length(t2_head)>SIZE)
                {
                        struct arc_node *temp_node1 = deleteLast_t2(t2_head);
			del = kv_delete_arc(temp_node1->key);
                        if(length(b2_head)>SIZE)
                        {
                                struct arc_node *temp_node2 = deleteLast_b2(t2_head);
                                insertFirst_b2(temp_node1->key,temp_node1->data);
                        }
                        else
                        {
                                insertFirst_b2(temp_node->key,temp_node->data);
                        }
                        insertFirst_t2(temp_node->key,temp_node->data);
                }
		else
		{
			insertFirst_t2(temp_node->key,temp_node->data);
		}
		int k = 0;
		if(del)
	     	{
        	    for(k=0;k<SIZE;k++)
        	    {
                 	   if(hash_array_new[k].key == NULL)
                   	   {
                   	  	   hash_array_new[k].key = temp_node->key;
                   	  	   hash_array_new[k].value = temp_node->data;
                   	  	   break;
                   	   }
           	    }
       		 }
	        cache_in[5].kind_count = cache_in[5].kind_count + 1;
		return temp_node->data;
	}
	return NULL;
}

/******************************************************************
Function to remove key-value entry from cache which has selected by
cache replacement policies and free the memory used by key and value
to buffer pools for reuse.
*******************************************************************/

bool kv_delete_arc(char* key)
{
   int index = 0;
   while(hash_array_new[index].key != NULL) {

      if(strncmp(hash_array_new[index].key,key,strlen(key)+1) == 0)
      {
         cache_in[3].kind_count = cache_in[3].kind_count - strlen(hash_array_new[index].key) - strlen(hash_array_new[index].value)-2;
         xfree(hash_array_new[index].value);
         xfree(hash_array_new[index].key);
         hash_array_new[index].key = NULL;
         hash_array_new[index].value = NULL;
         cache_in[4].kind_count = cache_in[4].kind_count - 1;
         return 1;
      }
      index++;
   }
   return 0;
}

/*********************************************************************
  Function to reset all the tables like cache table, cache information
  table.
**********************************************************************/

void kv_reset_arc()
{
        int i = 0;
        for(i = 0; i<MEM_SIZE; i++) {

         if(hash_array_new[i].key != NULL)
         {
                xfree(hash_array_new[i].value);
 		xfree(hash_array_new[i].key);
                hash_array_new[i].key = NULL;
                hash_array_new[i].value = NULL;

        }
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

int get_cache_info_arc(char* kind)
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

char **most_popular_keys_arc(int k)
{
	struct arc_node *t2_node  = t2_head; 
        char **most_frequent = (char**)getmem(sizeof(char**)*k);
        int k1 = 0;
        for (k1 = 0; k1 < k; k1++)
        {
                most_frequent[k1] = (char*)getmem(strlen(t2_node->key)+1);
                most_frequent[k1] = t2_node->key;
		t2_node = t2_node->next;
        }
        return most_frequent;
}
