/**************************************************************
Program to implememt asynchronous message passing using future
**************************************************************/

#include <xinu.h>
#include <future.h>
#include <stdlib.h>

/******************************************************************************
Queue implementation and functions for queue operation i.e. enqueue and dequeue
Queue implementation is used for set_queue and get_queue which are waiting 
queues for producer and consumer respectively 
*******************************************************************************/

/*New node is created to hold process id*/

struct Node* newNode(int pid)
{
    struct Node *node = (struct Node*)getmem(sizeof(struct Node));
    node->key = pid;
    node->next = NULL;
    return node;
}

/*New queue is created to add process id nodes*/

struct Queue *createQueue()
{
    struct Queue *q = (struct Queue*)getmem(sizeof(struct Queue));
    q->head = q->tail = NULL;
    return q;
}

/*function to enqueue node into set_queue/get_queue */

void Enqueue(struct Queue *q, int pid)
{
    struct Node *node = newNode(pid);
    if (q->tail == NULL)
    {
       q->head = q->tail = node;
       return;
    }
    q->tail->next = node;
    q->tail = node;
}

/*function to dequeue node into set_queue/get_queue */

struct Node *Dequeue(struct Queue *q)
{
     if (q->head == NULL)
       return NULL;

    struct Node *node = q->head;
    q->head = q->head->next;

    if (q->head == NULL)
       q->tail = NULL;
    return node;
}

/************************************************************
 Function to create new future object and initioazing future
 variables with some default values
*************************************************************/

future_t* future_alloc(future_mode_t mode)
{
        future_t* t1 = (future_t *)getmem(sizeof(future_t));
	if((char*)t1 == (char*)SYSERR) 
	{
                return (future_t*)SYSERR;
        }
	t1->state = FUTURE_EMPTY;
        t1->value = 0;
        t1->mode = mode;
       	t1->pid = getpid();
	t1->set_queue = createQueue();
	t1->get_queue = createQueue();
        return t1;
	
}

/* system call to free the memory*/

syscall future_free(future_t* f)
{
    intmask mask;
    mask = disable();
    
    while((f->get_queue->head!=NULL))
	Dequeue(f->get_queue);

    while((f->set_queue->head!=NULL))
        Dequeue(f->set_queue);

    if(freemem((char*)f, sizeof(future_t)) == SYSERR)
    {
	restore(mask);
        return SYSERR;
    }
    restore(mask);
    return OK;	
}

/*system call used by producer to set the value of future variable*/

syscall future_set(future_t* f, int value)
{
	pid32 pid;
	pid32 current_pid;
	struct Node *n;
	intmask mask;
	mask = disable();

/****************** FUTURE_EXCLUSIVE ************************/

	if(f->mode == FUTURE_EXCLUSIVE)
        {
                if(f->state == FUTURE_EMPTY)
                {
                        f->value = value;
                        f->state = FUTURE_READY;
                }
		if(f->state == FUTURE_WAITING)
                {
                        f->value = value;
                        f->state = FUTURE_READY;
                        n = Dequeue(f->get_queue);
                        pid = n->key;
                        ready(pid);
                }
        }

/********************* FUTURE_SHARED ************************/

	if(f->mode == FUTURE_SHARED)
        {
		if(f->state == FUTURE_READY)
                {
			return SYSERR;
                }
                if(f->state == FUTURE_EMPTY)
                {
			f->value = value;
                        f->state = FUTURE_READY;
			
                }
		if(f->state == FUTURE_WAITING)
                {
                        f->value = value;
                        f->state = FUTURE_READY;
			while((f->get_queue->head!=NULL))
			{			
				n = Dequeue(f->get_queue);
				pid = n->key;
				ready(pid);
			}
                }
        }

/********************* FUTURE_QUEUE ************************/

	if(f->mode == FUTURE_QUEUE)
        {
                if(f->state == FUTURE_READY)
                {
			current_pid = getpid();
			if (isbadpid(current_pid))
                        {
                            restore(mask);
                            return SYSERR;
                        }	
                        Enqueue(f->set_queue,current_pid);
                        proctab[current_pid].prstate = PR_WAIT;
                        resched();
                }
		if(f->state == FUTURE_WAITING)
                {
                        f->value = value;
                        f->state = FUTURE_READY;
                        n = Dequeue(f->get_queue);
                        pid = n->key;
                        ready(pid);
                }
		if(f->state == FUTURE_EMPTY)
                {
                        f->value = value;
                        f->state = FUTURE_READY;
                }
        }
	restore(mask);
	return OK;
}

/*system call used by consumer to fetch the value of future variable*/

syscall future_get(future_t* f, int* value)
{
    pid32 pid;
    pid32 current_pid;
    struct Node *n;
    intmask mask;
    mask = disable();

/********************* FUTURE_EXCLUSIVE ************************/

    if(f->mode == FUTURE_EXCLUSIVE)
    {
	if(f->state == FUTURE_EMPTY)
	{
		f->state = FUTURE_WAITING;
		current_pid = getpid();
                if (isbadpid(current_pid))
                {
                    restore(mask);
                    return SYSERR;
                }
                Enqueue(f->get_queue,current_pid);
                proctab[current_pid].prstate = PR_WAIT;
                resched();
	}
        if(f->state == FUTURE_READY)
        {
            *value = f->value;
	    restore(mask);
	    return OK;
        }	
	
    }

/********************* FUTURE_SHARED ************************/

    if(f->mode == FUTURE_SHARED)
    {
        if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
        {
		f->state = FUTURE_WAITING;
                current_pid = getpid();
                if (isbadpid(current_pid))
                {
                    restore(mask);
                    return SYSERR;
                }
                Enqueue(f->get_queue,current_pid);
                proctab[current_pid].prstate = PR_WAIT;
                resched();
        }
	if(f->state == FUTURE_READY)
        {
            *value = f->value;
	    restore(mask);
            return OK;
        }
    }

/********************** FUTURE_QUEUE ************************/

    if(f->mode == FUTURE_QUEUE)
    {
        if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
        {
                f->state = FUTURE_WAITING;
                current_pid = getpid();
                if (isbadpid(current_pid))
                {
                    restore(mask);
                    return SYSERR;
                }
                Enqueue(f->get_queue,current_pid);
                proctab[current_pid].prstate = PR_WAIT;
                resched();
        }
        if(f->state == FUTURE_READY)
        {
            *value = f->value;
	    if(f->get_queue->head!=NULL)
	    {
		n = Dequeue(f->set_queue);
           	f->state = FUTURE_WAITING;
	   	pid = n->key;
            	ready(pid);
	    }
	    restore(mask);
            return OK;
        }
    }
    restore(mask);
    return OK;
}
