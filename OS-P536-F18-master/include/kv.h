#include<stdio.h>

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
struct arc_node
{
   char *data;
   char *key;
   struct arc_node *next;
   struct arc_node *prev;
};

struct hash_node
{
   char *value;
   char *key;
};

struct kind_info
{
   char* kind_name; 
   int kind_count;   
};

struct cache_info
{
   int total_hits;
   int total_access;
   int total_set_success;
   int cache_size;
   int num_keys;
   int total_enviction;
};

struct LFU
{
   char *lfu_key;
   int frequency;
};

int kv_init();
int kv_set(char*,char*);
void display();
char* kv_get(char*);
bool kv_delete(char*);
void kv_reset();
int get_cache_info(char*);
char** most_popular_keys(int);
struct arc_node* kv_ispresent(struct arc_node*,char*);
