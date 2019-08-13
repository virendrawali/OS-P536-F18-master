#include <xinu.h>
#include <stdio.h>
#include<xmalloc.h>

shellcmd xsh_xmalloc(int nargs,char *args[]) {
	xmalloc_init();
	void *mem1 = xmalloc(7);
	void *mem2 = xmalloc(5);
	void *mem3 = xmalloc(6);
	void *mem4 = xmalloc(13);
	void *mem5 = xmalloc(17);
	void *mem6 = xmalloc(10);
	void *mem7 = xmalloc(30);
	void *mem8 = xmalloc(56);
	xheap_snapshot();
	printf("\n");
	xfree(mem1);
	xheap_snapshot();
	printf("\n");
	xfree(mem5);
	xheap_snapshot();
	printf("\n");
	xfree(mem6);
	xfree(mem2);
	xheap_snapshot();
	printf("\n");
	void *mem9 = xmalloc(1000);
	void *mem10 = xmalloc(215);
	void *mem11 = xmalloc(511);
	void *mem12 = xmalloc(108);
	void *mem14 = xmalloc(4000);
	xheap_snapshot();
        printf("\n");
	xfree(mem3);
	xfree(mem9);
	void *mem13 = xmalloc(976);
	xheap_snapshot();
        printf("\n");
	//void *mem14 = xmalloc(1300);
	//printf("\n");
	void *mem15 = xmalloc(441);
	xheap_snapshot();
        printf("\n");
	return 1;	
}
